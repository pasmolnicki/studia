CC = g++
OPT_FLAGS = -std=c++20 -O2 -pthread
DEBUG_FLAGS = -std=c++20 -g -pthread -fsanitize=address
COMM = $(CC) $(OPT_FLAGS)

philo: main.cpp
	$(COMM) -o main main.cpp
server: server.cpp
	$(COMM) -o server server.cpp

debug-server: server.cpp
	$(CC) $(DEBUG_FLAGS) -o server server.cpp
