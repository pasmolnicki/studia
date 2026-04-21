use crate::tsp::{self, VecPoints};
use rand;
use serde::{Serialize};
use std::fs::File;
use std::io::Write;
use std::path::{PathBuf};
use std::{path};

#[derive(Debug, Serialize)]
pub struct TspAlgorithmResult {
    pub name: String,
    pub mean_distance: i32,
    pub mean_n_steps: i32,
    pub best_solution: tsp::VecPoints,
}

pub struct LocalSearchResult {
    distance: i32,
    n_steps: i32,
    solution: tsp::VecPoints,
}

trait NamedObject {
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

fn gen_permuntations(data: &tsp::Data, n: usize, rng: &mut dyn rand::Rng) -> Vec<tsp::VecPoints> {
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

/*
Zadanie 1. Wykonaj algorytm Local Search dla n losowych permutacji (n to liczba wierzchołków).
Dla każdych danych podaj średnią wartość uzyskanego rozwiązania, 
średnią liczbę kroków poprawy oraz najlepsze uzyskane rozwiązanie.
*/

pub trait TspProcedure {
    fn local_search(&self, points: &tsp::VecPoints) -> LocalSearchResult;

    fn run(&self, data: &tsp::Data) -> TspAlgorithmResult {
        let mut result = TspAlgorithmResult {
            name: format!("LocalSearchZ1_{}", data.name),
            mean_distance: 0,
            mean_n_steps: 0,
            best_solution: data.points.clone(),
        };

        let mut min_distance = i32::MAX;
        let mut rng = rand::rng();
        let permutations = gen_permuntations(data, data.points.points.len(), &mut rng);

        for (i, perm) in permutations.iter().enumerate() {
            let solution = self.local_search(perm);

            if solution.distance < min_distance {
                min_distance = solution.distance;
                result.best_solution = solution.solution;
            }

            result.mean_distance += (solution.distance - result.mean_distance) / (i as i32 + 1);
            result.mean_n_steps += (solution.n_steps - result.mean_n_steps) / (i as i32 + 1);
        }

        result
    }
}

pub struct LocalSearchZ1;

// The idea behind this struct is to store
// (i, j) pair of indices that we want to invert in the current solution
// and also store the original distance of the solution before applying the inversion
// so that we can calculate the new distance after inversion in constant time
// by calculating the difference in distance caused by the inversion of (i, j) and adding it to the original distance
#[derive(Clone, Debug)]
struct InversionIter {
    i: usize,
    j: usize,
    orig_distance: i32,
}

impl InversionIter {
    pub fn new(points: &VecPoints) -> Self {
        Self { i: 0, j: 1, orig_distance: points.calc_distance() }
    }

    pub fn next(&mut self, points: &VecPoints) -> Option<i32> {
        if self.i >= points.points.len() - 1 {
            return None;
        }

        let distance = self.calculate_inversion_new_distance(points);

        self.j += 1;
        if self.j >= points.points.len() {
            self.i += 1;
            self.j = if self.i >= points.points.len() - 1 { 0 } else { self.i + 1 };
        }

        Some(distance)
    }

    pub fn invert(&mut self, points: &mut VecPoints) {
        let new_distance = self.calculate_inversion_new_distance(points);
        points.points.swap(self.i, self.j);
        self.orig_distance = new_distance;
    }

    pub fn reset(&mut self) {
        self.i = 0;
        self.j = 1;
    }

    fn calculate_inversion_new_distance(&self, points: &VecPoints) -> i32 {
        // Now calculate partial update for distance after inversion of (i, j)
        // Consider the following case:
        // M = (1, 5, 2, 3, 4) - original order
        // (i, j) = (1, 2) - we want to invert index 1 and 2
        // M` = (1, 2, 5, 3, 4) - new order
        // What changed in distance? (3-4) stayed the same, but (1-5) and (5-2) changed to (1-2) and (2-5)
        // So we need to calculate:
        // new_distance = old_distance - dist(1, 5) - dist(5, 2) + dist(1, 2) + dist(2, 5)
        let prev_dist_to_i = if self.i > 0 {
            tsp::point_distance(points.points[self.i - 1], points.points[self.i])
        } else { 0 };
        let prev_dist_after_j = if self.j < points.points.len() - 1 {
            tsp::point_distance(points.points[self.j + 1], points.points[self.j])
        } else { 0 };
        let new_dist_to_i = if self.i > 0 {
            tsp::point_distance(points.points[self.i - 1], points.points[self.j])
        } else { 0 };
        let new_dist_after_j = if self.j < points.points.len() - 1 {
            tsp::point_distance(points.points[self.j + 1], points.points[self.i])
        } else { 0 };

        self.orig_distance - prev_dist_to_i - prev_dist_after_j + new_dist_to_i + new_dist_after_j
    }
}

impl TspProcedure for LocalSearchZ1 {
    fn local_search(&self, points: &VecPoints) -> LocalSearchResult {
        // Simply do the following until we can't improve the solution anymore:
        // 1. For each pair of indices (i, j) calculate the new distance
        // 2. Choose the best pair (i, j) that gives the best improvement and apply the inversion
        // 3. Repeat until no improvement is possible
        let mut iter = InversionIter::new(points);
        let mut current_solution = points.clone();
        let mut n_steps = 0;
        let mut best_distance = iter.orig_distance;

        loop {
            let mut best_iter = None;
            while let Some(new_distance) = iter.next(&current_solution) {
                if new_distance < best_distance {
                    best_distance = new_distance;
                    best_iter = Some(iter.clone());
                }
            }

            if let Some(mut best_iter) = best_iter {
                best_iter.invert(&mut current_solution);
                iter = best_iter;
                iter.reset();
                n_steps += 1;
            } else {
                break;
            }
        }

        LocalSearchResult { distance: iter.orig_distance, n_steps, solution: current_solution }
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_inversion_iter() {
        let mut points = tsp::load_data(&["qa194.tsp"])[0].points.clone();
        let mut iter = InversionIter::new(&points);

        iter.invert(&mut points);
        assert_eq!(iter.orig_distance, points.calc_distance());
    }
}