# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -g
INC_PATHS = ./include ./visualization # Holds space-separated paths
SRC_DIR = ./src
VIS_SRC_DIR = ./visualization
TEST_DIR = ./tests
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Generate -I flags from INC_PATHS
INCLUDE_FLAGS = $(patsubst %,-I%,$(INC_PATHS))

# Ensure obj and bin directories exist
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BIN_DIR))

# --- Source files ---
LIB_SRCS = $(SRC_DIR)/graph.cpp $(SRC_DIR)/vehicle.cpp $(SRC_DIR)/intersection.cpp $(SRC_DIR)/simulation.cpp \
           $(SRC_DIR)/optimizer.cpp $(SRC_DIR)/utils.cpp $(VIS_SRC_DIR)/visualizer.cpp
LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%.cpp,$(LIB_SRCS))) \
           $(patsubst $(VIS_SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(filter $(VIS_SRC_DIR)/%.cpp,$(LIB_SRCS)))

MAIN_SRC = $(SRC_DIR)/main.cpp
MAIN_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRC))

# Old individual tests (can be kept or removed if test_traffic_flow covers them)
TEST_GRAPH_SRC = $(TEST_DIR)/test_graph.cpp
TEST_ROUTING_SRC = $(TEST_DIR)/test_routing.cpp
TEST_INTERSECTION_SRC = $(TEST_DIR)/test_intersection.cpp
TEST_SIMULATION_SRC = $(TEST_DIR)/test_simulation.cpp

TEST_GRAPH_OBJ = $(OBJ_DIR)/test_graph.o
TEST_ROUTING_OBJ = $(OBJ_DIR)/test_routing.o
TEST_INTERSECTION_OBJ = $(OBJ_DIR)/test_intersection.o
TEST_SIMULATION_OBJ = $(OBJ_DIR)/test_simulation.o

# New comprehensive test file
TEST_TRAFFIC_FLOW_SRC = $(TEST_DIR)/test_traffic_flow.cpp
TEST_TRAFFIC_FLOW_OBJ = $(OBJ_DIR)/test_traffic_flow.o

# --- Executable targets ---
MAIN_EXEC = $(BIN_DIR)/traffic_sim
TEST_EXEC_GRAPH = $(BIN_DIR)/test_graph
TEST_EXEC_ROUTING = $(BIN_DIR)/test_routing
TEST_EXEC_INTERSECTION = $(BIN_DIR)/test_intersection
TEST_EXEC_SIMULATION = $(BIN_DIR)/test_simulation
TEST_EXEC_TRAFFIC_FLOW = $(BIN_DIR)/test_traffic_flow

ALL_TEST_EXECS = $(TEST_EXEC_GRAPH) $(TEST_EXEC_ROUTING) $(TEST_EXEC_INTERSECTION) $(TEST_EXEC_SIMULATION) $(TEST_EXEC_TRAFFIC_FLOW)

all: $(MAIN_EXEC) $(ALL_TEST_EXECS)

# --- Object file compilation rules ---

# Library objects
$(OBJ_DIR)/graph.o: $(SRC_DIR)/graph.cpp $(INC_PATHS)/graph.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/vehicle.o: $(SRC_DIR)/vehicle.cpp $(INC_PATHS)/vehicle.hpp $(INC_PATHS)/graph.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/intersection.o: $(SRC_DIR)/intersection.cpp $(INC_PATHS)/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/simulation.o: $(SRC_DIR)/simulation.cpp $(INC_PATHS)/simulation.hpp $(INC_PATHS)/graph.hpp $(INC_PATHS)/vehicle.hpp $(INC_PATHS)/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/optimizer.o: $(SRC_DIR)/optimizer.cpp $(INC_PATHS)/optimizer.hpp $(INC_PATHS)/graph.hpp $(INC_PATHS)/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/utils.o: $(SRC_DIR)/utils.cpp $(INC_PATHS)/utils.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(OBJ_DIR)/visualizer.o: $(VIS_SRC_DIR)/visualizer.cpp $(INC_PATHS)/visualizer.hpp $(INC_PATHS)/graph.hpp $(INC_PATHS)/vehicle.hpp $(INC_PATHS)/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Main application object
$(OBJ_DIR)/main.o: $(MAIN_SRC) $(INC_PATHS)/simulation.hpp $(INC_PATHS)/graph.hpp $(INC_PATHS)/vehicle.hpp $(INC_PATHS)/intersection.hpp $(INC_PATHS)/optimizer.hpp $(INC_PATHS)/utils.hpp $(INC_PATHS)/visualizer.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Test objects
$(TEST_GRAPH_OBJ): $(TEST_GRAPH_SRC) $(INC_PATHS)/graph.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(TEST_ROUTING_OBJ): $(TEST_ROUTING_SRC) $(INC_PATHS)/vehicle.hpp $(INC_PATHS)/graph.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(TEST_INTERSECTION_OBJ): $(TEST_INTERSECTION_SRC) $(INC_PATHS)/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(TEST_SIMULATION_OBJ): $(TEST_SIMULATION_SRC) $(INC_PATHS)/simulation.hpp $(INC_PATHS)/graph.hpp $(INC_PATHS)/vehicle.hpp $(INC_PATHS)/intersection.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(TEST_TRAFFIC_FLOW_OBJ): $(TEST_TRAFFIC_FLOW_SRC) $(INC_PATHS)/graph.hpp $(INC_PATHS)/vehicle.hpp $(INC_PATHS)/intersection.hpp $(INC_PATHS)/simulation.hpp $(INC_PATHS)/utils.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# --- Executable linking rules ---
$(MAIN_EXEC): $(MAIN_OBJ) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_GRAPH): $(TEST_GRAPH_OBJ) $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_ROUTING): $(TEST_ROUTING_OBJ) $(OBJ_DIR)/vehicle.o $(OBJ_DIR)/graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_INTERSECTION): $(TEST_INTERSECTION_OBJ) $(OBJ_DIR)/intersection.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# For TEST_EXEC_SIMULATION, decide if it links against all LIB_OBJS or a filtered list.
# Assuming it's an older test that might not need newer optimizer/utils/visualizer:
OLD_LIB_OBJS_FOR_TEST_SIM = $(OBJ_DIR)/graph.o $(OBJ_DIR)/vehicle.o $(OBJ_DIR)/intersection.o $(OBJ_DIR)/simulation.o
$(TEST_EXEC_SIMULATION): $(TEST_SIMULATION_OBJ) $(OLD_LIB_OBJS_FOR_TEST_SIM)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXEC_TRAFFIC_FLOW): $(TEST_TRAFFIC_FLOW_OBJ) $(LIB_OBJS) # New test links against all lib objects
	$(CXX) $(CXXFLAGS) $^ -o $@

# --- Utility targets ---
run_tests: $(ALL_TEST_EXECS)
	@echo "Running all tests..."
	@$(TEST_EXEC_GRAPH)
	@$(TEST_EXEC_ROUTING)
	@$(TEST_EXEC_INTERSECTION)
	@$(TEST_EXEC_SIMULATION)
	@$(TEST_EXEC_TRAFFIC_FLOW)
	@echo "All tests finished."

clean:
	@echo "Cleaning up..."
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*
	@echo "Cleanup complete."

.PHONY: all run_tests clean
