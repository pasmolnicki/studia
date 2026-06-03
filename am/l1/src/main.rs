#[allow(unused_imports)]
use l1::{algo::{self, LocalSearchZ1, LocalSearchZ2, LocalSearchZ3, SimulatedAnnealingBase, TabuSearchBase, TspAlgorithmResult}, tsp::{self, Data}};
use algo::{TspProcedure};
use std::{error::Error, fs::File, io::Write, path::{self, PathBuf}};


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

    result.best_solution.visualize(
        Some(&format!("results/plots/{}_{}", data.name, procedure_name)))
}

#[allow(dead_code)]
fn save_to_csv(data: &Data, procedure_name: &str, result: &algo::TspAlgorithmResult) -> Result<(), Box<dyn Error + 'static>> {
    let csv_path = format!("results/{}/{}_{}.csv", procedure_name, data.name, procedure_name);
    let mut wtr = csv::Writer::from_path(&csv_path)?;
    wtr.write_record(&["mean_distance", "mean_n_steps", "best_distance"])?;
    wtr.write_record(&[result.mean_distance.to_string(), result.mean_n_steps.to_string(), result.best_solution.calc_distance().to_string()])?;
    wtr.flush()?;
    Ok(())
}

#[allow(dead_code)]
fn load_small_data_sets() -> Vec<Data> {
    tsp::load_data(&["wi29.tsp", "dj38.tsp", "qa194.tsp"])
}

#[allow(dead_code)]
fn load_below_1k_data_sets() -> Vec<Data> {
    tsp::load_data(&["wi29.tsp", "dj38.tsp", "qa194.tsp", "uy734.tsp", "zi929.tsp"])
}

#[allow(dead_code)]
fn load_lagrge_data_sets() -> Vec<Data> {
    tsp::load_data(&["mu1979.tsp", "ca4663.tsp", "tz6117.tsp", "eg7146.tsp", "ei8246.tsp"])
}


#[allow(dead_code)]
fn load_all_data() -> Vec<Data> {
    tsp::load_data(&[
        "wi29.tsp", "dj38.tsp", "qa194.tsp", "uy734.tsp", "zi929.tsp",
        "mu1979.tsp", "ca4663.tsp", "tz6117.tsp", "eg7146.tsp", "ei8246.tsp"])
}

#[allow(dead_code)]
fn optimize_simulated_annealing_base() -> Result<(), Box<dyn Error>> {
    let csv_path = format!("results/sa_parameter_tuning.csv");
    let mut wtr = csv::Writer::from_path(&csv_path)?;
    wtr.write_record(&["initial_temperature", "cooling_factor", "epoch_length", "no_improve_limit", "avg_distance"])?;

    let data = load_below_1k_data_sets();
    let temperatures = [1.0, 5.0, 10.0, 20.0, 50.0, 75.0, 100.0];
    let cooling_factors = [0.8, 0.85, 0.90, 0.95, 0.99];
    let epoch_lengths = [10, 20, 35, 50, 100];
    let no_improve_limits = [5, 10, 15, 20, 30];

    let mut best_params = algo::SimulatedAnnealingParams {
        initial_temperature: 0.0,
        cooling_factor: 0.0,
        epoch_length: 0,
        no_improve_limit: 0,
    };

    let mut best_mean_distance = f64::INFINITY;
    
    for &temp in &temperatures {
        for &cool in &cooling_factors {
            for &epoch in &epoch_lengths {
                for &no_improve in &no_improve_limits {
                    let sa = SimulatedAnnealingBase::new(temp, cool, epoch, no_improve);
                    let mut total_distance = 0.0;
                    for d in data.iter() {
                        let res = sa.run(d, false);
                        total_distance += res.mean_distance as f64;
                    }
                    let avg_distance = total_distance / (data.len() as f64);

                    wtr.write_record(&[temp.to_string(), cool.to_string(), epoch.to_string(), no_improve.to_string(), avg_distance.to_string()])?;
                    println!("Temp: {}, Cool: {}, Epoch: {}, NoImprove: {} => AvgDist: {}", temp, cool, epoch, no_improve, avg_distance);
                    if avg_distance < best_mean_distance {
                        best_mean_distance = avg_distance;
                        best_params = algo::SimulatedAnnealingParams {
                            initial_temperature: temp,
                            cooling_factor: cool,
                            epoch_length: epoch,
                            no_improve_limit: no_improve,
                        };
                    }
                }
            }
        }
    }

    wtr.flush()?;
    println!("\nBest Parameters: Temp: {}, Cool: {}, Epoch: {}, NoImprove: {} => AvgDist: {}", 
        best_params.initial_temperature, best_params.cooling_factor, best_params.epoch_length, best_params.no_improve_limit, best_mean_distance);

    Ok(())
}


