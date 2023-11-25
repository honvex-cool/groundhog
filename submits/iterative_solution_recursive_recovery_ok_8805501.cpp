#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#ifdef _SOLUTION_AS_LIST_
#include <list>
#endif

#include <cstdlib>

#ifndef _PROBLEM_HPP_
#define _PROBLEM_HPP_

#include <limits>
#include <utility>
#include <vector>

#ifdef _SOLUTION_AS_LIST_
#include <list>
#endif

using Usize = unsigned int;
Usize constexpr NO_INDEX = std::numeric_limits<Usize>::max();

template<typename T>
class Grid {
public:
    explicit Grid(Usize const num_rows, Usize const num_columns, T const& initializer)
        : num_columns(num_columns)
        , data(num_rows * this->num_columns, initializer) {
    }

    T& operator()(Usize const row, Usize const column) {
        return data[flatten_coords(row, column)];
    }

    T const& operator()(Usize const row, Usize const column) const {
        return data[flatten_coords(row, column)];
    }

private:
    Usize flatten_coords(Usize const row, Usize const column) const {
        return row * num_columns + column;
    }

private:
    Usize num_columns;
    std::vector<T> data;
};

using Num = long long;
Num constexpr INF = std::numeric_limits<Num>::max();

void solve(
    std::vector<Num> const& cuts,
    Grid<Num>& costs,
    Grid<Usize>& indices,
    Usize const num_cpus
);

#ifdef _SOLUTION_AS_LIST_
void recover(
    Grid<Usize> const& indices,
    Usize const n,
    std::list<Usize>& ordered,
    Usize const num_cpus
);
#else
void recover(
    Grid<Usize> const& indices,
    Usize const n,
    std::vector<Usize>& ordered,
    Usize const num_cpus
);
#endif

#endif


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

#include <algorithm>
#include <utility>
#include <vector>

void solve(
    std::vector<Num> const& cuts,
    Grid<Num>& costs,
    Grid<Usize>& indices,
    Usize const num_cpus
) {
    (void)num_cpus;
    auto const n = cuts.size();
    for(Usize length = 2; length < n; length++) {
        for(Usize left = 0; left + length < n; left++) {
            auto const right = left + length;
            auto const left_cut = cuts[left];
            auto const right_cut = cuts[right];
            Num best = INF;
            Usize index = NO_INDEX;
            for(Usize internal = left + 1; internal < right; internal++) {
                auto op = std::max(cuts[internal] - left_cut, right_cut - cuts[internal]);
                auto const value = costs(left, internal) + costs(internal, right) + op;
                if(value < best) {
                    best = value;
                    index = internal;
                }
            }
            costs(left, right) = best;
            indices(left, right) = index;
        }
    }
}

namespace {
    void recover_range(
        Grid<Usize> const& indices,
        Usize const left,
        Usize const right,
        std::vector<Usize>& ordered
    ) {
        if(right - left <= 1)
            return;
        auto const internal = indices(left, right);
        ordered.emplace_back(internal);
        recover_range(indices, left, internal, ordered);
        recover_range(indices, internal, right, ordered);
    }
}

void recover(
    Grid<Usize> const& indices,
    Usize const n,
    std::vector<Usize>& ordered,
    Usize const num_cpus
) {
    (void)num_cpus;
    recover_range(indices, 0, n - 1, ordered);
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
