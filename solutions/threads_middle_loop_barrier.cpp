#include <problem.hpp>

#include <algorithm>
#include <thread>
#include <utility>
#include <vector>

#include <iostream>

void solve(
    std::vector<Num> const& cuts,
    Grid<Num>& costs,
    Grid<Usize>& indices,
    Usize const num_cpus
) {
    auto const n = cuts.size();
    auto const rows_per_thread = (n + num_cpus - 1) / num_cpus;
    ReusableBarrier barrier(num_cpus);
    std::vector<std::thread> workers;
    workers.reserve(num_cpus);
    for(Usize t = 0; t < num_cpus; t++) {
        auto const start = t * rows_per_thread;
        auto const stop = std::min(start + rows_per_thread, n);
        workers.emplace_back([n, start, stop, &cuts, &costs, &indices, &barrier]() -> void {
            for(Usize length = 2; length < n; length++) {
                for(Usize left = start; (left < stop) & (left + length < n); left++) {
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
                barrier();
            }
        });
    }
    for(auto& worker : workers)
        worker.join();
}
