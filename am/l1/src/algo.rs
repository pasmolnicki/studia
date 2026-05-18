use crate::tsp::{self, VecPoints, point_distance};
use rand::{Rng, RngExt};
use rand::seq::SliceRandom;
use serde::{Serialize};
use std::fs::File;
use std::io::Write;
use std::path::{PathBuf};
use std::sync::{Arc, Mutex};
use std::{path};
use threadpool::ThreadPool;
use std::sync::mpsc::channel;

#[derive(Debug, Serialize)]
pub struct TspAlgorithmResult {
    pub name: String,
    pub mean_distance: i64,
    pub mean_n_steps: i64,
    pub best_solution: tsp::VecPoints,
}

impl Default for TspAlgorithmResult {
    fn default() -> Self {
        Self { name: "name".to_string(), mean_distance: i64::MAX, mean_n_steps: i64::MAX, best_solution: VecPoints::new() }
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
    let mut file = File::create(&file_path).expect(
        &format!("Couldn't create file: {}", file_path.to_str().unwrap()));
    
    file.write_all(serialized.as_bytes()).expect("Couldn't write to file");
}

// Distance matrix builder - pre-computes distances between all pairs of points
#[derive(Clone)]
pub struct DistanceMatrix {
    matrix: Vec<Vec<i64>>,
}

impl DistanceMatrix {
    fn new(points: &[(f64, f64)]) -> Self {
        let n = points.len();
        let mut matrix = vec![vec![0i64; n]; n];

        for i in 0..n {
            for j in (i + 1)..n {
                let dist = point_distance(points[i], points[j]) as i64;
                matrix[i][j] = dist;
                matrix[j][i] = dist;
            }
        }

        DistanceMatrix { matrix }
    }

    fn get(&self, i: usize, j: usize) -> i64 {
        self.matrix[i][j]
    }

