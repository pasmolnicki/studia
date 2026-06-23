use crate::{
    algo::{AlgoSearchResult, TspProcedure},
    ga::CrossoverType::{OX, PMX},
    tsp::{DistanceMatrix, VecPoints, rand_tours},
    utils::print_progress,
};
use rand::{Rng, RngExt};

fn ox_offspring(
    dm: &DistanceMatrix,
    parent1: &GaIndividual,
    parent2: &GaIndividual,
    cut: usize,
) -> GaIndividual {
    let num_cities = parent1.0.len();
    let mut offspring = Vec::with_capacity(num_cities);

    // Use a boolean vector for O(1) membership checks.
    // This is incredibly faster than a BTreeSet.
    let mut present_cities = vec![false; num_cities];

    // Inherit from parent 1
    for &city in &parent1.0[0..cut] {
        offspring.push(city);
        present_cities[city] = true;
    }

    // Fill remaining from parent 2
    for &city in parent2.0.iter() {
        if !present_cities[city] {
            offspring.push(city);
            // present_cities[city] = true; // Optional since we never check it again
        }
    }

    let len = dm.calculate_tour_length(&offspring);
    GaIndividual(offspring, len)
}

fn ox_crossover(
    rng: &mut dyn Rng,
    dm: &DistanceMatrix,
    parent1: &GaIndividual,
    parent2: &GaIndividual,
) -> (GaIndividual, GaIndividual) {
    (
        ox_offspring(dm, parent1, parent2, rng.random_range(0..parent1.0.len())),
        ox_offspring(dm, parent2, parent1, rng.random_range(0..parent1.0.len())),
    )
}

fn pmx_crossover(
    rng: &mut dyn Rng,
    dm: &DistanceMatrix,
    parent1: &GaIndividual,
    parent2: &GaIndividual,
) -> (GaIndividual, GaIndividual) {
    (
        ox_offspring(dm, parent1, parent2, rng.random_range(0..parent1.0.len())),
        ox_offspring(dm, parent2, parent1, rng.random_range(0..parent1.0.len())),
    )
}

#[derive(Clone, Copy, Debug)]
#[repr(i32)]
pub enum CrossoverType {
    OX(),
    PMX(),
}

impl Into<String> for CrossoverType {
    fn into(self) -> String {
        match self {
            OX() => "OX".to_string(),
            PMX() => "PMX".to_string(),
        }
    }
}

#[derive(Clone)]
pub struct GaIndividual(Vec<usize>, i64);
type GaPopulation = Vec<GaIndividual>;

#[derive(Clone, Copy)]
pub struct GAParams {
    n_population: fn(usize) -> usize,
    crossover_rate: f64,
    crossover_type: CrossoverType,
    mutation_rate: f64,
    n_generations: fn(usize) -> usize,
}

impl Default for GAParams {
    fn default() -> Self {
        Self {
            n_population: |n| n.isqrt(),
            crossover_rate: 0.6,
            crossover_type: OX(),
            mutation_rate: 0.2,
            n_generations: |n| (1.2 * n.isqrt() as f64) as usize,
        }
    }
}

impl GAParams {
    pub fn new(
        n_population: fn(usize) -> usize,
        n_generations: fn(usize) -> usize,
        crossover_rate: f64,
        crossover_type: CrossoverType,
        mutation_rate: f64,
    ) -> Self {
        Self {
            n_population,
            crossover_type,
            crossover_rate,
            mutation_rate,
            n_generations,
        }
    }
}

#[derive(Clone, Copy, Default)]
pub struct GeneticAlgorithm {
    params: GAParams,
}

impl TspProcedure for GeneticAlgorithm {
    fn run(&self, data: &crate::tsp::Data, print: bool) -> crate::algo::TspAlgorithmResult {
        self.run_multithreaded(data, print, 100)
    }

    fn algo(
        &self,
        points: &crate::tsp::VecPoints,
        dm: &DistanceMatrix,
    ) -> crate::algo::AlgoSearchResult {
        // First step: generate random population
        let n = points.points.len();
        let tours = rand_tours(&points, (self.params.n_population)(n) as i32);
        let mut population: GaPopulation = tours
            .into_iter()
            .map(|tour| {
                let len = dm.calculate_tour_length(&tour);
                GaIndividual(tour, len)
            })
            .collect();
        let n_genrations = (self.params.n_generations)(n);

        for i in 0..n_genrations {
            population = self.step(&population, dm);
            // print_progress("GA", true, n_genrations, population[0].1, i);
        }

        let best_ind = &population[0];
        let solution = VecPoints {
            points: best_ind.0.iter().map(|&idx| points.points[idx]).collect(),
            name: points.name.clone(),
        };
        AlgoSearchResult {
            distance: best_ind.1,
            n_steps: n_genrations as u64,
            solution,
        }
    }

