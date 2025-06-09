# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -g
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# --- Directories and Paths ---
# Renamed to INC_PATHS for clarity, contains space-separated paths
INC_PATHS = ./include ./visualization
SRC_DIR = ./src
VIS_SRC_DIR = ./visualization
TEST_DIR = ./tests
OBJ_DIR = ./obj
BIN_DIR = ./bin

# CORRECTED: Generate -I flags from INC_PATHS for the compiler.
# This is a cleaner way to handle multiple include directories.
INCLUDE_FLAGS = $(patsubst %,-I%,$(INC_PATHS))

# Ensure obj and bin directories exist before compiling
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))


# --- Source and Object File Definitions ---

# Library source files (components of the simulation logic)
LIB_SRCS = $(SRC_DIR)/graph.cpp $(SRC_DIR)/vehicle.cpp $(SRC_DIR)/intersection.cpp $(SRC_DIR)/simulation.cpp \
           $(SRC_DIR)/optimizer.cpp $(SRC_DIR)/utils.cpp $(VIS_SRC_DIR)/visualizer.cpp
LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%.cpp,$(LIB_SRCS))) \
           $(patsubst $(VIS_SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(filter $(VIS_SRC_DIR)/%.cpp,$(LIB_SRCS)))

# Main application source file
MAIN_SRC = $(SRC_DIR)/main.cpp
MAIN_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRC))

# Test source files
TEST_GRAPH_SRC = $(TEST_DIR)/test_graph.cpp
TEST_ROUTING_SRC = $(TEST_DIR)/test_routing.cpp
TEST_INTERSECTION_SRC = $(TEST_DIR)/test_intersection.cpp
TEST_SIMULATION_SRC = $(TEST_DIR)/test_simulation.cpp
TEST_TRAFFIC_FLOW_SRC = $(TEST_DIR)/test_traffic_flow.cpp

TEST_GRAPH_OBJ = $(OBJ_DIR)/test_graph.o
TEST_ROUTING_OBJ = $(OBJ_DIR)/test_routing.o
TEST_INTERSECTION_OBJ = $(OBJ_DIR)/test_intersection.o
TEST_SIMULATION_OBJ = $(OBJ_DIR)/test_simulation.o
TEST_TRAFFIC_FLOW_OBJ = $(OBJ_DIR)/test_traffic_flow.o


# --- Executable Targets ---
MAIN_EXEC = $(BIN_DIR)/traffic_sim
TEST_EXEC_GRAPH = $(BIN_DIR)/test_graph
TEST_EXEC_ROUTING = $(BIN_DIR)/test_routing
TEST_EXEC_INTERSECTION = $(BIN_DIR)/test_intersection
TEST_EXEC_SIMULATION = $(BIN_DIR)/test_simulation
TEST_EXEC_TRAFFIC_FLOW = $(BIN_DIR)/test_traffic_flow

ALL_TEST_EXECS = $(TEST_EXEC_GRAPH) $(TEST_EXEC_ROUTING) $(TEST_EXEC_INTERSECTION) $(TEST_EXEC_SIMULATION) $(TEST_EXEC_TRAFFIC_FLOW)

# Default target: build main application and all test executables
all: $(MAIN_EXEC) $(ALL_TEST_EXECS)


# --- Object File Compilation Rules ---
# The header paths in the dependency list (after the colon) have been fixed.
# The compile command now uses the clean $(INCLUDE_FLAGS) variable.

# Library objects
$(OBJ_DIR)/graph.o: $(SRC_DIR)/graph.cpp ./include/graph.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/vehicle.o: $(SRC_DIR)/vehicle.cpp ./include/vehicle.hpp ./include/graph.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/intersection.o: $(SRC_DIR)/intersection.cpp ./include/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/simulation.o: $(SRC_DIR)/simulation.cpp ./include/simulation.hpp ./include/graph.hpp ./include/vehicle.hpp ./include/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/optimizer.o: $(SRC_DIR)/optimizer.cpp ./include/optimizer.hpp ./include/graph.hpp ./include/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/utils.o: $(SRC_DIR)/utils.cpp ./include/utils.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/visualizer.o: $(VIS_SRC_DIR)/visualizer.cpp ./visualization/visualizer.hpp ./include/graph.hpp ./include/vehicle.hpp ./include/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Main application object
$(OBJ_DIR)/main.o: $(MAIN_SRC) ./include/simulation.hpp ./include/graph.hpp ./include/vehicle.hpp ./include/intersection.hpp ./include/optimizer.hpp ./include/utils.hpp ./visualization/visualizer.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Test objects
$(TEST_GRAPH_OBJ): $(TEST_GRAPH_SRC) ./include/graph.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(TEST_ROUTING_OBJ): $(TEST_ROUTING_SRC) ./include/vehicle.hpp ./include/graph.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(TEST_INTERSECTION_OBJ): $(TEST_INTERSECTION_SRC) ./include/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(TEST_SIMULATION_OBJ): $(TEST_SIMULATION_SRC) ./include/simulation.hpp ./include/graph.hpp ./include/vehicle.hpp ./include/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(TEST_TRAFFIC_FLOW_OBJ): $(TEST_TRAFFIC_FLOW_SRC) ./include/graph.hpp ./include/vehicle.hpp ./include/intersection.hpp ./include/simulation.hpp ./include/utils.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@


# --- Executable Linking Rules ---

# Main simulation executable
$(MAIN_EXEC): $(MAIN_OBJ) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Test executables
$(TEST_EXEC_GRAPH): $(TEST_GRAPH_OBJ) $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_ROUTING): $(TEST_ROUTING_OBJ) $(OBJ_DIR)/vehicle.o $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_INTERSECTION): $(TEST_INTERSECTION_OBJ) $(OBJ_DIR)/intersection.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_SIMULATION): $(TEST_SIMULATION_OBJ) $(filter-out $(OBJ_DIR)/optimizer.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/visualizer.o, $(LIB_OBJS))
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_TRAFFIC_FLOW): $(TEST_TRAFFIC_FLOW_OBJ) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)


# --- Utility Targets ---

# Rule to run all tests
run_tests: $(ALL_TEST_EXECS)
	@echo "Running all tests..."
	@echo "--- Running Graph Tests (test_graph) ---"
	@./$(TEST_EXEC_GRAPH)
	@echo "--- Running Routing Tests (test_routing) ---"
	@./$(TEST_EXEC_ROUTING)
	@echo "--- Running Intersection Tests (test_intersection) ---"
	@./$(TEST_EXEC_INTERSECTION)
	@echo "--- Running Simulation Tests (test_simulation) ---"
	@./$(TEST_EXEC_SIMULATION)
	@echo "--- Running Traffic Flow Tests (test_traffic_flow) ---"
	@./$(TEST_EXEC_TRAFFIC_FLOW)
	@echo "All tests finished."

# Clean rule
clean:
	@echo "Cleaning up..."
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*
	@echo "Cleanup complete."

.PHONY: all run_tests clean