#include <problem.hpp>

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
