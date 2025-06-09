# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -g
INC_DIR = ./include ./visualization # Added ./visualization
SRC_DIR = ./src
VIS_SRC_DIR = ./visualization # Added for visualizer.cpp
TEST_DIR = ./tests
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Ensure obj and bin directories exist
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BIN_DIR))

# --- Source files ---
# Library source files (components of the simulation logic)
LIB_SRCS = $(SRC_DIR)/graph.cpp $(SRC_DIR)/vehicle.cpp $(SRC_DIR)/intersection.cpp $(SRC_DIR)/simulation.cpp \
           $(SRC_DIR)/optimizer.cpp $(SRC_DIR)/utils.cpp $(VIS_SRC_DIR)/visualizer.cpp
LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%.cpp,$(LIB_SRCS))) \
           $(patsubst $(VIS_SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(filter $(VIS_SRC_DIR)/%.cpp,$(LIB_SRCS)))

# Main application source file
MAIN_SRC = $(SRC_DIR)/main.cpp
MAIN_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRC))

# Test source files
# Old individual tests (can be kept or removed if test_traffic_flow covers them)
TEST_GRAPH_SRC = $(TEST_DIR)/test_graph.cpp
TEST_ROUTING_SRC = $(TEST_DIR)/test_routing.cpp
TEST_INTERSECTION_SRC = $(TEST_DIR)/test_intersection.cpp
TEST_SIMULATION_SRC = $(TEST_DIR)/test_simulation.cpp # This might be the old comprehensive one

TEST_GRAPH_OBJ = $(OBJ_DIR)/test_graph.o
TEST_ROUTING_OBJ = $(OBJ_DIR)/test_routing.o
TEST_INTERSECTION_OBJ = $(OBJ_DIR)/test_intersection.o
TEST_SIMULATION_OBJ = $(OBJ_DIR)/test_simulation.o

# New comprehensive test file
TEST_TRAFFIC_FLOW_SRC = $(TEST_DIR)/test_traffic_flow.cpp
TEST_TRAFFIC_FLOW_OBJ = $(OBJ_DIR)/test_traffic_flow.o


# --- Executable targets ---
MAIN_EXEC = $(BIN_DIR)/traffic_sim

# Old test executables
TEST_EXEC_GRAPH = $(BIN_DIR)/test_graph
TEST_EXEC_ROUTING = $(BIN_DIR)/test_routing
TEST_EXEC_INTERSECTION = $(BIN_DIR)/test_intersection
TEST_EXEC_SIMULATION = $(BIN_DIR)/test_simulation # Old comprehensive test

# New test executable
TEST_EXEC_TRAFFIC_FLOW = $(BIN_DIR)/test_traffic_flow

ALL_TEST_EXECS = $(TEST_EXEC_GRAPH) $(TEST_EXEC_ROUTING) $(TEST_EXEC_INTERSECTION) $(TEST_EXEC_SIMULATION) $(TEST_EXEC_TRAFFIC_FLOW)

# Default target: build main application and all test executables
all: $(MAIN_EXEC) $(ALL_TEST_EXECS)

# --- Object file compilation rules ---

# Library objects
$(OBJ_DIR)/graph.o: $(SRC_DIR)/graph.cpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(OBJ_DIR)/vehicle.o: $(SRC_DIR)/vehicle.cpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(OBJ_DIR)/intersection.o: $(SRC_DIR)/intersection.cpp $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(OBJ_DIR)/simulation.o: $(SRC_DIR)/simulation.cpp $(INC_DIR)/simulation.hpp $(INC_DIR)/graph.hpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(OBJ_DIR)/optimizer.o: $(SRC_DIR)/optimizer.cpp $(INC_DIR)/optimizer.hpp $(INC_DIR)/graph.hpp $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(OBJ_DIR)/utils.o: $(SRC_DIR)/utils.cpp $(INC_DIR)/utils.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(OBJ_DIR)/visualizer.o: $(VIS_SRC_DIR)/visualizer.cpp $(INC_DIR)/visualizer.hpp $(INC_DIR)/graph.hpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@


# Main application object
$(OBJ_DIR)/main.o: $(MAIN_SRC) $(INC_DIR)/simulation.hpp $(INC_DIR)/graph.hpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/intersection.hpp $(INC_DIR)/optimizer.hpp $(INC_DIR)/utils.hpp $(INC_DIR)/visualizer.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

# Test objects
$(TEST_GRAPH_OBJ): $(TEST_GRAPH_SRC) $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(TEST_ROUTING_OBJ): $(TEST_ROUTING_SRC) $(INC_DIR)/vehicle.hpp $(INC_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(TEST_INTERSECTION_OBJ): $(TEST_INTERSECTION_SRC) $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(TEST_SIMULATION_OBJ): $(TEST_SIMULATION_SRC) $(INC_DIR)/simulation.hpp $(INC_DIR)/graph.hpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/intersection.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@

$(TEST_TRAFFIC_FLOW_OBJ): $(TEST_TRAFFIC_FLOW_SRC) $(INC_DIR)/graph.hpp $(INC_DIR)/vehicle.hpp $(INC_DIR)/intersection.hpp $(INC_DIR)/simulation.hpp $(INC_DIR)/utils.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I./include -c $< -o $@


# --- Executable linking rules ---

# Main simulation executable
$(MAIN_EXEC): $(MAIN_OBJ) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Test executables
$(TEST_EXEC_GRAPH): $(TEST_GRAPH_OBJ) $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_ROUTING): $(TEST_ROUTING_OBJ) $(OBJ_DIR)/vehicle.o $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_INTERSECTION): $(TEST_INTERSECTION_OBJ) $(OBJ_DIR)/intersection.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_SIMULATION): $(TEST_SIMULATION_OBJ) $(filter-out $(OBJ_DIR)/optimizer.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/visualizer.o, $(LIB_OBJS)) # Links against old lib objects
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_TRAFFIC_FLOW): $(TEST_TRAFFIC_FLOW_OBJ) $(LIB_OBJS) # New test links against all lib objects
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
	@echo "--- Running Traffic Flow Tests (test_traffic_flow) ---"
	@$(TEST_EXEC_TRAFFIC_FLOW)
	@echo "All tests finished."

# Clean rule
clean:
	@echo "Cleaning up..."
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*
	@echo "Cleanup complete."

.PHONY: all run_tests clean
