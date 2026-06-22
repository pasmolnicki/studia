use crate::tsp::{
    self, DistanceMatrix, VecPoints, apply_invert, apply_transpose, calculate_invert_delta,
    calculate_transpose_delta, rand_tour,
};
use crate::utils::TabooList;
use rand::seq::SliceRandom;
use rand::{Rng, RngExt};
use serde::Serialize;
use std::fs::File;
use std::io::Write;
use std::path;
use std::path::PathBuf;
use std::sync::mpsc::channel;
use std::sync::{Arc, Mutex};
use threadpool::ThreadPool;

#[derive(Debug, Serialize)]
pub struct TspAlgorithmResult {
    pub name: String,
    pub mean_distance: i64,
    pub mean_n_steps: i64,
    pub best_solution: tsp::VecPoints,
}

impl Default for TspAlgorithmResult {
    fn default() -> Self {
        Self {
            name: "name".to_string(),
            mean_distance: i64::MAX,
            mean_n_steps: i64::MAX,
            best_solution: VecPoints::new(),
        }
    }
}

pub struct AlgoSearchResult {
    pub distance: i64,
    pub n_steps: u64,
    pub solution: tsp::VecPoints,
}

pub trait NamedObject {
    fn name(&self) -> &str;
}

impl NamedObject for TspAlgorithmResult {
    fn name(&self) -> &str {
        &self.name
    }
}

impl NamedObject for tsp::VecPoints {
    fn name(&self) -> &str {
        &self.name
    }
}

pub fn save_to_file<T: NamedObject + serde::Serialize>(result: T, file_name: &str) {
    let serialized = serde_json::to_string(&result).unwrap();
    let path = path::absolute(PathBuf::from(file_name)).unwrap();
    let file_path = path.join(format!("{}.json", result.name()));
    let mut file = File::create(&file_path).expect(&format!(
        "Couldn't create file: {}",
        file_path.to_str().unwrap()
    ));

    file.write_all(serialized.as_bytes())
        .expect("Couldn't write to file");
}

// Distance matrix builder - pre-computes distances between all pairs of points

fn print_starting(algo_name: &str, data_set: &str, print: bool) {
    if print {
        println!(
            "\x1b[1;36m[{}] Starting optimization on {} vertices\x1b[0m",
            algo_name, data_set
        );
    }
}

fn print_progress(tag: &str, print: bool, n: usize, best_distance: i64, i: usize) {
    if print && (i % 10 == 0 || i == n - 1) {
        let progress = ((i + 1) as f64 / n as f64 * 100.0) as u32;
        let bar_width = 30;
        let filled = (progress as usize * bar_width / 100).min(bar_width);
        let bar = format!("[{}{}]", "=".repeat(filled), " ".repeat(bar_width - filled));
        print!(
            "\r\x1b[1;34m[{}]\x1b[0m {} {}/{} ({}%) | Best: {}",
            tag,
            bar,
            i + 1,
            n,
            progress,
            best_distance
        );
        std::io::Write::flush(&mut std::io::stdout()).ok();
    }
}

fn print_complete(algo_name: &str, print: bool) {
    if print {
        println!("\n\x1b[1;32m[{algo_name}] Complete\x1b[0m");
    }
}

