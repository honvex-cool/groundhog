#include <problem.hpp>

#include <stack>
#include <utility>

using Subproblem = std::pair<Usize, Usize>;

void recover(
    Grid<Usize> const& indices,
    Usize const n,
    std::vector<Usize>& ordered,
    Usize const num_cpus
) {
    (void)num_cpus;
    std::stack<Subproblem> to_recover;
    to_recover.emplace(0, n - 1);
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
