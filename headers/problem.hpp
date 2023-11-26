#ifndef _PROBLEM_HPP_
#define _PROBLEM_HPP_

#include <condition_variable>
#include <limits>
#include <mutex>
#include <utility>
#include <vector>

#ifdef _SOLUTION_AS_LIST_
#include <list>
#endif

using Usize = unsigned int;
Usize constexpr NO_INDEX = std::numeric_limits<Usize>::max();

// adapted from https://github.com/kirksaunders/barrier/blob/master/barrier.hpp
class ReusableBarrier {
public:
    explicit ReusableBarrier(Usize const width)
        : width(width)
        , current_version(0)
        , remaining(width) {
    }

    void operator()() {
        std::unique_lock<std::mutex> lock(arrival);
        auto const waiting_version = current_version;
        if(--remaining > 0)
            changed.wait(
                lock,
                [this, waiting_version]() -> bool {
                    return current_version > waiting_version;
                }
            );
        else {
            remaining = width;
            current_version++;
            changed.notify_all();
        }
    }

private:
    std::condition_variable changed;
    Usize width;
    Usize current_version;
    Usize remaining;
    std::mutex arrival;
};

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
