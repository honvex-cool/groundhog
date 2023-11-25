#include <problem.hpp>

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