    fn name(&self) -> &str {
        "GeneticAlgorithm"
    }
}

impl GeneticAlgorithm {
    pub fn new(params: GAParams) -> Self {
        Self { params }
    }

    fn crossover(
        &self,
        rng: &mut dyn Rng,
        dm: &DistanceMatrix,
        parent1: &GaIndividual,
        parent2: &GaIndividual,
    ) -> (GaIndividual, GaIndividual) {
        match self.params.crossover_type {
            OX() => ox_crossover(rng, dm, parent1, parent2),
            PMX() => pmx_crossover(rng, dm, parent1, parent2),
        }
    }
    /// Run one full pass of 2-opt improvement. Returns true if any improvement was made.
    fn two_opt_pass(&self, individual: &mut GaIndividual, dm: &DistanceMatrix) -> bool {
        let n = individual.0.len();
        let mut improved = false;
        let mut i = 0;

        while i < n - 1 {
            let mut j = i + 2;
            while j < n {
                // For i=0, edge (n-1, 0) is the wrap-around; handle separately
                let a = individual.0[i];
                let b = individual.0[i + 1];
                let c = individual.0[j];
                let d = individual.0[(j + 1) % n];

                let old_cost = dm.get(a, b) + dm.get(c, d);
                let new_cost = dm.get(a, c) + dm.get(b, d);

                if new_cost < old_cost {
                    individual.0[i + 1..=j].reverse();
                    individual.1 += new_cost - old_cost;
                    improved = true;
                    j = i + 2; // Restart inner loop after structural change
                } else {
                    j += 1;
                }
            }
            i += 1;
        }

        improved
    }

    fn mutate(&self, rng: &mut dyn Rng, offspring: &mut GaIndividual, dm: &DistanceMatrix) {
        let len = offspring.0.len();
        // self.params.mutation_rate now means "probability per city"
        let n_mutations = (len as f64 * self.params.mutation_rate).round() as usize;

        for _ in 0..n_mutations.max(1) {
            let mut i = rng.random_range(0..len);
            let mut j = rng.random_range(0..len);
            if i > j {
                std::mem::swap(&mut i, &mut j);
            }

            // Capture the four city indices before reversal
            let a = offspring.0[(i + len - 1) % len];
            let b = offspring.0[i];
            let c = offspring.0[j];
            let d = offspring.0[(j + 1) % len];

            let old_cost = dm.get(a, b) + dm.get(c, d);

            // After reversal: a is now adjacent to c, b is now adjacent to d
            offspring.0[i..=j].reverse();

            let new_cost = dm.get(a, c) + dm.get(b, d);
            offspring.1 += new_cost - old_cost;
        }
    }

    fn step(&self, population: &GaPopulation, dm: &DistanceMatrix) -> GaPopulation {
        let mut rng = rand::rng();

        let parents = self.selection(&mut rng, population);

        // Pre-allocate the exact needed capacity to avoid intermediate reallocations
        let mut new_population = Vec::with_capacity(parents.len() + population.len());

        for pair in parents.chunks_exact(2) {
            let (mut off1, mut off2) =
                self.crossover(&mut rng, dm, &population[pair[0]], &population[pair[1]]);

            if rng.random::<f64>() < self.params.mutation_rate {
                self.mutate(&mut rng, &mut off1, dm);
            }

            if rng.random::<f64>() < self.params.mutation_rate {
                self.mutate(&mut rng, &mut off2, dm);
            }

            new_population.push(off1);
            new_population.push(off2);
        }

        new_population.extend_from_slice(population);
        new_population.sort_unstable_by_key(|ind| ind.1);
        new_population.truncate(population.len());
        self.two_opt_pass(&mut new_population[0], dm);
        new_population
    }

    fn selection(&self, rng: &mut dyn Rng, population: &GaPopulation) -> Vec<usize> {
        // let fitness = self.fitness(population);
        // (0..(self.params.crossover_rate * population.len() as f64) as usize)
        //     .map(|_| self.roulette_wheel(rng, &fitness, population))
        //    .collect()
        let num_parents = (self.params.crossover_rate * population.len() as f64) as usize;
        let mut parents = Vec::with_capacity(num_parents);

        // Tournament size of 3 is a good default for balanced selection pressure.
        // You can increase this to 5 if you want faster convergence.
        let tournament_size = 3;

        for _ in 0..num_parents {
            let mut best_idx = rng.random_range(0..population.len());
            let mut best_dist = population[best_idx].1;

            for _ in 1..tournament_size {
                let idx = rng.random_range(0..population.len());
                let dist = population[idx].1;
                if dist < best_dist {
                    best_dist = dist;
                    best_idx = idx;
                }
            }
            parents.push(best_idx);
        }
        parents
    }
}
