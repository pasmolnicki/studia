# Generate shared library for the c code
CC = gcc
CFLAGS = -fPIC -shared
SHRD_TARGET = libnums.so
STATIC_TARGET = libnums.a
LDFLAGS = -L. -lnums
SRCS = nums.c
OBJS = $(SRCS:.c=.o)

ADA_NUMS = ada_nums.adb
ADA_OBJS = $(ADA_NUMS:.adb=.o)
ADA_ALI = $(ADA_NUMS:.adb=.ali)
ADA_LIB = libada_nums.a

RUST_LIB = rust-nums/target/release/librust_nums.a
RUST_LDFLAGS = -Lrust-nums/target/release -lrust_nums

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(SHRD_TARGET) $(STATIC_TARGET) main \
	main_ada main_ada_c main_ada_rust main_ada.o main_ada.ali \
	main_ada_rust.o main_ada_rust.ali b~* \
	ada/*.ali ada/$(ADA_OBJS) $(ADA_LIB) ada/b~* \
	nums_c_bindings.* nums_rust_bindings.* \
	main_rust_combined main_c_combined main_ada_combined \
	libada_nums.so 

c-static:
# Compile the C library (as static) then the main program
	$(CC) -c -o nums.o nums.c
	ar rcs $(STATIC_TARGET) nums.o


c-shared: main.c $(SRCS)
# Compile the C library as shared then the main program
	$(CC) -fPIC -c -o nums.o nums.c
	$(CC) -shared -o $(SHRD_TARGET) nums.o

# Build Rust dynamic library
rust-shared:
	cd rust-nums && cargo build --release && cd ..

# Build Ada dynamic library (produces libada_nums.so)
ada-shared: nums.c
# compile Ada unit and produce object files
	gnatmake -c ada/$(ADA_NUMS)
	mv ada_nums.ali ada/ada_nums.ali
	mv ada_nums.o ada/ada_nums.o
	gnatbind -Lnums ada/ada_nums
	gcc -fPIC -c b~ada_nums.adb -o ada/b~ada_nums.o
# create shared library linking GNAT runtime
	gcc -shared -o libada_nums.so ada/ada_nums.o ada/b~ada_nums.o -lgnat -lgcc -lm

# Build all shared libs
build-shared: c-shared rust-shared ada-shared
	@echo "Built shared libraries: libnums.so, librust_nums.so (in rust-nums/target/release), libada_nums.so"

# c-link: build Ada then Rust; don't require stale rust artifact path
c-link: ada/$(ADA_NUMS)
# Build C library
	$(CC) -c -o nums.o nums.c
	ar rcs $(STATIC_TARGET) nums.o
# Build Rust library
	cd rust-nums && cargo build --release && cd ..
# Build Ada library
	gnatmake -c ada/$(ADA_NUMS)
	mv ada_nums.ali ada/ada_nums.ali
	mv ada_nums.o ada/ada_nums.o
	gnatbind -Lnums ada/ada_nums
	gcc -c b~ada_nums.adb -o ada/b~ada_nums.o
	ar rcs $(ADA_LIB) ada/ada_nums.o ada/b~ada_nums.o
# Compile C main linking Rust (rust_ wrappers) and Ada (ada_ wrappers)
	$(CC) main.c -o main_c_combined -Lrust-nums/target/release -lrust_nums -L. -lnums -lada_nums -lgnat

rust-link: ada/$(ADA_NUMS) nums.c
# Build C library
	$(CC) -c -o nums.o nums.c
	ar rcs $(STATIC_TARGET) nums.o
# Build Ada library
	gnatmake -c ada/$(ADA_NUMS)
	mv ada_nums.ali ada/ada_nums.ali
	mv ada_nums.o ada/ada_nums.o
	gnatbind -Lnums ada/ada_nums
	gcc -c b~ada_nums.adb -o ada/b~ada_nums.o
	ar rcs $(ADA_LIB) ada/ada_nums.o ada/b~ada_nums.o
# Build Rust main linking C and Ada (link GNAT runtime)
	rustc main.rs -o main_rust_combined -l static=nums -L. -l static=ada_nums -L. -l gnat

ada-link: nums.c
# Build C library
	$(CC) -c -o nums.o nums.c
	ar rcs $(STATIC_TARGET) nums.o
# Build Rust library
	cd rust-nums && cargo build --release && cd ..
# Build and link Ada main with C and Rust
	gnatmake -Iada main_ada -o main_ada_combined -largs -L. -lnums -Lrust-nums/target/release -lrust_nums -lgnat


python-link: python/python_test_runner.py
# Build C library
	$(CC) -c -o nums.o nums.c
	$(CC) -shared -o $(SHRD_TARGET) nums.o
# Build Rust library
	cd rust-nums && cargo build --release && cd ..
# Build Ada library
	gnatmake -c ada/$(ADA_NUMS)
	mv ada_nums.ali ada/ada_nums.ali
	mv ada_nums.o ada/ada_nums.o
	gnatbind -Lnums ada/ada_nums
	gcc -fPIC -c b~ada_nums.adb -o ada/b~ada_nums.o
# create shared library linking GNAT runtime
	gcc -shared -o libada_nums.so ada/ada_nums.o ada/b~ada_nums.o -lgnat -lgcc -lm