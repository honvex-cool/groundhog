# please modify here to decide where to put tests and their output
TEST_DIR=tests
TEST_IN_DIR=$(TEST_DIR)/in
TEST_OK_DIR=$(TEST_DIR)/ok
TEST_OUT_DIR=$(TEST_DIR)/out
REPORTS=reports
CHARTS=charts

# please modify here to change the test generator
# it is expected to have the following interface:
# python3 $(GENERATOR) seed test_path z n d
GENERATOR=utils/generate.py

# the seed used for testing
SEED=42

# please modify here to change the name of the sanity test
SANITY_TEST=many_small
SANITY_IN=$(TEST_IN_DIR)/$(SANITY_TEST).in
SANITY_OK=$(TEST_OK_DIR)/$(SANITY_TEST).ok
SANITY_OUT_DIR=$(TEST_OUT_DIR)/sanity_check
# please modify here to change its configuration: z n d
# the sanity test is supposed to be fairly small
SANITY_CONFIG=100 400 1000000000

# please modify here to change the name of the stress test
STRESS_TEST=big
STRESS_IN=$(TEST_IN_DIR)/$(STRESS_TEST).in
STRESS_OK=$(TEST_OK_DIR)/$(STRESS_TEST).ok
STRESS_OUT_DIR=$(TEST_OUT_DIR)/stress
# please modify here to change its configuration: z n d
STRESS_CONFIG=3 4000 1000000000

full: sanity_check stress

# this command is designed for running fairly quick and fairly convincing sanity tests
# so that we can catch errors early and avoid wasting time with wrong answers on the stress tests
sanity_check: configurations $(SANITY_OK)
	python3 utils/benchmark.py $(if $(machine),$(machine),machine) $(SOL_EXE_DIR) $(SANITY_IN) $(SANITY_OK) $(SANITY_OUT_DIR)/solutions $(REPORTS) $(CHARTS)
	python3 utils/benchmark.py $(if $(machine),$(machine),machine) $(REC_EXE_DIR) $(SANITY_IN) $(SANITY_OK) $(SANITY_OUT_DIR)/recoveries $(REPORTS) $(CHARTS)

stress: configurations $(STRESS_OK)
	python3 utils/benchmark.py $(if $(machine),$(machine),machine) $(SOL_EXE_DIR) $(STRESS_IN) $(STRESS_OK) $(STRESS_OUT_DIR)/solutions $(REPORTS) $(CHARTS)
	python3 utils/benchmark.py $(if $(machine),$(machine),machine) $(REC_EXE_DIR) $(STRESS_IN) $(STRESS_OK) $(STRESS_OUT_DIR)/recoveries $(REPORTS) $(CHARTS)

# generate tests according to their configurations
generate: $(SANITY_IN) $(STRESS_IN)

# obtain the correct answers using the model solution (verified by Satori)
# careful! the model solution is not parallelized, so it may take a while!
solve: $(SANITY_OK) $(STRESS_OK)

clean: clean_execs clean_outputs

clean_execs:
	$(RM) $(EXE_DIR)

clean_outputs:
	$(RM) $(TEST_OUT_DIR) $(REPORTS) $(CHARTS)

# careful! computing correct answers for removed tests may take a while!
reset:
	$(RM) $(TEST_DIR)

# careful! performs reset
purge: clean reset

# internals start here, there should be no need for modifications below this line

HPP_DIR=headers
HEADERS=$(HPP_DIR)/problem.hpp

SOL_SRC_DIR=solutions
REC_SRC_DIR=recoveries

COMMON_SOL_SRC=$(SOL_SRC_DIR)/threads_middle_loop_barrier.cpp
COMMON_REC_SRC=$(REC_SRC_DIR)/recursive.cpp

SOL_DEPS=main.cpp $(COMMON_REC_SRC) $(HEADERS)
REC_DEPS=main.cpp $(COMMON_SOL_SRC) $(HEADERS)

