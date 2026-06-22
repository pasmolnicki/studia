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
