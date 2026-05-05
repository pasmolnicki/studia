use l1::{algo, tsp::{self, Data}};
use algo::{TspProcedure};
use std::error::Error;


#[allow(dead_code)]
fn dummy_test() {
    let algo: algo::LocalSearchZ1 = algo::LocalSearchZ1;
    let mut data = tsp::VecPoints::new();
    data.points = vec![(0.0, 0.0), (2.0, 1.0), (4.0, 2.0), (1.0, 3.0), (2.0, 4.0)];
    data.visualize(Some("initial")).unwrap();

    let res = algo.run(
        &Data { 
            name: String::new(), tsp_type: String::new(), dimension: 0, 
            edge_weight_type: tsp::EdgeWeightType::Eucl2d, points: data 
        }, true
    );
    res.best_solution.visualize(Some("solved")).unwrap();
}

fn main() -> Result<(), Box<dyn Error>> {

    dummy_test();

    // let file_names = ["dj38.tsp", "qa194.tsp"];

    // println!("{:?}", tsp::load_data(&file_names));
    // let data = tsp::load_data(&file_names);
    // let algo: algo::LocalSearchZ1 = algo::LocalSearchZ1;
    // data[0].points.visualize(Some("inital-dj38"))?;
    // let result = algo.run(&data[0], true);
    // result.best_solution.visualize(Some("solved-dj38"))?;
    Ok(())
}
