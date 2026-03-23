# Static libraries (C, Ada, Rust)

Make sure that lib section in file [Cargo.toml](./rust-nums/Cargo.toml) looks like this:

```
[lib]
name = "rust_nums"
crate-type = ["staticlib"]
```

Then compile the libraries

```sh
make c-link
```

```sh
make ada-link
```

```sh
make rust-link
```

# Python wrapper

Change the lib section 

```
[lib]
name = "rust_nums"
crate-type = ["cdylib"]
```

```sh
make python-link
python3 tests/python_test_runner.py
```