#[allow(dead_code)]
fn optimize_tabu_search() -> Result<(), Box<dyn Error>> {
    let data = load_below_1k_data_sets();
    let tabu_tenures = [5, 15, 30, 0];
    let max_iterations = [5, 20, 50, 70];
    let no_improve_limits = [5, 15, 30];
    let csv_path = format!("results/ts_parameter_tuning.csv");
    let mut wtr = csv::Writer::from_path(&csv_path)?;
    wtr.write_record(&["tabu_tenure", "max_iterations", "no_improve_limit", "avg_distance"])?;

    let mut best_params = algo::TabuSearchParams {
        tabu_tenure: 0,
        max_iterations: 0,
        no_improve_limit: 0,
    };

    let mut best_mean_distance = f64::INFINITY;

    for &tenure in &tabu_tenures {
        for &max_iter in &max_iterations {
            for &no_improve in &no_improve_limits {
                let ts = TabuSearchBase::new(tenure, max_iter, no_improve);
                let mut total_distance = 0.0;
                for d in data.iter() {
                    let res = ts.run(d, false);
                    total_distance += res.mean_distance as f64;
                }
                let avg_distance = total_distance / (data.len() as f64);
                println!("Tenure: {}, MaxIter: {}, NoImprove: {} => AvgDist: {}", tenure, max_iter, no_improve, avg_distance);
                if avg_distance < best_mean_distance {
                    best_mean_distance = avg_distance;
                    best_params = algo::TabuSearchParams {
                        tabu_tenure: tenure,
                        max_iterations: max_iter,
                        no_improve_limit: no_improve,
                    };
                }
                wtr.write_record(&[tenure.to_string(), max_iter.to_string(), no_improve.to_string(), avg_distance.to_string()])?;
            }
        }
    }

    wtr.flush()?;
    println!("\nBest Parameters: Tenure: {}, MaxIter: {}, NoImprove: {} => AvgDist: {}", 
        best_params.tabu_tenure, best_params.max_iterations, best_params.no_improve_limit, best_mean_distance);

    Ok(())
}

#[allow(dead_code)]
fn run_reapted_experiment(algo: &dyn TspProcedure, n: i32, algo_name: &str) -> Result<(), Box<dyn Error>> {
    let data_set = load_lagrge_data_sets();

    for data in data_set.iter() {
        let mut best_solution = TspAlgorithmResult::default();
        for i in 0..n {
            let result = algo.run(data, true);

            if result.mean_distance < best_solution.mean_distance {
                best_solution.best_solution = result.best_solution;
            }

            println!(" {} {}/{} mean_distance={} mean_n_steps={}", 
                data.name, i, n, best_solution.mean_distance, best_solution.mean_n_steps);
        }
        best_solution.best_solution.visualize(Some(&format!("results/{}-{}.png", algo_name, n)))?;
    }
    

    Ok(())
}

const SIMULATED_ANNEALING: &str = "simulated_ann";
const TABU_SEARCH: &str = "tabu_search";
const LOCAL_SEARCH_Z1: &str = "Z1";
const LOCAL_SEARCH_Z2: &str = "Z2";
const LOCAL_SEARCH_Z3: &str = "Z3";

