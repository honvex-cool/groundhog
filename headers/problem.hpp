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