    fn calculate_tour_length(&self, route: &[usize]) -> i64 {
        let n = route.len();
        let mut sum = 0i64;
        for i in 1..n {
            sum += self.get(route[i - 1], route[i]);
        }
        sum += self.get(route[n - 1], route[0]);
        sum
    }
}

// Invert move: reverses the sequence from index i to j
fn apply_invert(route: &mut [usize], i: usize, j: usize) {
    let mut left = i;
    let mut right = j;
    while left < right {
        route.swap(left, right);
        left += 1;
        right -= 1;
    }
}

// Calculate the delta (cost change) of inverting from i to j
fn calculate_invert_delta(dist_matrix: &DistanceMatrix, route: &[usize], i: usize, j: usize) -> i64 {
    let n = route.len();
    let im1 = (i as i32 - 1).rem_euclid(n as i32) as usize;
    let jp1 = (j + 1) % n;

    let old_cost = dist_matrix.get(route[im1], route[i]) + dist_matrix.get(route[j], route[jp1]);
    let new_cost = dist_matrix.get(route[im1], route[j]) + dist_matrix.get(route[i], route[jp1]);

    new_cost - old_cost
}

// Transpose move: swaps elements at positions i and j
fn apply_transpose(route: &mut [usize], i: usize, j: usize) {
    route.swap(i, j);
}

// Calculate the delta (cost change) of transposing positions i and j
fn calculate_transpose_delta(dist_matrix: &DistanceMatrix, route: &[usize], i: usize, j: usize) -> i64 {
    let n = route.len();
    let im1 = (i as i32 - 1).rem_euclid(n as i32) as usize;
    let ip1 = (i + 1) % n;
    let jm1 = (j as i32 - 1).rem_euclid(n as i32) as usize;
    let jp1 = (j + 1) % n;

    let a = route[i];
    let b = route[j];

    if (i + 1 == j) || (i == 0 && j == n - 1) {
        let first_idx = if i == 0 && j == n - 1 { j } else { i };
        let second_idx = if i == 0 && j == n - 1 { i } else { j };
        let prev = (first_idx as i32 - 1).rem_euclid(n as i32) as usize;
        let next = (second_idx + 1) % n;

        let old_cost = dist_matrix.get(route[prev], route[first_idx]) + dist_matrix.get(route[second_idx], route[next]);
        let new_cost = dist_matrix.get(route[prev], route[second_idx]) + dist_matrix.get(route[first_idx], route[next]);
        new_cost - old_cost
    } else {
        let old_cost = dist_matrix.get(route[im1], a) + dist_matrix.get(a, route[ip1])
            + dist_matrix.get(route[jm1], b) + dist_matrix.get(b, route[jp1]);
        let new_cost = dist_matrix.get(route[im1], b) + dist_matrix.get(b, route[ip1])
            + dist_matrix.get(route[jm1], a) + dist_matrix.get(a, route[jp1]);
        new_cost - old_cost
    }
}

fn print_starting(algo_name: &str, data_set: &str, print: bool) {
    if print {
        println!("\x1b[1;36m[{}] Starting optimization on {} vertices\x1b[0m", algo_name, data_set);
    }
}

fn print_progress(tag: &str, print: bool, n: usize, best_distance: i64, i: usize) {
    if print && (i % 10 == 0 || i == n - 1) {
        let progress = ((i + 1) as f64 / n as f64 * 100.0) as u32;
        let bar_width = 30;
        let filled = (progress as usize * bar_width / 100).min(bar_width);
        let bar = format!("[{}{}]", "=".repeat(filled), " ".repeat(bar_width - filled));
        print!("\rx1b[1;34m[{}]\x1b[0m {} {}/{} ({}%) | Best: {}", tag, bar, i + 1, n, progress, best_distance);
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
        where Self: Clone + Send + Sync + 'static 
    {
                let dist_matrix = DistanceMatrix::new(&data.points.points);
        let n = data.points.points.len();

        print_starting(self.name(), &data.name, print);

        let points = Arc::new(data.points.clone());
        let dist_matrix = Arc::new(dist_matrix.clone());
        let shared = Arc::new(Mutex::new(
            SharedProcedureState {
                total_distance: 0,
                total_steps: 0,
                best_distance: i64::MAX,
                best_solution: data.points.clone(),
            }
        ));

        let pool = ThreadPool::new(16);
        let (tx, rx) = channel();
        let algo = Arc::new(self.clone());

        for _i in 0..n_jobs {
            let tx = tx.clone();
            let points = points.clone();
            let dist_matrix = dist_matrix.clone();
            let algo = algo.clone();
            let shared = shared.clone();

            pool.execute(move ||{
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
            print_progress(self.name(), print, n_jobs, shared.lock().unwrap().best_distance, i);
        }

        print_complete(self.name(), print);

        let shared = shared.lock().unwrap();

        TspAlgorithmResult {
            name: format!("{}_{}", self.name(), data.name),
            mean_distance: shared.total_distance / n as i64,
            mean_n_steps: (shared.total_steps / n as u64) as i64,
            best_solution: shared.best_solution.clone(),
        }
    }
}

pub struct LocalSearchZ1;

impl TspProcedure for LocalSearchZ1 {
    fn name(&self) -> &str {
        "LocalSearchZ1"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        let mut route: Vec<usize> = (0..points.points.len()).collect();
        let mut rng = rand::rng();
        route.shuffle(&mut rng);

        let mut len = dist_matrix.calculate_tour_length(&route);
        let mut steps = 0u64;

        let improved_flag = true;
        while improved_flag {
            let n = route.len();

            // let best_move = (0..(n - 1))
            //     .into_par_iter()
            //     .flat_map(|i| {
            //         ((i + 1)..n)
            //             .into_par_iter()
            //             .map(move |j| (i, j))
            //     })
            //     .filter(|(i, j)| !(*i == 0 && *j == n - 1))
            //     .map(|(i, j)| {
            //         let delta = calculate_invert_delta(dist_matrix, &route, i, j);
            //         (delta, i, j)
            //     })
            //     .min_by_key(|(delta, _, _)| *delta);

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
                    len += best_delta;
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
            distance: len,
            n_steps: steps,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        let dist_matrix = DistanceMatrix::new(&data.points.points);
        let n = data.points.points.len();

        let mut total_distance = 0i64;
        let mut total_steps = 0u64;
        let mut best_distance = i64::MAX;
        let mut best_solution = data.points.clone();

        if print {
            println!("\x1b[1;36m[LocalSearchZ1] Starting optimization on {} vertices\x1b[0m", data.name);
        }

        for i in 0..n {
            let result = self.algo(&data.points, &dist_matrix);
            total_distance += result.distance;
            total_steps += result.n_steps;

            if result.distance < best_distance {
                best_distance = result.distance;
                best_solution = result.solution;
            }

            print_progress("Z1", print, n, best_distance, i);
        }

        if print {
            println!("\n\x1b[1;32m[LocalSearchZ1] Complete\x1b[0m");
        }

        TspAlgorithmResult {
            name: format!("LocalSearchZ1_{}", data.name),
            mean_distance: total_distance / n as i64,
            mean_n_steps: (total_steps / n as u64) as i64,
            best_solution,
        }
    }
}



pub struct LocalSearchZ2;

impl TspProcedure for LocalSearchZ2 {
    fn name(&self) -> &str {
        "LocalSearchZ2"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        let mut route: Vec<usize> = (0..points.points.len()).collect();
        let mut rng = rand::rng();
        route.shuffle(&mut rng);

        let mut len = dist_matrix.calculate_tour_length(&route);
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
                len += best_delta;
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
            distance: len,
            n_steps: steps,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        let dist_matrix = DistanceMatrix::new(&data.points.points);
        let n = data.points.points.len();

        let mut total_distance = 0i64;
        let mut total_steps = 0u64;
        let mut best_distance = i64::MAX;
        let mut best_solution = data.points.clone();

        print_starting("LocalSearchZ2", &data.name, print);

        for i in 0..n {
            let result = self.algo(&data.points, &dist_matrix);
            total_distance += result.distance;
            total_steps += result.n_steps;

            if result.distance < best_distance {
                best_distance = result.distance;
                best_solution = result.solution;
            }

            print_progress("Z2", print, n, best_distance, i);
        }

        if print {
            println!("\n\x1b[1;32m[LocalSearchZ2] Complete\x1b[0m");
        }

        TspAlgorithmResult {
            name: format!("LocalSearchZ2_{}", data.name),
            mean_distance: total_distance / n as i64,
            mean_n_steps: (total_steps / n as u64) as i64,
            best_solution,
        }
    }
}

pub struct LocalSearchZ3;

impl TspProcedure for LocalSearchZ3 {
    fn name(&self) -> &str {
        "LocalSearchZ3"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        let mut route: Vec<usize> = (0..points.points.len()).collect();
        let mut rng = rand::rng();
        route.shuffle(&mut rng);

        let mut len = dist_matrix.calculate_tour_length(&route);
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
                len += best_delta;
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
            distance: len,
            n_steps: steps,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        let dist_matrix = DistanceMatrix::new(&data.points.points);
        let n = data.points.points.len();

        let mut total_distance = 0i64;
        let mut total_steps = 0u64;
        let mut best_distance = i64::MAX;
        let mut best_solution = data.points.clone();

        print_starting("LocalSearchZ3", &data.name, print);

        for i in 0..n {
            let result = self.algo(&data.points, &dist_matrix);
            total_distance += result.distance;
            total_steps += result.n_steps;

            if result.distance < best_distance {
                best_distance = result.distance;
                best_solution = result.solution;
            }

            print_progress("Z3", print, n, best_distance, i);
        }

        if print {
            println!("\n\x1b[1;32m[LocalSearchZ3] Complete\x1b[0m");
        }

        TspAlgorithmResult {
            name: format!("LocalSearchZ3_{}", data.name),
            mean_distance: total_distance / n as i64,
            mean_n_steps: (total_steps / n as u64) as i64,
            best_solution,
        }
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
    pub fn new(initial_temperature: f64, cooling_factor: f64, epoch: usize, no_improve_limit: u32) -> Self {
        Self {
            params: SimulatedAnnealingParams {
                initial_temperature,
                cooling_factor,
                epoch_length: epoch,
                no_improve_limit,
            }
        }
    }

    pub fn random_inversion(rng: &mut dyn  Rng,  n: usize) -> (usize, usize) {
        let i = rng.random_range(0..n - 1);
        (i, rng.random_range(i + 1..n))
    }
}

impl TspProcedure for SimulatedAnnealingBase {
    fn name(&self) -> &str {
        "SimulatedAnnealing"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        let mut route: Vec<usize> = (0..points.points.len()).collect();
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
                let (i, j) = 
                    SimulatedAnnealingBase::random_inversion(&mut rng, route.len());
                let delta = calculate_invert_delta(dist_matrix, &route, i, j);

                let accept = if delta < 0 { 
                    true 
                } else {
                    let boltzman = (-delta as f64 / temperature.max(0.01))
                        .min(MAX_EXPONENT).exp();
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
            distance: best_tour_len,
            n_steps: steps as u64,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        self.run_multithreaded(data, print, 100)
    }
}


pub struct TabuSearchParams {
    pub tabu_tenure: usize,
    pub max_iterations: usize,
    pub no_improve_limit: u64,
}

impl Default for TabuSearchParams {
    fn default() -> Self {
        Self {
            tabu_tenure: 0, // Will be set to n/2 in algo
            max_iterations: 500,
            no_improve_limit: 100,
        }
    }
}

// Tabu Search - search with memory of recent moves
pub struct TabuSearchBase {
    pub params: TabuSearchParams,
}

impl Default for TabuSearchBase {
    fn default() -> Self {
        Self {
            params: TabuSearchParams::default(),
        }
    }
}

impl TabuSearchBase {
    pub fn new(tabu_tenure: usize, max_iterations: usize, no_improve_limit: u64) -> Self {
        Self {
            params: TabuSearchParams {
                tabu_tenure,
                max_iterations,
                no_improve_limit,
            }
        }
    }
}

impl TspProcedure for TabuSearchBase {
    fn name(&self) -> &str {
        "TabuSearch"
    }

    fn algo(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> AlgoSearchResult {
        let mut route: Vec<usize> = (0..points.points.len()).collect();
        let mut rng = rand::rng();
        route.shuffle(&mut rng);

        let mut len = dist_matrix.calculate_tour_length(&route);
        let mut best_len = len;
        let mut best_route = route.clone();
        let mut steps = 0u64;
        let mut no_improve_count = 0u64;

        // Tabu list: FIFO queue storing (i, j) move pairs
        let mut tabu_list: Vec<(usize, usize)> = Vec::with_capacity(self.params.tabu_tenure);
        let actual_tenure = if self.params.tabu_tenure > 0 {
            self.params.tabu_tenure
        } else {
            (points.points.len() / 2).max(5)
        };

        let n = route.len();
        let mut iteration = 0;

        while iteration < self.params.max_iterations && no_improve_count < self.params.no_improve_limit {
            let mut best_move_delta = i64::MAX;
            let mut best_move = (0, 1);

            // Search all neighbors
            for i in 0..(n - 1) {
                for j in (i + 1)..n {
                    if i == 0 && j == n - 1 {
                        continue;
                    }

                    let delta = calculate_invert_delta(dist_matrix, &route, i, j);
                    let new_len = len + delta;

                    // Check if this move is tabu
                    let is_tabu = tabu_list.contains(&(i, j)) || tabu_list.contains(&(j, i));

                    // Accept if: not tabu, OR tabu but improves global best (aspiration)
                    let accept = !is_tabu || (new_len < best_len);

                    if accept && delta < best_move_delta {
                        best_move_delta = delta;
                        best_move = (i, j);
                        let _ = is_tabu && new_len < best_len;
                    }
                }
            }

            // Apply best move
            if best_move_delta < i64::MAX {
                apply_invert(&mut route, best_move.0, best_move.1);
                len += best_move_delta;
                steps += 1;

                // Update tabu list (FIFO)
                tabu_list.push(best_move);
                if tabu_list.len() > actual_tenure {
                    tabu_list.remove(0);
                }

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
            distance: best_len,
            n_steps: steps,
            solution,
        }
    }

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult {
        let dist_matrix = DistanceMatrix::new(&data.points.points);
        let n = data.points.points.len();

        let mut total_distance = 0i64;
        let mut total_steps = 0u64;
        let mut best_distance = i64::MAX;
        let mut best_solution = data.points.clone();

        print_starting("TabuSearch", &data.name, print);

        for i in 0..n {
            let result = self.algo(&data.points, &dist_matrix);
            total_distance += result.distance;
            total_steps += result.n_steps;

            if result.distance < best_distance {
                best_distance = result.distance;
                best_solution = result.solution;
            }

            print_progress("TSP", print, n, best_distance, i);
        }

        print_complete("TabuSearch", print);

        TspAlgorithmResult {
            name: format!("TabuSearch_{}", data.name),
            mean_distance: total_distance / n as i64,
            mean_n_steps: (total_steps / n as u64) as i64,
            best_solution,
        }
    }
}