/// Example of running all three algorithms on TSP datasets
#[allow(dead_code)]
fn run_full_experiment() -> Result<(), Box<dyn Error>> {
    // These are the TSP problem instances to solve
    let data_list = 
        load_small_data_sets();
        // load_lagrge_data_sets();
        // tsp::load_data(&["ca4663.tsp", "tz6117.tsp", "eg7146.tsp", "ei8246.tsp"]);

    for data in data_list.iter() {
        println!("Processing: {}", data.name);
        
        run_experiment(&LocalSearchZ1, data, LOCAL_SEARCH_Z1)?;
        run_experiment(&LocalSearchZ2, data, LOCAL_SEARCH_Z2)?;
        run_experiment(&LocalSearchZ3, data, LOCAL_SEARCH_Z3)?;
        run_experiment(&SimulatedAnnealingBase::default(), data, SIMULATED_ANNEALING)?;
        run_experiment(&TabuSearchBase::default(), data, TABU_SEARCH)?;
        
        println!();
    }
    
    Ok(())
}

fn generate_md_tables() {
    let optimal_tours = [
        ("wi29.tsp", 27603, "Western Sahara"),
        ("dj38.tsp", 6656, "Djibouti"),
        ("qa194.tsp", 9352, "Qatar"),
        ("uy734.tsp", 79114, "Uruguay"),
        ("zi929.tsp", 95_345, "Zimbabwe"),
        ("mu1979.tsp", 86_891, "Mauritania"),
        ("ca4663.tsp", 1_290_319, "Canada"),
        ("tz6117.tsp", 95_345, "Tanzania"),
        ("eg7146.tsp", 172_387, "Egypt"),
        ("ei8246.tsp", 206_171, "Ireland"),
    ];

    let path = path::absolute(PathBuf::from("./results/")).unwrap();
    let output_path = path.join("summary_tables.md");
    let mut md_file = File::create(&output_path).expect("Could not create summary_tables.md");
    let header = "| File | Algorithm | Mean Distance | Mean Steps | Best Distance | Optimum Ratio |\n|---|---|---|---|---|---|\n";

    for (file, optimum, name) in optimal_tours {
        md_file.write_all(format!("## {name} ({file})\n\n").as_bytes()).expect("Could not write to summary_tables.md");
        md_file.write_all(header.as_bytes()).expect("Could not write to summary_tables.md");

        print!("## {name} ({file})\n");
        print!("{}", header);

        for algo_name in [SIMULATED_ANNEALING, TABU_SEARCH, LOCAL_SEARCH_Z1, LOCAL_SEARCH_Z2, LOCAL_SEARCH_Z3].iter() {
            let csv_path = path.join(algo_name).join(format!("{}_{}.csv", file.split('.').next().unwrap(), algo_name));
            if let Ok(mut rdr) = csv::Reader::from_path(&csv_path) {
                if let Some(Ok(record)) = rdr.records().next() {
                    let mean_distance: i32 = record.get(0).unwrap_or("0").parse().unwrap_or(0);
                    let mean_n_steps: i32 = record.get(1).unwrap_or("0").parse().unwrap_or(0);
                    let best_distance: i32 = record.get(2).unwrap_or("0").parse().unwrap_or(0);

                    md_file.write_all(format!("| {} | {} | {} | {} | {} | {:.3} |\n", 
                        file, algo_name, mean_distance, mean_n_steps, best_distance, 
                        optimum as f64 / best_distance as f64).as_bytes()).expect("Could not write to summary_tables.md");
                    print!("| {} | {} | {} | {} | {} | {:.3} |\n", 
                        file, algo_name, mean_distance, mean_n_steps, best_distance, optimum as f64 / best_distance as f64);
                }
            } 
            // else {
                // println!("Could not read results for {} with {}. Expected at: {}", file, algo_name, csv_path.display());
            // }
        }

        md_file.write_all(b"\n").expect("Could not write to summary_tables.md");
        println!();
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    run_full_experiment()?;
    // optimize_simulated_annealing_base()?;
    // optimize_tabu_search()?;
    // run_reapted_experiment(&SimulatedAnnealingBase::default(), 100, "simulated_ann")?;

    // let data = &tsp::load_data(&["wi29.tsp"])[0];
    // run_experiment(&TabuSearchBase::default(), data, "test_sim_ann")?;

    generate_md_tables();

    Ok(())
}
