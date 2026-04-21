use l1::{algo, tsp::{self, Data}};
use algo::{TspProcedure};
use std::error::Error;

fn main() -> Result<(), Box<dyn Error>> {
    // println!("{:?}", tsp::load_data(&file_names));
    // let data = tsp::load_data(&file_names);
    let algo: algo::LocalSearchZ1 = algo::LocalSearchZ1;

    // let res = algo.run(&data[2]);
    // println!("{:?}", res);

    let mut data = tsp::VecPoints::new();
    data.points = vec![(0.0, 0.0), (2.0, 1.0), (4.0, 2.0), (1.0, 3.0), (2.0, 4.0)];
    data.visualize(Some("initial"))?;

    let res = algo.run(&Data { name: String::new(), tsp_type: String::new(), dimension: 0, edge_weight_type: tsp::EdgeWeightType::Eucl2d, points: data });
    res.best_solution.visualize(Some("solved"))?;

    Ok(())
}
