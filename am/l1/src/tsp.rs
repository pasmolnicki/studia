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
use serde::{Serialize};

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
    pub points: VecPoints,
}

#[derive(Debug, Clone, Serialize)]
pub struct VecPoints {
    pub points: Vec<(f32, f32)>,
    pub name: String,
}

pub fn point_distance(p1: (f32, f32), p2: (f32, f32)) -> i32 {
    let (px, py) = p1;
    let (x, y) = p2;
    ((px - x).powf(2.0) + (py - y).powf(2.0)).sqrt().round() as i32
}

impl VecPoints {
    #[must_use]
    pub fn new() -> Self {
        Self { points: Vec::new(), name: String::new() }
    }

    pub fn calc_distance(&self) -> i32 {
        let points = &self.points;
        let mut total = 0i32;
        for i in 1..points.len() {
            total += point_distance(points[i - 1], points[i]);
        }

        total
    }

    pub fn permutation(&self, rng: &mut dyn Rng) -> Self {
        let mut points = self.points.clone();
        points.shuffle(rng);
        Self { points, name: self.name.clone() }
    }

    pub fn set_name(&mut self, name: String) {
        self.name = name;
    }
}


const REL_PATH: &str = "./data/";
const OUTPUT_PATH: &str = "./results/";

fn load_files(file_names: &[&str]) -> Vec<String> {
    
    let mut files = Vec::new();
    let rel_path = PathBuf::from(REL_PATH);
    let base_path = path::absolute(rel_path).unwrap();

    for file in file_names.iter() {
        let file_path = base_path.join(*file);
        
        let contents = fs::read_to_string(&file_path)
            .expect(format!("Couldn't read file: {}", file_path.to_string_lossy()).as_str());
        files.push(contents);
    }

    files
}

fn parse_file(file: &String) -> Data {
    let mut data: Data = Data { 
        name: String::new(), tsp_type: String::new(), 
        dimension: 0, edge_weight_type: EdgeWeightType::Eucl2d, 
        points: VecPoints::new()
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
                data.points.points.push((x, y));
            }
            break;
        }
    }

    data
}

pub fn load_data(file_names: &[&str]) -> Vec<Data> {
    let files = load_files(file_names);
    let mut data = Vec::with_capacity(files.len());

    for file in files {
        data.push(parse_file(&file));
    }

    data
}

// fn run_single_experiment(points: &Vec<VecPoints>, groups: i32, samples_per_group: i32, name: &str) -> (ExpResult, VecPoints) {
//     assert!(points.len() <= (groups * samples_per_group) as usize, "Invalid groups and samples params");
    
//     const MAX: i32 = ((1 << 31) as i32).wrapping_sub(1);
//     let mut mean = 0i32;
//     let mut min_values = Vec::with_capacity(groups as usize);
//     let mut best_solution = VecPoints::new();

//     for group in 0..groups {
//         let mut min = MAX;
//         for i in 0..samples_per_group {
//             let p = &points[(group * samples_per_group + i) as usize];
//             let dist = p.calc_distance();
//             if min > dist {
//                 best_solution = p.clone();
//                 min = dist;
//             }
//         }
        
//         min_values.push(min);
//         mean += (min - mean) / (group + 1);
//     }

//     (ExpResult { 
//         name: format!("{}-{}-{}", name, groups, samples_per_group), 
//         mean, min_values, groups, samples_per_group }, 
//     best_solution)
// }

