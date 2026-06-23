use rand::{Rng, RngExt};
use std::collections::BTreeSet;

pub struct TabooList {
    buffer: Vec<(usize, usize)>,
    present_values: BTreeSet<(usize, usize)>,
    index: usize,
}

impl TabooList {
    pub fn new(n: usize) -> Self {
        Self {
            buffer: vec![(0, 0); n],
            present_values: BTreeSet::new(),
            index: 0usize,
        }
    }

    pub fn insert(&mut self, v: (usize, usize)) {
        self.present_values.remove(&self.buffer[self.index]);
        self.buffer[self.index] = v;
        self.index = (self.index + 1) % self.buffer.len();
        self.present_values.insert(v);
    }

    pub fn find(&self, v: (usize, usize)) -> bool {
        self.present_values.contains(&v) || self.present_values.contains(&(v.1, v.0))
    }
}

pub fn random_inversion(rng: &mut dyn Rng, n: usize) -> (usize, usize) {
    let i = rng.random_range(0..n - 1);
    (i, rng.random_range(i + 1..n))
}

pub fn print_starting(algo_name: &str, data_set: &str, print: bool) {
    if print {
        println!(
            "\x1b[1;36m[{}] Starting optimization on {} vertices\x1b[0m",
            algo_name, data_set
        );
    }
}

pub fn print_progress(tag: &str, print: bool, n: usize, best_distance: i64, i: usize) {
    if print && (i % 10 == 0 || i == n - 1) {
        let progress = ((i + 1) as f64 / n as f64 * 100.0) as u32;
        let bar_width = 30;
        let filled = (progress as usize * bar_width / 100).min(bar_width);
        let bar = format!("[{}{}]", "=".repeat(filled), " ".repeat(bar_width - filled));
        print!(
            "\r\x1b[1;34m[{}]\x1b[0m {} {}/{} ({}%) | Best: {}",
            tag,
            bar,
            i + 1,
            n,
            progress,
            best_distance
        );
        std::io::Write::flush(&mut std::io::stdout()).ok();
    }
}

pub fn print_complete(algo_name: &str, print: bool) {
    if print {
        println!("\n\x1b[1;32m[{algo_name}] Complete\x1b[0m");
    }
}