pub trait TspProcedure {
    fn algo(&self, points: &tsp::VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult;

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult;
    fn name(&self) -> &str;

    fn run_multithreaded(&self, data: &tsp::Data, print: bool, n_jobs: usize) -> TspAlgorithmResult
    where
        Self: Clone + Send + Sync + 'static,
    {
        print_starting(self.name(), &data.name, print);

        let dist_matrix = DistanceMatrix::new(&data.points.points);
        let points = Arc::new(data.points.clone());
        let dist_matrix = Arc::new(dist_matrix.clone());
        let shared = Arc::new(Mutex::new(SharedProcedureState {
            total_distance: 0,
            total_steps: 0,
            best_distance: i64::MAX,
            best_solution: data.points.clone(),
        }));

        let pool = ThreadPool::new(16);
        let (tx, rx) = channel();
        let algo = Arc::new(self.clone());

        for _i in 0..n_jobs {
            let tx = tx.clone();
            let points = points.clone();
            let dist_matrix = dist_matrix.clone();
            let algo = algo.clone();
            let shared = shared.clone();

            pool.execute(move || {
                let result = algo.algo(&points, &dist_matrix);
                let mut shared = shared.lock().unwrap();
                shared.total_distance += result.distance;
                shared.total_steps += result.n_steps;

                if result.distance < shared.best_distance {
                    shared.best_distance = result.distance;
                    shared.best_solution = result.solution;
                }

                tx.send(1).expect("Couldn't send on tx");
            });
        }

        for i in 0..n_jobs {
            rx.recv().expect(&format!("Didn't receive value at {i}"));
            print_progress(
                self.name(),
                print,
                n_jobs,
                shared.lock().unwrap().best_distance,
                i,
            );
        }

        print_complete(self.name(), print);

        let shared = shared.lock().unwrap();

        TspAlgorithmResult {
            name: format!("{}_{}", self.name(), data.name),
            mean_distance: shared.total_distance / n_jobs as i64,
            mean_n_steps: (shared.total_steps / n_jobs as u64) as i64,
            best_solution: shared.best_solution.clone(),
        }
    }
}

#[derive(Clone, Copy)]
pub struct LocalSearchZ1;

impl TspProcedure for LocalSearchZ1 {
    fn name(&self) -> &str {
        "LocalSearchZ1"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        let mut route: Vec<usize> = (0..points.points.len()).collect();
        let mut rng = rand::rng();
        route.shuffle(&mut rng);

        let mut steps = 0u64;

        let improved_flag = true;
        while improved_flag {
            let n = route.len();

            let mut best_move: Option<(i64, usize, usize)> = None;
            for i in 0..(n - 1) {
                for j in (i + 1)..n {
                    if i == 0 && j == n - 1 {
                        continue;
                    }
                    let delta = calculate_invert_delta(dist_matrix, &route, i, j);
                    if best_move.is_none() || delta < best_move.unwrap().0 {
                        best_move = Some((delta, i, j));
                    }
                }
            }

            match best_move {
                Some((best_delta, best_i, best_j)) if best_delta < 0 => {
                    apply_invert(&mut route, best_i, best_j);
                    steps += 1;
                }
                _ => break,
            }
        }

        let solution = VecPoints {
            points: route.iter().map(|&idx| points.points[idx]).collect(),
            name: points.name.clone(),
        };

        AlgoSearchResult {
            distance: solution.calc_distance() as i64,
            n_steps: steps,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        self.run_multithreaded(data, print, 100)
    }
}

#[derive(Clone, Copy)]
pub struct LocalSearchZ2;

impl TspProcedure for LocalSearchZ2 {
    fn name(&self) -> &str {
        "LocalSearchZ2"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        let mut route: Vec<usize> = (0..points.points.len()).collect();
        let mut rng = rand::rng();
        route.shuffle(&mut rng);

        let mut steps = 0u64;

        let improved_flag = true;
        while improved_flag {
            let mut best_delta = 0i64;
            let mut best_i = 0;
            let mut best_j = 1;
            let mut found_improvement = false;

            let n = route.len();
            for _ in 0..n {
                let i = rng.random_range(0..n);
                let j = rng.random_range(0..n);

                if i >= j || (i == 0 && j == n - 1) {
                    continue;
                }

                let d = calculate_invert_delta(dist_matrix, &route, i, j);
                if d < best_delta {
                    best_delta = d;
                    best_i = i;
                    best_j = j;
                    found_improvement = true;
                }
            }

            if found_improvement && best_delta < 0 {
                apply_invert(&mut route, best_i, best_j);
                steps += 1;
            } else {
                break;
            }
        }

        let solution = VecPoints {
            points: route.iter().map(|&idx| points.points[idx]).collect(),
            name: points.name.clone(),
        };

        AlgoSearchResult {
            distance: solution.calc_distance() as i64,
            n_steps: steps,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        self.run_multithreaded(data, print, 100)
    }
}

#[derive(Clone, Copy)]
pub struct LocalSearchZ3;

impl TspProcedure for LocalSearchZ3 {
    fn name(&self) -> &str {
        "LocalSearchZ3"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        let mut route: Vec<usize> = (0..points.points.len()).collect();
        let mut rng = rand::rng();
        route.shuffle(&mut rng);

        let mut steps = 0u64;

        let improved_flag = true;
        while improved_flag {
            let mut best_delta = 0i64;
            let mut best_i = 0;
            let mut best_j = 1;
            let mut found_improvement = false;

            let n = route.len();
            for i in 0..(n - 1) {
                for j in (i + 1)..n {
                    let d = calculate_transpose_delta(dist_matrix, &route, i, j);
                    if d < best_delta {
                        best_delta = d;
                        best_i = i;
                        best_j = j;
                        found_improvement = true;
                    }
                }
            }

            if found_improvement && best_delta < 0 {
                apply_transpose(&mut route, best_i, best_j);
                steps += 1;
            } else {
                break;
            }
        }

        let solution = VecPoints {
            points: route.iter().map(|&idx| points.points[idx]).collect(),
            name: points.name.clone(),
        };

        AlgoSearchResult {
            distance: solution.calc_distance() as i64,
            n_steps: steps,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        self.run_multithreaded(data, print, 100)
    }
}

#[derive(Clone, Copy)]
pub struct SimulatedAnnealingParams {
    pub initial_temperature: f64,
    pub cooling_factor: f64,
    pub epoch_length: usize,
    pub no_improve_limit: u32,
}

impl Default for SimulatedAnnealingParams {
    fn default() -> Self {
        Self {
            initial_temperature: 20.0,
            cooling_factor: 0.9,
            epoch_length: 100,
            no_improve_limit: 30,
        }
    }
}

#[derive(Clone)]
struct SharedProcedureState {
    total_distance: i64,
    total_steps: u64,
    best_distance: i64,
    best_solution: tsp::VecPoints,
}

// Simulated Annealing - metaheuristic that accepts worse solutions with decreasing probability
#[derive(Clone)]
pub struct SimulatedAnnealingBase {
    pub params: SimulatedAnnealingParams,
}

impl Default for SimulatedAnnealingBase {
    fn default() -> Self {
        Self {
            params: SimulatedAnnealingParams::default(),
        }
    }
}

impl SimulatedAnnealingBase {
    pub fn new(
        initial_temperature: f64,
        cooling_factor: f64,
        epoch: usize,
        no_improve_limit: u32,
    ) -> Self {
        Self {
            params: SimulatedAnnealingParams {
                initial_temperature,
                cooling_factor,
                epoch_length: epoch,
                no_improve_limit,
            },
        }
    }

