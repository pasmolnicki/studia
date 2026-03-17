/*
Dla zbiorów Western Sahara, Djibouti, Quatar, Uruguay i Zimbabwe wykonaj
następujące zadania:
1. Wylosuj 1000 permutacji wierzchołków i policz
    (a) średnią z minimum dla każdych 10 kolejnych losowań (100 grup po 10 losowań),
    (b) średnią z minimum dla każdych 50 kolejnych losowań (20 grup po 50 losowań),
    (c) i minimalną wartość dla tych 1000 losowań
*/

use std::{fs, path};
use std::path::{PathBuf};
use rand::Rng;
use rand::seq::SliceRandom;

#[derive(Debug)]
pub enum EdgeWeightType {
    Eucl2d
}

impl From<String> for EdgeWeightType {
    fn from(value: String) -> Self {
        match value {
            _ => Self::Eucl2d
        }
    }
}

#[derive(Debug)]
pub struct Data {    
    // Specified in the files
    pub name: String,
    pub tsp_type: String,
    pub dimension: i32,
    pub edge_weight_type: EdgeWeightType,
    pub points: Vec<(f32, f32)>,
}

#[derive(Debug)]
struct ExpResult {
    pub name: String,
    pub points: Vec<(f32, f32)>,
    pub dist: i32,
}



const REL_PATH: &str = "./data/";

fn load_files() -> Vec<String> {
    const FILE_NAMES: [&str; 5] = [
        "qa194.tsp", // Quatar
        "dj38.tsp", // Djibouti
        "uy734.tsp", // Uruguay
        "wi29.tsp", // Western Sahara
        "zi929.tsp", // Zimbabwe
    ];
    
    let mut files = Vec::new();
    let rel_path = PathBuf::from(REL_PATH);
    let base_path = path::absolute(rel_path).unwrap();

    for file in FILE_NAMES.iter() {
        let file_path = base_path.join(*file);
        
        let contents = fs::read_to_string(
                &file_path
            ).expect(format!("Couldn't read file: {}", file_path.to_string_lossy()).as_str());
        files.push(contents);
    }

    files
}

fn parse_file(file: &String) -> Data {
    let mut data: Data = Data { 
        name: String::new(), tsp_type: String::new(), 
        dimension: 0, edge_weight_type: EdgeWeightType::Eucl2d, 
        points: Vec::new() 
    };

    const TOKEN_LIST: [&str; 5] = [
        "NAME", "COMMENT", "TYPE", "DIMENSION", "EDGE_WEIGHT_TYPE",
    ];

    const POINTS_SECTION: &str = "NODE_COORD_SECTION";

    for line in file.split('\n') {
        let tokens: Vec<&str> = line.split(' ').collect();

        let token = tokens[0].trim_end_matches(|x: char| {
            x == ':' || x.is_whitespace()
        });

        if TOKEN_LIST.contains(&token) {
            let value_token = if tokens[1] == ":" {tokens[2].trim()} else {tokens[1].trim()};

            match token {
                "NAME" => data.name = value_token.to_string(),
                "TYPE" => data.tsp_type = value_token.to_string(),
                "DIMENSION" => data.dimension = value_token.parse::<i32>().unwrap(),
                "EDGE_WEIGHT_TYPE" => data.edge_weight_type = EdgeWeightType::from(value_token.to_string()),
                _ => {}
            }
        } else if token.contains(POINTS_SECTION) {
            for point_line in file.split(POINTS_SECTION).nth(1).unwrap().split('\n') {
                let point_tokens: Vec<&str> = point_line.split(' ').filter(|s| !s.is_empty()).collect();
                if point_tokens.len() < 3 {
                    continue;
                }
                let x = point_tokens[1].trim().parse::<f32>().unwrap();
                let y = point_tokens[2].trim().parse::<f32>().unwrap();
                data.points.push((x, y));
            }
            break;
        }
    }

    data
}

pub fn load_data() -> Vec<Data> {
    let files = load_files();
    let mut data = Vec::with_capacity(files.len());

    for file in files {
        data.push(parse_file(&file));
    }

    data
}

fn calc_distance(points: &Vec<(f32, f32)>) -> i32 {

    let mut total = 0i32;
    for i in 1..points.len() {
        let (px, py) = points[i-1];
        let (x, y) = points[i];
        total += ((px - x).powf(2.0) + (py - y).powf(2.0)).sqrt().round() as i32;
    }

    total
}

fn run_signle_experiment(data: &Data, groups: i32, samples_per_group: i32, rng: &mut dyn Rng) -> i32 {
    const MAX: i32 = ((1 << 31) as i32).wrapping_sub(1);
    let mut result = 0i32;

    for group in 0..groups {

        let mut min = MAX;
        for _i in 0..samples_per_group {
            let mut points = data.points.clone();
            points.shuffle(rng);

            let distance = calc_distance(&points);
            min = std::cmp::min(min, distance);
        }

        result += (min - result) / (group + 1);
    }

    result
}

pub fn run_experiment() {
    let data_vec = load_data();
    let mut rng = rand::rng();

    for data in data_vec.iter() {
        std::println!("{}", data.name);
        std::println!("groups = 100, samples = 10: {}", run_signle_experiment(data, 100, 10, &mut rng));
        std::println!("groups = 20, samples = 50: {}", run_signle_experiment(data, 20, 50, &mut rng));
        std::println!("groups = 1, samlples = 1000: {}", run_signle_experiment(data, 1, 1000, &mut rng));
    }
    
}

