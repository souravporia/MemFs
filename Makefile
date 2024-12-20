# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17

# Target executables
TARGETS = memfs benchmark

# Source files
SRCS = src/FileSystem.cpp src/Schema.cpp src/VirtualDisk.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGETS)

# Link object files to create memfs executable
memfs: Main.o $(OBJS)
	$(CXX) -o $@ Main.o $(OBJS)

benchmark: BenchMark.o $(OBJS)
	$(CXX) -o $@ BenchMark.o $(OBJS)

# Compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) Main.o BenchMark.o $(TARGETS)

# Phony targets (targets that don't correspond to files)
.PHONY: all clean