    pub fn random_inversion(rng: &mut dyn Rng, n: usize) -> (usize, usize) {
        let i = rng.random_range(0..n - 1);
        (i, rng.random_range(i + 1..n))
    }
}

impl TspProcedure for SimulatedAnnealingBase {
    fn name(&self) -> &str {
        "SimulatedAnnealing"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        let mut n_points = points.points.len();
        let mut route: Vec<usize> = (0..n_points).collect();
        let mut rng = rand::rng();
        route.shuffle(&mut rng);

        const MAX_EXPONENT: f64 = 10.0;

        let mut tour_len = dist_matrix.calculate_tour_length(&route);
        let mut best_tour_len = tour_len;
        let mut steps = 0usize;
        let mut best_route = route.clone();
        let mut temperature = self.params.initial_temperature;
        let no_improve_limit = self.params.no_improve_limit;
        let mut no_improvement_count = 0u32;

        loop {
            let mut improved = false;
            for _epoch in 0..self.params.epoch_length {
                let (i, j) = SimulatedAnnealingBase::random_inversion(&mut rng, route.len());
                let delta = calculate_invert_delta(dist_matrix, &route, i, j);

                let accept = if delta < 0 {
                    true
                } else {
                    let boltzman = (-delta as f64 / temperature.max(0.01))
                        .min(MAX_EXPONENT)
                        .exp();
                    rng.random::<f64>() < boltzman
                };

                if accept {
                    apply_invert(&mut route, i, j);
                    tour_len += delta;

                    if tour_len < best_tour_len {
                        improved = true;
                        best_tour_len = tour_len;
                        best_route = route.clone();
                    }
                }
            }

            // Cool off
            temperature *= self.params.cooling_factor;

            if !improved {
                no_improvement_count += 1;
            }

            steps += self.params.epoch_length;
            if no_improvement_count >= no_improve_limit {
                break;
            }
        }

        let solution = VecPoints {
            points: best_route.iter().map(|&idx| points.points[idx]).collect(),
            name: points.name.clone(),
        };

        AlgoSearchResult {
            distance: solution.calc_distance() as i64,
            n_steps: steps as u64,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        self.run_multithreaded(data, print, 100)
    }
}

#[derive(Clone, Copy)]
pub struct TabooSearchParams {
    pub taboo_tenure: fn(usize) -> usize,
    pub max_iterations: usize,
    pub no_improve_limit: u64,
}

impl Default for TabooSearchParams {
    fn default() -> Self {
        Self {
            taboo_tenure: |n| n.isqrt(), // Will be set to n/2 in algo
            max_iterations: 70,
            no_improve_limit: 5,
        }
    }
}

// Taboo Search - search with memory of recent moves
#[derive(Clone)]
pub struct TabooSearchBase {
    pub params: TabooSearchParams,
}

impl Default for TabooSearchBase {
    fn default() -> Self {
        Self {
            params: TabooSearchParams::default(),
        }
    }
}

impl TabooSearchBase {
    pub fn new(
        taboo_tenure: fn(usize) -> usize,
        max_iterations: usize,
        no_improve_limit: u64,
    ) -> Self {
        Self {
            params: TabooSearchParams {
                taboo_tenure,
                max_iterations,
                no_improve_limit,
            },
        }
    }
}

impl TspProcedure for TabooSearchBase {
    fn name(&self) -> &str {
        "TabooSearch"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        // Step 1: generate initial route
        let n = points.points.len();
        let mut route = rand_tour(points);
        let mut len = dist_matrix.calculate_tour_length(&route);
        let mut best_len = len;
        let mut best_route = route.clone();
        let mut steps = 0u64;
        let mut no_improve_count = 0u64;
        let mut taboo_list = TabooList::new((self.params.taboo_tenure)(n));
        let mut iteration = 0;

        while iteration < self.params.max_iterations
            && no_improve_count < self.params.no_improve_limit
        {
            let mut best_move_delta = i64::MAX;
            let mut best_move = (0, 1);

            // Step 2: Find best possible move in the whole search space
            for i in 0..(n - 1) {
                for j in (i + 1)..n {
                    if i == 0 && j == n - 1 {
                        continue;
                    }

                    let delta = calculate_invert_delta(dist_matrix, &route, i, j);
                    let new_len = len + delta;

                    // Check if this move is taboo
                    let is_taboo = taboo_list.find((i, j));

                    // Step 2.1: If the route is not in the taboo or is just better accept it
                    let accept = !is_taboo || (new_len < best_len);

                    if accept && delta < best_move_delta {
                        best_move_delta = delta;
                        best_move = (i, j);
                    }
                }
            }

            // Step 3: Apply best possible move
            if best_move_delta < i64::MAX {
                apply_invert(&mut route, best_move.0, best_move.1);
                len += best_move_delta;
                steps += 1;

                // Update taboo list (FIFO)
                taboo_list.insert(best_move);

                // Check if improved global best
                if len < best_len {
                    best_len = len;
                    best_route = route.clone();
                    no_improve_count = 0;
                } else {
                    no_improve_count += 1;
                }
            } else {
                break;
            }

            iteration += 1;
        }

        let solution = VecPoints {
            points: best_route.iter().map(|&idx| points.points[idx]).collect(),
            name: points.name.clone(),
        };

        AlgoSearchResult {
            distance: solution.calc_distance() as i64,
            n_steps: steps,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        self.run_multithreaded(data, print, 100)
    }
}
