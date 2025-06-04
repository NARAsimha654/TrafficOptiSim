# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -g # Added -g for debugging
INC_DIR = ./include
SRC_DIR = ./src
TEST_DIR = ./tests
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Ensure obj and bin directories exist
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BIN_DIR))

# Source files for the main library
LIB_SRCS = $(SRC_DIR)/graph.cpp $(SRC_DIR)/vehicle.cpp
LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LIB_SRCS))

# Test targets
TEST_EXECS = $(BIN_DIR)/test_graph $(BIN_DIR)/test_routing

# Default target: build all test executables
all: $(TEST_EXECS)

# --- Object file compilation rules ---

# Specific rules for library objects are more robust for dependencies:
$(OBJ_DIR)/graph.o: $(SRC_DIR)/graph.cpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/vehicle.o: $(SRC_DIR)/vehicle.cpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Rules for test object files
$(OBJ_DIR)/test_graph.o: $(TEST_DIR)/test_graph.cpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/test_routing.o: $(TEST_DIR)/test_routing.cpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# --- Test executable linking rules ---

# Rule to link the test_graph executable
$(BIN_DIR)/test_graph: $(OBJ_DIR)/test_graph.o $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# Rule to link the test_routing executable
# It needs test_routing.o, vehicle.o, and graph.o
$(BIN_DIR)/test_routing: $(OBJ_DIR)/test_routing.o $(OBJ_DIR)/vehicle.o $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# --- Utility targets ---

# Rule to run all tests
run_tests: $(TEST_EXECS)
	@echo "Running all tests..."
	@echo "--- Running Graph Tests ---"
	@$(BIN_DIR)/test_graph
	@echo "--- Running Routing Tests ---"
	@$(BIN_DIR)/test_routing
	@echo "All tests finished."

# Clean rule
clean:
	@echo "Cleaning up..."
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*
	@echo "Cleanup complete."

.PHONY: all run_tests clean
