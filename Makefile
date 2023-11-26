# please modify here to decide where to put tests
TEST_DIR=tests
TEST_IN_DIR=$(TEST_DIR)/in
TEST_OK_DIR=$(TEST_DIR)/ok

# please modify here to change the test generator
# it is expected to have the following interface:
# python3 $(GENERATOR) [seed] [test_path] [num_testcases] [n] [d]
GENERATOR=utils/generate.py

# the seed used for testing
SEED=42

# please modify here to change the name of the correctness test
CORRECTNESS_TEST=many_small
# please modify here to change its configuration: [z] [n] [d]
CORRECTNESS_CONFIG=1000 100 1000000000

# please modify here to change the name of the stress test
STRESS_TEST=big
# please modify here to change its configuration: [z] [n] [d]
STRESS_CONFIG=5 4000 1000000000

validate: $(TEST_OK_DIR)/$(CORRECTNESS_TEST).ok

# generate tests according to their configurations
generate: $(TEST_IN_DIR)/$(CORRECTNESS_TEST).in $(TEST_IN_DIR)/$(STRESS_TEST).in

# obtain the correct answers using the model solution (verified by Satori)
# careful! the model solution is not parallelized, so it may take a while!
solve: $(TEST_OK_DIR)/$(CORRECTNESS_TEST).ok $(TEST_OK_DIR)/$(STRESS_TEST).ok

clean:
	$(RM) $(EXE_DIR)

# careful! regenerating removed tests may take a while!
reset:
	$(RM) $(TEST_DIR)

# careful! performs reset
purge: clean reset

# internals start here, there should be no need for modifications below this line

HPP_DIR=headers
HEADERS=$(HPP_DIR)/problem.hpp

SOL_SRC_DIR=solutions

REC_SRC_DIR=recoveries
COMMON_REC_SRC=$(REC_SRC_DIR)/recursive.cpp

SOL_DEPS=main.cpp $(COMMON_REC_SRC) $(HEADERS)

EXE_DIR=execs
SOL_EXE_DIR=$(EXE_DIR)/solutions
REC_EXE_DIR=$(EXE_DIR)/recoveries

OK_SRC=submits/iterative_solution_recursive_recovery_ok_8805501.cpp
OK_EXE=$(EXE_DIR)/ok.x

CXX=g++
CXXFLAGS=-std=c++17 -O3 -Wall -Wextra -Werror
OPENMP_FLAGS=-fopenmp
COMPILE=$(CXX) $(CXXFLAGS)
COMPILE_CONFIGURATION=$(COMPILE) -I $(HPP_DIR) main.cpp

RM=rm -rf

# abandon hope, all ye who enter here

$(OK_EXE): $(OK_SRC)
	@mkdir -p $(EXE_DIR)
	$(COMPILE) $(OK_SRC) -o $(OK_EXE)

$(SOL_EXE_DIR)/openmp%.x: $(SOL_DEPS) $(SOL_SRC_DIR)/openmp%.cpp
	@mkdir -p $(SOL_EXE_DIR)
	$(COMPILE_CONFIGURATION) $(OPENMP_FLAGS) $(SOL_SRC_DIR)/openmp$*.cpp $(COMMON_REC_SRC) -o $@

$(SOL_EXE_DIR)/%.x: $(SOL_DEPS) $(SOL_SRC_DIR)/%.cpp
	@mkdir -p $(SOL_EXE_DIR)
	$(COMPILE_CONFIGURATION) $(SOL_SRC_DIR)/$*.cpp $(COMMON_REC_SRC) -o $@

$(TEST_IN_DIR)/$(CORRECTNESS_TEST).in: $(GENERATOR)
	@mkdir -p $(TEST_IN_DIR)
	python3 $(GENERATOR) $(SEED) $@ $(CORRECTNESS_CONFIG)

$(TEST_IN_DIR)/$(STRESS_TEST).in: $(GENERATOR)
	@mkdir -p $(TEST_IN_DIR)
	python3 $(GENERATOR) $(SEED) $@ $(STRESS_CONFIG)

$(TEST_OK_DIR)/$(CORRECTNESS_TEST).ok: $(TEST_IN_DIR)/$(CORRECTNESS_TEST).in $(OK_EXE)
	@mkdir -p $(TEST_OK_DIR)
	$(OK_EXE) < $(TEST_IN_DIR)/$(CORRECTNESS_TEST).in > $@

$(TEST_OK_DIR)/$(STRESS_TEST).ok: $(TEST_IN_DIR)/$(STRESS_TEST).in $(OK_EXE)
	@mkdir -p $(TEST_OK_DIR)
	$(OK_EXE) < $(TEST_IN_DIR)/$(STRESS_TEST).in > $@
