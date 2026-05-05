use l1::{algo::{self, LocalSearchZ1, LocalSearchZ2, LocalSearchZ3, SimulatedAnnealingBase, TabuSearchBase}, tsp::{self, Data}};
use algo::{TspProcedure};
use std::{error::Error, path::PathBuf};


#[allow(dead_code)]
fn dummy_test() {
    let algorithm = algo::LocalSearchZ1;
    let mut data = tsp::VecPoints::new();
    // Larger dataset to show progress indicators
    data.points = vec![
        (2.0, 1.0), (0.0, 0.0), (4.0, 2.0), (1.0, 3.0), (2.0, 4.0),
        (5.0, 1.0), (3.0, 5.0), (6.0, 3.0), (1.0, 6.0), (7.0, 2.0),
        (4.0, 6.0), (8.0, 1.0), (2.0, 7.0), (6.0, 6.0), (3.0, 2.0),
        (5.0, 7.0), (1.0, 4.0), (7.0, 4.0), (4.0, 0.0), (8.0, 5.0),
    ];
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

#[allow(dead_code)]
fn run_experiment(procedure: &dyn TspProcedure, data: &Data, procedure_name: &str) -> Result<PathBuf, Box<dyn Error>>  {
    // Task 1: Full Local Search with Invert moves
    println!("  Running LocalSearch{procedure_name} (full invert)...");
    let result = procedure.run(data, true);
    println!("  {procedure_name} - Mean: {}, Steps: {}, Best: {}", 
        result.mean_distance, result.mean_n_steps, 
        result.best_solution.calc_distance() as i64);

    save_to_csv(data, procedure_name, &result)?;

    result.best_solution.visualize(Some(&format!("results/{}_{}", data.name, procedure_name)))
}

#[allow(dead_code)]
fn save_to_csv(data: &Data, procedure_name: &str, result: &algo::TspAlgorithmResult) -> Result<(), Box<dyn Error + 'static>> {
    let csv_path = format!("results/{}_{}.csv", data.name, procedure_name);
    let mut wtr = csv::Writer::from_path(&csv_path)?;
    wtr.write_record(&["mean_distance", "mean_n_steps", "best_distance"])?;
    wtr.write_record(&[result.mean_distance.to_string(), result.mean_n_steps.to_string(), result.best_solution.calc_distance().to_string()])?;
    wtr.flush()?;
    Ok(())
}

/// Example of running all three algorithms on TSP datasets
#[allow(dead_code)]
fn run_full_experiment() -> Result<(), Box<dyn Error>> {
    // These are the TSP problem instances to solve
    let file_names = [
        "dj38.tsp", "qa194.tsp", "uy734.tsp", "wi29.tsp", "zi929.tsp",
        "mu1979.tsp", "ca4663.tsp", "tz6117.tsp", "eg7146.tsp", "ei8246.tsp"];
    let data_list = tsp::load_data(&file_names);
    
    for data in data_list.iter() {
        println!("Processing: {}", data.name);
        
        // run_experiment(&LocalSearchZ1, data, "Z1")?;
        // run_experiment(&LocalSearchZ2, data, "Z2")?;
        run_experiment(&LocalSearchZ3, data, "Z3")?;
        println!();
    }
    
    Ok(())
}

fn main() -> Result<(), Box<dyn Error>> {
    run_full_experiment()?;


    // Test on smaller dataset to verify all 5 algorithms work
    // let mut data = tsp::VecPoints::new();
    // data.points = vec![
    //     (2.0, 1.0), (0.0, 0.0), (4.0, 2.0), (1.0, 3.0), (2.0, 4.0),
    //     (5.0, 1.0), (3.0, 5.0), (6.0, 3.0), (1.0, 6.0), (7.0, 2.0),
    //     (4.0, 6.0), (8.0, 1.0), (2.0, 7.0), (6.0, 6.0), (3.0, 2.0),
    //     (5.0, 7.0), (1.0, 4.0), (7.0, 4.0), (4.0, 0.0), (8.0, 5.0),
    // ];
    // data.set_name("test".to_string());
    
    // let test_data = Data {
    //     name: "test".to_string(),
    //     tsp_type: String::new(),
    //     dimension: data.points.len() as i32,
    //     edge_weight_type: tsp::EdgeWeightType::Eucl2d,
    //     points: data,
    // };

    // println!("\n========== TSP Algorithm Comparison ==========\n");

    // // Run all 5 algorithms
    // let z1 = LocalSearchZ1;
    // let z2 = LocalSearchZ2;
    // let z3 = LocalSearchZ3;
    // let sa = SimulatedAnnealingBase::default();
    // let ts = TabuSearchBase::default();

    // println!("\n--- Running LocalSearchZ1 (Parallel Invert) ---");
    // let result_z1 = z1.run(&test_data, true);
    // println!("Mean Distance: {} | Mean Steps: {} | Best: {}\n", 
    //     result_z1.mean_distance, result_z1.mean_n_steps, 
    //     result_z1.best_solution.calc_distance() as i64);

    // println!("\n--- Running LocalSearchZ2 (Random Sampling Invert) ---");
    // let result_z2 = z2.run(&test_data, true);
    // println!("Mean Distance: {} | Mean Steps: {} | Best: {}\n", 
    //     result_z2.mean_distance, result_z2.mean_n_steps, 
    //     result_z2.best_solution.calc_distance() as i64);

    // println!("\n--- Running LocalSearchZ3 (Full Transpose) ---");
    // let result_z3 = z3.run(&test_data, true);
    // println!("Mean Distance: {} | Mean Steps: {} | Best: {}\n", 
    //     result_z3.mean_distance, result_z3.mean_n_steps, 
    //     result_z3.best_solution.calc_distance() as i64);

    // println!("\n--- Running SimulatedAnnealing ---");
    // let result_sa = sa.run(&test_data, true);
    // println!("Mean Distance: {} | Mean Steps: {} | Best: {}\n", 
    //     result_sa.mean_distance, result_sa.mean_n_steps, 
    //     result_sa.best_solution.calc_distance() as i64);

    // println!("\n--- Running TabuSearch ---");
    // let result_ts = ts.run(&test_data, true);
    // println!("Mean Distance: {} | Mean Steps: {} | Best: {}\n", 
    //     result_ts.mean_distance, result_ts.mean_n_steps, 
    //     result_ts.best_solution.calc_distance() as i64);

    // // Summary comparison
    // println!("\n========== SUMMARY ==========");
    // println!("Algorithm        | Mean Dist | Mean Steps | Best");
    // println!("--------------------------------------------------");
    // println!("LocalSearchZ1    | {:9} | {:10} | {}", result_z1.mean_distance, result_z1.mean_n_steps, result_z1.best_solution.calc_distance() as i64);
    // println!("LocalSearchZ2    | {:9} | {:10} | {}", result_z2.mean_distance, result_z2.mean_n_steps, result_z2.best_solution.calc_distance() as i64);
    // println!("LocalSearchZ3    | {:9} | {:10} | {}", result_z3.mean_distance, result_z3.mean_n_steps, result_z3.best_solution.calc_distance() as i64);
    // println!("SimulatedAnneal. | {:9} | {:10} | {}", result_sa.mean_distance, result_sa.mean_n_steps, result_sa.best_solution.calc_distance() as i64);
    // println!("TabuSearch       | {:9} | {:10} | {}", result_ts.mean_distance, result_ts.mean_n_steps, result_ts.best_solution.calc_distance() as i64);
    // println!();

    Ok(())
}
