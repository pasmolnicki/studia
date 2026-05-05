use l1::{algo, tsp::{self, Data}};
use algo::{TspProcedure};
use std::error::Error;


#[allow(dead_code)]
fn dummy_test() {
    let algorithm = algo::LocalSearchZ1;
    let mut data = tsp::VecPoints::new();
    data.points = vec![(2.0, 1.0), (0.0, 0.0), (4.0, 2.0), (1.0, 3.0), (2.0, 4.0)];
    data.set_name("test".to_string());
    data.visualize(Some("initial")).unwrap();

    let res = algorithm.run(
        &Data { 
            name: "test".to_string(), 
            tsp_type: String::new(), 
            dimension: 0, 
            edge_weight_type: tsp::EdgeWeightType::Eucl2d, 
            points: data 
        }, true
    );
    res.best_solution.visualize(Some("solved")).unwrap();
}

/// Example of running all three algorithms on TSP datasets
#[allow(dead_code)]
fn run_full_experiment() -> Result<(), Box<dyn Error>> {
    // These are the TSP problem instances to solve
    let file_names = ["wi29.tsp", "dj38.tsp", "qa194.tsp", "uy734.tsp", "zi929.tsp"];
    
    for file_name in file_names.iter() {
        println!("Processing: {}", file_name);
        let data = tsp::load_data(&[file_name]);
        
        // Task 1: Full Local Search with Invert moves
        println!("  Running LocalSearchZ1 (full invert)...");
        let z1 = algo::LocalSearchZ1;
        let result_z1 = z1.run(&data[0], true);
        println!("  Z1 - Mean: {}, Steps: {}, Best: {}", 
            result_z1.mean_distance, result_z1.mean_n_steps, 
            result_z1.best_solution.calc_distance() as i64);
        
        // Task 2: Random sampling Local Search with Invert moves
        println!("  Running LocalSearchZ2 (random invert)...");
        let z2 = algo::LocalSearchZ2;
        let result_z2 = z2.run(&data[0], true);
        println!("  Z2 - Mean: {}, Steps: {}, Best: {}", 
            result_z2.mean_distance, result_z2.mean_n_steps, 
            result_z2.best_solution.calc_distance() as i64);
        
        // Task 3: Full Local Search with Transpose moves
        println!("  Running LocalSearchZ3 (full transpose)...");
        let z3 = algo::LocalSearchZ3;
        let result_z3 = z3.run(&data[0], true);
        println!("  Z3 - Mean: {}, Steps: {}, Best: {}", 
            result_z3.mean_distance, result_z3.mean_n_steps, 
            result_z3.best_solution.calc_distance() as i64);
        
        println!();
    }
    
    Ok(())
}

fn main() -> Result<(), Box<dyn Error>> {
    // Run the dummy test by default
    dummy_test();

    // Uncomment the line below to run the full experiment on all TSP datasets
    // Note: This will take some time as it runs n iterations for each algorithm on each dataset
    // run_full_experiment()?;

    Ok(())
}
