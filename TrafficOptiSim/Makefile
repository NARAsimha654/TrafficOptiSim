# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread
INC_DIR = ./include
SRC_DIR = ./src
TEST_DIR = ./tests
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Ensure obj and bin directories exist
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BIN_DIR))

# Source files for the main library (currently only graph.cpp)
LIB_SRCS = $(SRC_DIR)/graph.cpp
LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LIB_SRCS))

# Test source files
TEST_SRCS = $(TEST_DIR)/test_graph.cpp
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/test_%.o,$(TEST_SRCS))
TEST_TARGET = $(BIN_DIR)/test_graph

# Default target
all: $(TEST_TARGET)

# Rule to compile library object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/%.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Rule to compile test object files (specific for test_graph.cpp for now)
$(OBJ_DIR)/test_graph.o: $(TEST_DIR)/test_graph.cpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Rule to link the test executable
$(TEST_TARGET): $(OBJ_DIR)/test_graph.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) $^ -o $@

# Rule to run the tests
run_tests: $(TEST_TARGET)
	@echo "Running tests..."
	@$(TEST_TARGET)
	@echo "Tests finished."

# Clean rule
clean:
	@echo "Cleaning up..."
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/$(TEST_TARGET) $(BIN_DIR)/*
	@echo "Cleanup complete."

.PHONY: all run_tests clean
