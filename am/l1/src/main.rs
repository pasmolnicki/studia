use l1::{tsp, algo};

fn main() {
    let file_names = vec![
        "qa194.tsp", // Quatar
        "dj38.tsp", // Djibouti
        "uy734.tsp", // Uruguay
        "wi29.tsp", // Western Sahara
        "zi929.tsp", // Zimbabwe
    ];

    println!("{:?}", tsp::load_data(file_names));
}
