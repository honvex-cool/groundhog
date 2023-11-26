#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#ifdef _SOLUTION_AS_LIST_
#include <list>
#endif

#include <cstdlib>

#include <problem.hpp>

namespace {
    using Duration = std::chrono::nanoseconds;

    void setup() {
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);
        std::cout.tie(nullptr);
    }

    void print_answer(std::ostream& out, Num const cost, std::vector<Num> const& cuts) {
        out << cost << "\n";
        for(auto const cut : cuts)
            out << cut << " ";
        out << "\n";
    }

    std::vector<Num> read_testcase(std::istream& in) {
        Num plank_size;
        in >> plank_size;
        Usize num_cuts;
        in >> num_cuts;
        std::vector<Num> cuts(num_cuts);
        for(auto& cut : cuts)
            in >> cut;
        cuts.emplace_back(0);
        cuts.emplace_back(plank_size);
        std::sort(cuts.begin(), cuts.end());
        return cuts;
    }

    std::pair<Duration, Duration> process_testcase(Usize const num_cpus) {
        auto const cuts = read_testcase(std::cin);
        auto const n = cuts.size();
        Grid<Num> costs(n, n, 0);
        Grid<Usize> indices(n, n, 0);
        auto const solution_start = std::chrono::steady_clock::now();
        solve(cuts, costs, indices, num_cpus);
        auto const solution_time = std::chrono::steady_clock::now() - solution_start;
        auto const recovery_start = std::chrono::steady_clock::now();
#ifdef _SOLUTION_AS_LIST_
        std::list<Usize> ordered;
#else
        std::vector<Usize> ordered;
        ordered.reserve(n);
#endif
        recover(indices, n, ordered, num_cpus);
        std::vector<Num> answer;
        answer.reserve(n);
        for(auto const index : ordered)
            answer.emplace_back(cuts[index]);
        auto const recovery_time = std::chrono::steady_clock::now() - recovery_start;
        print_answer(std::cout, costs(0, n - 1), answer);
        return { solution_time, recovery_time };
    }

    void run(Usize const num_cpus) {
        Usize num_testcases;
        std::cin >> num_testcases;
        Duration solution_min = Duration::max();
        Duration solution_max = Duration::min();
        Duration solution_total = Duration::zero();
        Duration recovery_min = Duration::max();
        Duration recovery_max = Duration::min();
        Duration recovery_total = Duration::zero();
        for(Usize _ = 0; _ < num_testcases; _++) {
            auto const [solution, recovery] = process_testcase(num_cpus);
            solution_min = std::min(solution_min, solution);
            solution_max = std::max(solution_max, solution);
            solution_total += solution;
            recovery_min = std::min(recovery_min, recovery);
            recovery_max = std::max(recovery_max, recovery);
            recovery_total += recovery;
        }
        std::cerr << "n_tests " << num_testcases << "\n";
        std::cerr << "sol_min " << solution_min.count() << " ns\n";
        std::cerr << "sol_max " << solution_max.count() << " ns\n";
        std::cerr << "sol_tot " << solution_total.count() << " ns\n";
        std::cerr << "rec_min " << recovery_min.count() << " ns\n";
        std::cerr << "rec_max " << recovery_max.count() << " ns\n";
        std::cerr << "rec_tot " << recovery_total.count() << " ns\n";
    }
}

int main(int const argc, char const** const argv) {
    setup();
    Usize num_cpus = 1;
    if(argc == 2) {
        try {
            num_cpus = static_cast<Usize>(std::stoi(argv[1]));
        } catch(...) {
            return EXIT_FAILURE;
        }
    }
    run(num_cpus);
    return EXIT_SUCCESS;
}
