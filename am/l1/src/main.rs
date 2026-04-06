use l1::{tsp, algo};
use algo::{TspProcedure};

fn main() {
    let file_names = vec![
        "qa194.tsp", // Quatar
        "dj38.tsp", // Djibouti
        "uy734.tsp", // Uruguay
        "wi29.tsp", // Western Sahara
        "zi929.tsp", // Zimbabwe
    ];

    // println!("{:?}", tsp::load_data(&file_names));
    let data = tsp::load_data(&file_names);
    let algo = algo::LocalSearchZ1;

    let res = algo.run(&data[2]);
    println!("{:?}", res);
}
