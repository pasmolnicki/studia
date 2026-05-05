/*
Dla zbiorów Western Sahara, Djibouti, Quatar, Uruguay i Zimbabwe wykonaj
następujące zadania:
1. Wylosuj 1000 permutacji wierzchołków i policz
    (a) średnią z minimum dla każdych 10 kolejnych losowań (100 grup po 10 losowań),
    (b) średnią z minimum dla każdych 50 kolejnych losowań (20 grup po 50 losowań),
    (c) i minimalną wartość dla tych 1000 losowań
*/

use std::{error::Error, fs, path};
use std::path::PathBuf;
use rand::Rng;
use rand::seq::SliceRandom;
use serde::{Serialize};
use plotters::prelude::*;

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
    pub points: Vec<(f64, f64)>,
    pub name: String,
}

pub fn point_distance(p1: (f64, f64), p2: (f64, f64)) -> f64 {
    let (px, py) = p1;
    let (x, y) = p2;
    ((px - x).powf(2.0) + (py - y).powf(2.0)).sqrt()
}

impl VecPoints {
    #[must_use]
    pub fn new() -> Self {
        Self { points: Vec::new(), name: String::new() }
    }

    pub fn calc_distance(&self) -> f64 {
        let points = &self.points;
        let mut total = 0f64;
        for i in 1..points.len() {
            total += point_distance(points[i - 1], points[i]);
        }

        total
    }

    // pub fn distance_matrix(&self) -> Vec<Vec<u64>> {
    //     let mut m = vec![vec![0u64; self.points.len()]; self.points.len()];

    //     for (i, v) in m.iter_mut().enumerate() {
    //         for (j, val) in v.iter_mut().enumerate() {
    //             *val = point_distance(self.points[i], self.points[j])
    //         }
    //     }

    //     m
    // }

    pub fn permutation(&self, rng: &mut dyn Rng) -> Self {
        let mut points = self.points.clone();
        points.shuffle(rng);
        Self { points, name: self.name.clone() }
    }

    pub fn set_name(&mut self, name: String) {
        self.name = name;
    }

    // Treats points as a cycle, so each point is connected to the next one
    // and the last one is connected to the first one
    pub fn visualize(&self, file_name: Option<&str>) -> Result<PathBuf, Box<dyn Error>> {
        if self.points.is_empty() {
            return Err("cannot visualize an empty point set".into());
        }

        let output_path = match file_name {
            Some(file_name) => {
                let path = PathBuf::from(file_name);
                if path.extension().is_none() {
                    path.with_extension("png")
                } else {
                    path
                }
            }
            None => {
                let file_name = if self.name.is_empty() {
                    "tsp_visualization.png".to_string()
                } else {
                    format!("{}.png", self.name)
                };
                PathBuf::from(OUTPUT_PATH).join(file_name)
            }
        };

        if let Some(parent) = output_path.parent() {
            fs::create_dir_all(parent)?;
        }

        let (min_x, max_x) = self.points.iter().fold(
            (f64::INFINITY, f64::NEG_INFINITY),
            |(min_x, max_x), (x, _)| (min_x.min(*x), max_x.max(*x)),
        );
        let (min_y, max_y) = self.points.iter().fold(
            (f64::INFINITY, f64::NEG_INFINITY),
            |(min_y, max_y), (_, y)| (min_y.min(*y), max_y.max(*y)),
        );

        let x_span = (max_x - min_x).abs();
        let y_span = (max_y - min_y).abs();
        let x_padding = if x_span == 0.0 { 1.0 } else { x_span * 0.1 };
        let y_padding = if y_span == 0.0 { 1.0 } else { y_span * 0.1 };

        let result_path = output_path.clone();
        let root = BitMapBackend::new(&output_path, (1000, 800)).into_drawing_area();
        root.fill(&WHITE)?;

        let mut chart = ChartBuilder::on(&root)
            .margin(20)
            .build_cartesian_2d(
                (min_x - x_padding)..(max_x + x_padding),
                (min_y - y_padding)..(max_y + y_padding),
            )?;

        let mut cycle_points = self.points.clone();
        if cycle_points.len() > 1 {
            cycle_points.push(cycle_points[0]);
        }

        chart.draw_series(LineSeries::new(cycle_points, &BLUE.mix(0.8)))?;

        chart.draw_series(self.points.iter().map(|(x, y)| Circle::new((*x, *y), 5, RED.filled())))?;

        root.present()?;
        Ok(result_path)
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
                let x = point_tokens[1].trim().parse::<f64>().unwrap();
                let y = point_tokens[2].trim().parse::<f64>().unwrap();
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

