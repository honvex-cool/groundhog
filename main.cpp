#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#ifdef _SOLUTION_AS_LIST_
#include <list>
#endif

#include <cstdlib>

#include <problem.hpp>

namespace {
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

    void process_testcase(Usize const num_cpus) {
        auto const cuts = read_testcase(std::cin);
        auto const n = cuts.size();
        Grid<Num> costs(n, n, 0);
        Grid<Usize> indices(n, n, 0);
        solve(cuts, costs, indices, num_cpus);
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
        print_answer(std::cout, costs(0, n - 1), answer);
    }

    void run(Usize const num_cpus) {
        Usize num_testcases;
        std::cin >> num_testcases;
        for(Usize _ = 0; _ < num_testcases; _++)
            process_testcase(num_cpus);
    }
}

int main(int const argc, char const** const argv) {
    setup();
    Usize num_cpus = 0;
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
