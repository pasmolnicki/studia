use crate::{
    tsp::{Data, DistanceMatrix, VecPoints, rand_tours},
    utils::random_inversion,
};
use rand::{Rng, RngExt};
use std::collections::BTreeSet;

fn random_offspring(parent1: &Vec<usize>, parent2: &Vec<usize>, cut: usize) -> Vec<usize> {
    let mut offspring = Vec::with_capacity(parent1.len());
    offspring.extend_from_slice(&(&parent1)[0..cut]);
    let mut present_cities: BTreeSet<usize> = offspring.iter().copied().collect();
    for &city in parent2 {
        if present_cities.insert(city) {
            offspring.push(city);
        }
    }
    offspring
}

#[derive(Clone, Copy)]
struct GAParams {
    n_population: fn(usize) -> usize,
    crossover_rate: f64,
    mutation_rate: f64,
    n_generations: fn(usize) -> usize,
}

impl Default for GAParams {
    fn default() -> Self {
        Self {
            n_population: |_| 100,
            crossover_rate: 0.8,
            mutation_rate: 0.05,
            n_generations: |_| 50,
        }
    }
}

#[derive(Clone, Copy, Default)]
struct GeneticAlgorithm {
    params: GAParams,
}

impl GeneticAlgorithm {
    fn new(params: GAParams) -> Self {
        Self { params }
    }

    fn fitness(&self, eval: &Vec<i64>) -> Vec<f64> {
        let max = eval.iter().max().unwrap();
        let sum = eval.iter().map(|v| *max - *v).sum::<i64>();
        eval.iter()
            .map(|len| ((max - *len) as f64) / (sum as f64))
            .collect()
    }

    fn crossover(
        &self,
        rng: &mut dyn Rng,
        parent1: &Vec<usize>,
        parent2: &Vec<usize>,
    ) -> (Vec<usize>, Vec<usize>) {
        let cut = rng.random_range(0..parent1.len());
        (
            random_offspring(parent1, parent2, cut),
            random_offspring(parent2, parent1, cut),
        )
    }

    fn mutate(&self, rng: &mut dyn Rng, offspring: &mut Vec<usize>) {
        let (i, j) = random_inversion(rng, offspring.len());
        offspring.swap(i, j);
    }

    fn roulette_wheel(
        &self,
        rng: &mut dyn rand::Rng,
        fitness: &Vec<f64>,
        population: &Vec<Vec<usize>>,
    ) -> usize {
        // Now calculate cumulative sum
        let cumsum = fitness
            .iter()
            .scan(0f64, |acc, &x| {
                *acc += x;
                Some(*acc)
            })
            .enumerate();

        let rand_value = rng.random::<f64>();
        for (i, v) in cumsum {
            if v > rand_value {
                return i;
            }
        }

        population.len() - 1
    }

    fn step(&self, population: &Vec<Vec<usize>>, dm: &DistanceMatrix) -> Vec<Vec<usize>> {
        let mut rng = rand::rng();
        let eval = population
            .iter()
            .map(|route| dm.calculate_tour_length(route))
            .collect::<Vec<i64>>();

        // Get the selected indices for parents
        let parents = self.selection(&mut rng, population, &eval);
        let mut offspring = Vec::with_capacity(parents.len());
        for pair in parents.chunks_exact(2) {
            let (mut off1, mut off2) =
                self.crossover(&mut rng, &population[pair[0]], &population[pair[1]]);

            if rng.random::<f64>() < self.params.mutation_rate {
                self.mutate(&mut rng, &mut off1);
            }

            if rng.random::<f64>() < self.params.mutation_rate {
                self.mutate(&mut rng, &mut off2);
            }

            offspring.push(off1);
            offspring.push(off2);
        }

        let acutal_parents: Vec<Vec<usize>> =
            (0..parents.len()).map(|i| population[i].clone()).collect();
        offspring.extend_from_slice(&acutal_parents);
        let mut scored: Vec<(Vec<usize>, i64)> = offspring
            .into_iter()
            .map(|route| (route, dm.calculate_tour_length(&route)))
            .collect();

        scored.sort_by_key(|(_, eval)| *eval);
    }

    fn selection(
        &self,
        rng: &mut dyn Rng,
        population: &Vec<Vec<usize>>,
        eval: &Vec<i64>,
    ) -> Vec<usize> {
        let fitness = self.fitness(eval);
        (0..(self.params.crossover_rate * population.len() as f64) as usize)
            .map(|_| self.roulette_wheel(rng, &fitness, population))
            .collect()
    }

    #[allow(dead_code)]
    pub fn run(&self, data: &VecPoints, dm: &DistanceMatrix) {
        // First step: generate random population
        let mut population = rand_tours(&data, 30);
    }
}