EXE_DIR=execs
SOL_EXE_DIR=$(EXE_DIR)/solutions
REC_EXE_DIR=$(EXE_DIR)/recoveries

OK_SRC=submits/iterative_solution_recursive_recovery_ok_8805501.cpp
OK_EXE=$(EXE_DIR)/ok.x

CXX=g++
CXXFLAGS=-std=c++17 -O3 -Wall -Wextra -Werror -pthread
OPENMP_FLAGS=-fopenmp
COMPILE=$(CXX) $(CXXFLAGS)
COMPILE_CONFIGURATION=$(COMPILE) -I $(HPP_DIR) main.cpp

RM=rm -rf

# abandon hope, all ye who enter here

ALL_CONFIGURATIONS+=$(SOL_EXE_DIR)/sequential.x
ALL_CONFIGURATIONS+=$(SOL_EXE_DIR)/openmp_middle_loop.x
ALL_CONFIGURATIONS+=$(SOL_EXE_DIR)/openmp_middle_loop_barrier.x
ALL_CONFIGURATIONS+=$(SOL_EXE_DIR)/threads_middle_loop.x
ALL_CONFIGURATIONS+=$(SOL_EXE_DIR)/threads_middle_loop_barrier.x
ALL_CONFIGURATIONS+=$(REC_EXE_DIR)/iterative.x
ALL_CONFIGURATIONS+=$(REC_EXE_DIR)/recursive.x
ALL_CONFIGURATIONS+=$(REC_EXE_DIR)/threaded_iterative.x
ALL_CONFIGURATIONS+=$(REC_EXE_DIR)/threaded_recursive.x

configurations: $(ALL_CONFIGURATIONS)

$(OK_EXE): $(OK_SRC)
	@mkdir -p $(EXE_DIR)
	$(COMPILE) $(OK_SRC) -o $(OK_EXE)

$(SOL_EXE_DIR)/openmp%.x: $(SOL_DEPS) $(SOL_SRC_DIR)/openmp%.cpp
	@mkdir -p $(SOL_EXE_DIR)
	$(COMPILE_CONFIGURATION) $(OPENMP_FLAGS) $(SOL_SRC_DIR)/openmp$*.cpp $(COMMON_REC_SRC) -o $@

$(SOL_EXE_DIR)/%.x: $(SOL_DEPS) $(SOL_SRC_DIR)/%.cpp
	@mkdir -p $(SOL_EXE_DIR)
	$(COMPILE_CONFIGURATION) $(SOL_SRC_DIR)/$*.cpp $(COMMON_REC_SRC) -o $@

$(REC_EXE_DIR)/%.x: $(REC_DEPS) $(REC_SRC_DIR)/%.cpp
	@mkdir -p $(REC_EXE_DIR)
	$(COMPILE_CONFIGURATION) $(REC_SRC_DIR)/$*.cpp $(COMMON_SOL_SRC) -o $@

$(REC_EXE_DIR)/threaded%.x: $(REC_DEPS) $(REC_SRC_DIR)/threaded%.cpp
	@mkdir -p $(REC_EXE_DIR)
	$(COMPILE_CONFIGURATION) -D_SOLUTION_AS_LIST_ $(REC_SRC_DIR)/threaded$*.cpp $(COMMON_SOL_SRC) -o $@

$(SANITY_IN): $(GENERATOR)
	@mkdir -p $(TEST_IN_DIR)
	python3 $(GENERATOR) $(SEED) $@ $(SANITY_CONFIG)

$(STRESS_IN): $(GENERATOR)
	@mkdir -p $(TEST_IN_DIR)
	python3 $(GENERATOR) $(SEED) $@ $(STRESS_CONFIG)

$(SANITY_OK): $(SANITY_IN) $(OK_EXE)
	@mkdir -p $(TEST_OK_DIR)
	$(OK_EXE) < $(SANITY_IN) > $@

$(STRESS_OK): $(STRESS_IN) $(OK_EXE)
	@mkdir -p $(TEST_OK_DIR)
	$(OK_EXE) < $(STRESS_IN) > $@
