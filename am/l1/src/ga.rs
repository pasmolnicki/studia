use crate::tsp::{Data, DistanceMatrix, VecPoints, rand_tours};

#[derive(Clone)]
struct GeneticAlgorithm {}

impl GeneticAlgorithm {
    #[allow(dead_code)]
    pub fn run(data: &Data) {
        // First step: generate random population
        let mut population = rand_tours(&data.points, 30);
    }
}
