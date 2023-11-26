#include <problem.hpp>

#include <algorithm>
#include <utility>
#include <vector>

#include <omp.h>

void solve(
    std::vector<Num> const& cuts,
    Grid<Num>& costs,
    Grid<Usize>& indices,
    Usize const num_cpus
) {
    auto const n = cuts.size();
    omp_set_dynamic(0);
    Usize length;
    #pragma omp parallel num_threads(num_cpus) private(length)
    {
        for(length = 2; length < n; length++) {
            #pragma omp for
            for(Usize left = 0; left < n - length; left++) {
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
            #pragma omp barrier
        }
    }
}
