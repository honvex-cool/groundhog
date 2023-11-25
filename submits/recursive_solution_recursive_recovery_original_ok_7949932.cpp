#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <vector>

#include <cstddef>

using namespace std;

template<typename T>
class Matrix {
public:
    explicit Matrix(size_t const row_count, size_t const column_count, T const initializer = T {})
        : row_count { row_count }
        , column_count { column_count }
        , data(this->row_count * this->column_count, initializer) {
    }

    size_t get_row_count() const {
        return this->row_count;
    }

    size_t get_column_count() const {
        return this->column_count;
    }

    T& operator()(size_t const row, size_t const column) {
        return this->data[this->flatten_coords(row, column)];
    }

    T const& operator()(size_t const row, size_t const column) const {
        return this->data[this->flatten_coords(row, column)];
    }

private:
    size_t flatten_coords(size_t const row, size_t const column) const {
        return row * this->column_count + column;
    }

private:
    size_t row_count;
    size_t column_count;
    vector<T> data;
};

template<typename T>
ostream& operator<<(ostream& out, vector<T> const& elements) {
    for(auto const& element : elements)
        out << element << " ";
    return out;
}

template<typename T>
vector<T> read_vector(istream& in, size_t const element_count) {
    vector<T> elements(element_count);
    for(auto& element : elements)
        in >> element;
    return elements;
}

template<typename T>
vector<T> input_vector(istream& in) {
    size_t element_count;
    in >> element_count;
    return read_vector<T>(in, element_count);
}

template<typename T>
T constexpr INFINITY { numeric_limits<T>::max() };

using Number = unsigned long long;

struct Cutting {
    Number cost;
    size_t cut_index;

    explicit Cutting(Number const cost, size_t const cut_index)
        : cost { cost }
        , cut_index { cut_index } {
    }
};

size_t constexpr NO_CUT { INFINITY<size_t> };

using Plan = optional<Cutting>;

void solve_dataset();
pair<Number, vector<Number>> optimal_cutting(vector<Number> const& cuts);
vector<Number> recover_cut_ordering(vector<Number> const& cuts, Matrix<Plan> const& plans);
void push_cuts_in_order(
    vector<Number> const& cuts,
    size_t const begin,
    size_t const end,
    Matrix<Plan> const& plans,
    vector<Number>& ordering
);
void compute_all_plans(vector<Number> const& cuts, Matrix<Plan>& plans);
Cutting plan_cutting(
    vector<Number> const& cuts,
    size_t const begin,
    size_t const end,
    Matrix<Plan>& plans
);
Cutting compute_cutting(
    vector<Number> const& cuts,
    size_t const begin,
    size_t const end,
    Matrix<Plan>& plan
);
vector<Number> read_cuts_left_to_right(istream& in);
vector<Number> read_cuts(istream& in);

int main() {
    ios::sync_with_stdio(false);
    size_t dataset_count;
    cin >> dataset_count;
    for(size_t _ { 0u }; _ < dataset_count; ++_)
        solve_dataset();
}

void solve_dataset() {
    auto const cuts { read_cuts_left_to_right(cin) };
    auto const [min_cost, optimal_cut_ordering] { optimal_cutting(cuts) };
    cout << min_cost << "\n" << optimal_cut_ordering << "\n";
}

pair<Number, vector<Number>> optimal_cutting(vector<Number> const& cuts) {
    auto const size = cuts.size();
    Matrix<Plan> cutting_plans { size, size, nullopt };
    compute_all_plans(cuts, cutting_plans);
    return { cutting_plans(0u, cuts.size() - 1u)->cost, recover_cut_ordering(cuts, cutting_plans) };
}

vector<Number> recover_cut_ordering(vector<Number> const& cuts, Matrix<Plan> const& plans) {
    auto const size { cuts.size() };
    vector<Number> ordering;
    ordering.reserve(size);
    push_cuts_in_order(cuts, 0u, size - 1u, plans, ordering);
    return ordering;
}

void push_cuts_in_order(
    vector<Number> const& cuts,
    size_t const begin,
    size_t const end,
    Matrix<Plan> const& plans,
    vector<Number>& ordering
) {
    if(end - begin <= 1u)
        return;
    auto const split_index { plans(begin, end)->cut_index };
    ordering.push_back(cuts[split_index]);
    push_cuts_in_order(cuts, begin, split_index, plans, ordering);
    push_cuts_in_order(cuts, split_index, end, plans, ordering);
}

void compute_all_plans(vector<Number> const& cuts, Matrix<Plan>& plans) {
    plan_cutting(cuts, 0u, cuts.size() - 1u, plans);
}

Cutting plan_cutting(
    vector<Number> const& cuts,
    size_t const begin,
    size_t const end,
    Matrix<Plan>& plans
) {
    if(plans(begin, end))
        return plans(begin, end).value();
    return (plans(begin, end) = { compute_cutting(cuts, begin, end, plans) }).value();
}

Cutting compute_cutting(
    vector<Number> const& cuts,
    size_t const begin,
    size_t const end,
    Matrix<Plan>& plan
) {
    if(end - begin <= 1u)
        return Cutting { 0u, NO_CUT };
    Cutting cutting { INFINITY<Number>, NO_CUT };
    for(size_t split_index { begin + 1u }; split_index < end; ++split_index) {
        auto const cost {
            plan_cutting(cuts, begin, split_index, plan).cost
            + plan_cutting(cuts, split_index, end, plan).cost
            + max(cuts[split_index] - cuts[begin], cuts[end] - cuts[split_index])
        };
        if(cost < cutting.cost)
            cutting = Cutting { cost, split_index };
    }
    return cutting;
}

vector<Number> read_cuts_left_to_right(istream& in) {
    auto cuts { read_cuts(in) };
    sort(cuts.begin(), cuts.end());
    return cuts;
}

vector<Number> read_cuts(istream& in) {
    size_t plank_length;
    in >> plank_length;
    auto cuts { input_vector<Number>(in) };
    cuts.push_back(0u);
    cuts.push_back(plank_length);
    return cuts;
}
