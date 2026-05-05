use l1::{algo::{self, LocalSearchZ1, LocalSearchZ2, LocalSearchZ3}, tsp::{self, Data}};
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
    let file_names = ["mu1979.tsp", "ca4663.tsp", "tz6117.tsp", "eg7146.tsp", "ei8246.tsp"];
    let data_list = tsp::load_data(&file_names);
    
    for data in data_list.iter() {
        println!("Processing: {}", data.name);
        
        // run_experiment(&LocalSearchZ1, data, "Z1")?;
        run_experiment(&LocalSearchZ2, data, "Z2")?;
        // run_experiment(&LocalSearchZ3, data, "Z3")?;
        println!();
    }
    
    Ok(())
}

fn main() -> Result<(), Box<dyn Error>> {
    run_full_experiment()?;
    Ok(())
}
