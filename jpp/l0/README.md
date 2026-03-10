### Ada

```sh
cd ada
gnatmake main.adb
./main
```

### C++

```sh
g++ -o ./cpp/main cpp/main.cpp
./cpp/main
```

### Go

```sh
go build -o ./go/main go/main.go
./go/main
```

### Java

```sh
cd java
javac Main.java
java Main
```

### LISP

```sh
clisp lisp/main.lisp
```

### OCaml

```sh
cd ocaml
opam exec -- dune init proj hello
cd hello
opam exec -- dune build
opam exec -- dune exec hello
```

### Prolog

```sh
swipl -s prolog/main.pl
```