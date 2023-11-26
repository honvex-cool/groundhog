#include <problem.hpp>

#include <list>
#include <stack>
#include <thread>

namespace {
    using Subproblem = std::pair<Usize, Usize>;

    void recover_sequential(
        Grid<Usize> const& indices,
        Usize const left,
        Usize const right,
        std::list<Usize>& ordered
    ) {
        std::stack<Subproblem> to_recover;
        to_recover.emplace(left, right);
        while(not to_recover.empty()) {
            auto [left, right] = to_recover.top();
            to_recover.pop();
            while(right - left > 1) {
                auto const internal = indices(left, right);
                ordered.emplace_back(internal);
                to_recover.emplace(internal, right);
                right = internal;
            }
        }
    }

    void recover_threaded(
        Grid<Usize> const& indices,
        Usize const left,
        Usize const right,
        std::list<Usize>& ordered,
        Usize const num_cpus
    ) {
        if(right - left <= 1)
            return;
        auto const internal = indices(left, right);
        ordered.emplace_back(internal);
        std::list<Usize> rest;
        if(num_cpus == 1) {
            std::thread worker(
                [&indices, left, internal, &ordered]() -> void {
                    recover_sequential(indices, left, internal, ordered);
                }
            );
            recover_sequential(indices, internal, right, rest);
            worker.join();
        } else {
            auto const left_cpus = num_cpus / 2;
            std::thread left_worker(
                [&indices, left, internal, &ordered, left_cpus]() -> void {
                    recover_threaded(indices, left, internal, ordered, left_cpus);
                }
            );
            auto const right_cpus = num_cpus - left_cpus;
            std::thread right_worker(
                [&indices, internal, right, &rest, num_cpus, right_cpus]() -> void {
                    recover_threaded(indices, internal, right, rest, right_cpus);
                }
            );
            left_worker.join();
            right_worker.join();
        }
        ordered.splice(ordered.end(), rest);
    }
}

void recover(
    Grid<Usize> const& indices,
    Usize const n,
    std::list<Usize>& ordered,
    Usize const num_cpus
) {
    recover_threaded(indices, 0, n - 1, ordered, num_cpus);
}
