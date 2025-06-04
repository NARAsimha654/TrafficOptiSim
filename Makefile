# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -g
INC_DIR = ./include
SRC_DIR = ./src
TEST_DIR = ./tests
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Ensure obj and bin directories exist
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BIN_DIR))

# --- Source files ---
# Library source files (components of the simulation logic)
LIB_SRCS = $(SRC_DIR)/graph.cpp $(SRC_DIR)/vehicle.cpp $(SRC_DIR)/intersection.cpp $(SRC_DIR)/simulation.cpp
LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LIB_SRCS))

# Main application source file
MAIN_SRC = $(SRC_DIR)/main.cpp
MAIN_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRC))

# Test source files (individual test suites)
TEST_GRAPH_SRC = $(TEST_DIR)/test_graph.cpp
TEST_ROUTING_SRC = $(TEST_DIR)/test_routing.cpp
TEST_INTERSECTION_SRC = $(TEST_DIR)/test_intersection.cpp
TEST_SIMULATION_SRC = $(TEST_DIR)/test_simulation.cpp

TEST_GRAPH_OBJ = $(OBJ_DIR)/test_graph.o
TEST_ROUTING_OBJ = $(OBJ_DIR)/test_routing.o
TEST_INTERSECTION_OBJ = $(OBJ_DIR)/test_intersection.o
TEST_SIMULATION_OBJ = $(OBJ_DIR)/test_simulation.o

# --- Executable targets ---
MAIN_EXEC = $(BIN_DIR)/traffic_sim
TEST_EXEC_GRAPH = $(BIN_DIR)/test_graph
TEST_EXEC_ROUTING = $(BIN_DIR)/test_routing
TEST_EXEC_INTERSECTION = $(BIN_DIR)/test_intersection
TEST_EXEC_SIMULATION = $(BIN_DIR)/test_simulation

ALL_TEST_EXECS = $(TEST_EXEC_GRAPH) $(TEST_EXEC_ROUTING) $(TEST_EXEC_INTERSECTION) $(TEST_EXEC_SIMULATION)

# Default target: build main application and all test executables
all: $(MAIN_EXEC) $(ALL_TEST_EXECS)

# --- Object file compilation rules ---

# Library objects
$(OBJ_DIR)/graph.o: $(SRC_DIR)/graph.cpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/vehicle.o: $(SRC_DIR)/vehicle.cpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/intersection.o: $(SRC_DIR)/intersection.cpp $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/simulation.o: $(SRC_DIR)/simulation.cpp $(INC_DIR)/simulation.hpp $(INC_DIR)/graph.hpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Main application object
$(OBJ_DIR)/main.o: $(MAIN_SRC) $(INC_DIR)/simulation.hpp $(INC_DIR)/graph.hpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Test objects
$(TEST_GRAPH_OBJ): $(TEST_GRAPH_SRC) $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(TEST_ROUTING_OBJ): $(TEST_ROUTING_SRC) $(INC_DIR)/vehicle.hpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(TEST_INTERSECTION_OBJ): $(TEST_INTERSECTION_SRC) $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(TEST_SIMULATION_OBJ): $(TEST_SIMULATION_SRC) $(INC_DIR)/simulation.hpp $(INC_DIR)/graph.hpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@


# --- Executable linking rules ---

# Main simulation executable
$(MAIN_EXEC): $(MAIN_OBJ) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Test executables
$(TEST_EXEC_GRAPH): $(TEST_GRAPH_OBJ) $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_ROUTING): $(TEST_ROUTING_OBJ) $(OBJ_DIR)/vehicle.o $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# test_intersection needs intersection.o (which includes light_state_to_string)
$(TEST_EXEC_INTERSECTION): $(TEST_INTERSECTION_OBJ) $(OBJ_DIR)/intersection.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_SIMULATION): $(TEST_SIMULATION_OBJ) $(LIB_OBJS) # Simulation tests will need all lib components
	$(CXX) $(CXXFLAGS) $^ -o $@


# --- Utility targets ---

# Rule to run all tests
run_tests: $(ALL_TEST_EXECS)
	@echo "Running all tests..."
	@echo "--- Running Graph Tests (test_graph) ---"
	@$(TEST_EXEC_GRAPH)
	@echo "--- Running Routing Tests (test_routing) ---"
	@$(TEST_EXEC_ROUTING)
	@echo "--- Running Intersection Tests (test_intersection) ---"
	@$(TEST_EXEC_INTERSECTION)
	@echo "--- Running Simulation Tests (test_simulation) ---"
	@$(TEST_EXEC_SIMULATION)
	@echo "All tests finished."

# Clean rule
clean:
	@echo "Cleaning up..."
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*
	@echo "Cleanup complete."

.PHONY: all run_tests clean
