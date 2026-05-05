use crate::tsp::{self, VecPoints, point_distance};
use rand::{Rng, RngExt};
use rand::seq::SliceRandom;
use rayon::prelude::*;
use serde::{Serialize};
use std::fs::File;
use std::io::Write;
use std::path::{PathBuf};
use std::{path};

#[derive(Debug, Serialize)]
pub struct TspAlgorithmResult {
    pub name: String,
    pub mean_distance: i64,
    pub mean_n_steps: i64,
    pub best_solution: tsp::VecPoints,
}

pub struct LocalSearchResult {
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

fn gen_permuntations(data: &tsp::Data, n: usize, rng: &mut dyn Rng) -> Vec<tsp::VecPoints> {
    (0..n).map(|_| data.points.permutation(rng)).collect()
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

/*
Zadanie 1. Wykonaj algorytm Local Search dla n losowych permutacji (n to liczba wierzchołków).
Dla każdych danych podaj średnią wartość uzyskanego rozwiązania, 
średnią liczbę kroków poprawy oraz najlepsze uzyskane rozwiązanie.
*/

pub trait TspProcedure {
    fn local_search(&self, points: &tsp::VecPoints, dist_matrix: &DistanceMatrix) -> LocalSearchResult;

    fn run(&self, data: &tsp::Data, print: bool) -> TspAlgorithmResult;
}

pub struct LocalSearchZ1;

impl TspProcedure for LocalSearchZ1 {
    fn local_search(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> LocalSearchResult {
        let mut route: Vec<usize> = (0..points.points.len()).collect();
        let mut rng = rand::rng();
        route.shuffle(&mut rng);

        let mut len = dist_matrix.calculate_tour_length(&route);
        let mut steps = 0u64;

        let improved_flag = true;
        while improved_flag {
            let n = route.len();

            // Parallel evaluation of all neighborhood moves
            // Generate all candidate (i, j) pairs and evaluate them in parallel
            let best_move = (0..(n - 1))
                .into_par_iter()
                .flat_map(|i| {
                    ((i + 1)..n)
                        .into_par_iter()
                        .map(move |j| (i, j))
                })
                .filter(|(i, j)| !(*i == 0 && *j == n - 1))
                .map(|(i, j)| {
                    let delta = calculate_invert_delta(dist_matrix, &route, i, j);
                    (delta, i, j)
                })
                .min_by_key(|(delta, _, _)| *delta);

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

        LocalSearchResult {
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
            let result = self.local_search(&data.points, &dist_matrix);
            total_distance += result.distance;
            total_steps += result.n_steps;

            if result.distance < best_distance {
                best_distance = result.distance;
                best_solution = result.solution;
            }

            if print && (i % 10 == 0 || i == n - 1) {
                let progress = ((i + 1) as f64 / n as f64 * 100.0) as u32;
                let bar_width = 30;
                let filled = (progress as usize * bar_width / 100).min(bar_width);
                let bar = format!("[{}{}]", "=".repeat(filled), " ".repeat(bar_width - filled));
                print!("\r\x1b[1;33m[Z1]\x1b[0m {} {}/{} ({}%) | Best: {}", bar, i + 1, n, progress, best_distance);
                std::io::Write::flush(&mut std::io::stdout()).ok();
            }
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
    fn local_search(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> LocalSearchResult {
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

        LocalSearchResult {
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
            println!("\x1b[1;36m[LocalSearchZ2] Starting optimization on {} vertices\x1b[0m", data.name);
        }

        for i in 0..n {
            let result = self.local_search(&data.points, &dist_matrix);
            total_distance += result.distance;
            total_steps += result.n_steps;

            if result.distance < best_distance {
                best_distance = result.distance;
                best_solution = result.solution;
            }

            if print && (i % 10 == 0 || i == n - 1) {
                let progress = ((i + 1) as f64 / n as f64 * 100.0) as u32;
                let bar_width = 30;
                let filled = (progress as usize * bar_width / 100).min(bar_width);
                let bar = format!("[{}{}]", "=".repeat(filled), " ".repeat(bar_width - filled));
                print!("\r\x1b[1;35m[Z2]\x1b[0m {} {}/{} ({}%) | Best: {}", bar, i + 1, n, progress, best_distance);
                std::io::Write::flush(&mut std::io::stdout()).ok();
            }
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
    fn local_search(&self, points: &VecPoints, dist_matrix: &DistanceMatrix) -> LocalSearchResult {
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

        LocalSearchResult {
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
            println!("\x1b[1;36m[LocalSearchZ3] Starting optimization on {} vertices\x1b[0m", data.name);
        }

        for i in 0..n {
            let result = self.local_search(&data.points, &dist_matrix);
            total_distance += result.distance;
            total_steps += result.n_steps;

            if result.distance < best_distance {
                best_distance = result.distance;
                best_solution = result.solution;
            }

            if print && (i % 10 == 0 || i == n - 1) {
                let progress = ((i + 1) as f64 / n as f64 * 100.0) as u32;
                let bar_width = 30;
                let filled = (progress as usize * bar_width / 100).min(bar_width);
                let bar = format!("[{}{}]", "=".repeat(filled), " ".repeat(bar_width - filled));
                print!("\r\x1b[1;36m[Z3]\x1b[0m {} {}/{} ({}%) | Best: {}", bar, i + 1, n, progress, best_distance);
                std::io::Write::flush(&mut std::io::stdout()).ok();
            }
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