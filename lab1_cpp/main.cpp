#include <iostream>
#include <vector>
#include <chrono>


struct point
{
    int x;
    int y;
};

struct square
{
    point left_corner;
    int length;

    friend std::ostream& operator<<(std::ostream& os, const square& s)
    {
        os << s.left_corner.x + 1 << " " << s.left_corner.y + 1 << " " << s.length;
        return os;
    }

    square(int x, int y, int _length)
            : left_corner{x, y}, length(_length)
    {
    }
};

/// Make fragmentation for N divided by 2
/// \param n size of table
/// \return vector of square
std::vector<square> make_fragmentation_for_div_2(int n)
{
    std::vector<square> ret_squares;
    int half = int(n / 2);
    for (int i = 0; i < n; i += half)
    {
        for (int j = 0; j < n; j += half)
        {
            ret_squares.emplace_back(i, j, half);
        }
    }

    return ret_squares;
}

/// Make fragmentation for N divided by 3
/// \param n size of table
/// \return vector of square
std::vector<square> make_fragmentation_for_div_3(int n)
{
    std::vector<square> ret_squares;
    int small_inner_n = int(n / 3);
    int big_inner_n = small_inner_n * 2;
    ret_squares.emplace_back(0, 0, big_inner_n);

    // Bottom
    for (int i = 0; i < n; i += small_inner_n)
    {
        ret_squares.emplace_back(i, n - small_inner_n, small_inner_n);
    }

    // Side
    for (int i = 0; i < n - small_inner_n; i += small_inner_n)
    {
        ret_squares.emplace_back(n - small_inner_n, i, small_inner_n);
    }

    return ret_squares;
}

/// Make fragmentation for N divided by 5
/// \param n size of table
/// \return vector of square
std::vector<square> make_fragmentation_for_div_5(int n)
{
    std::vector<square> ret_squares;
    int half = int(n / 5 * 2);
    int third = int(n / 5 * 3);
    int fifth = int(n / 5);
    ret_squares.emplace_back(0, 0, third);

    // Big in corners

    //for a, b in [(0, 1), (1, 0), (1, 1)]:
    ret_squares.emplace_back(0, third, half);
    ret_squares.emplace_back(third, 0, half);
    ret_squares.emplace_back(third, third, half);

    // Small in gaps
    for (int i = 0; i < 2; ++i)
    {
        ret_squares.emplace_back(half, third + i * fifth, fifth);
        ret_squares.emplace_back(third + i * fifth, half, fifth);
    }

    return ret_squares;
}

/// Make base fragmentation for backtracking with 3 big square
/// \param n size of table
/// \return vector of square
std::vector<square> make_base_fragmentation(int n)
{
    int half = int(n / 2) + 1;
    return {square(0, 0, half),
            square(half, 0, half - 1),
            square(0, half, half - 1)};
}

/// Make backtracking iteration fragmentation
/// \param n size of table
/// \param center center of table
/// \param field field of defect_square
/// \param current_stack vector of current_stack
/// \best_stack_size size of stack with the best fragmentation
bool backtracking_iter(int n, int center, int** field, std::vector<square>& current_stack, int best_stack_size)
{
    const auto can_growth = [&](square& square)
    {
        // Check bottom
        if (square.left_corner.x + square.length + 1 <= n && square.left_corner.y + square.length + 1 <= n)
        {
            for (int i = square.left_corner.x; i < square.left_corner.x + square.length + 1; ++i)
            {
                if (field[i - center][square.left_corner.y + square.length - center] != 0)
                    return false;
            }
        }
        else
        {
            return false;
        }
        // Check side
        for (int i = square.left_corner.y; i < square.left_corner.y + square.length + 1; ++i)
        {
            if (field[square.left_corner.x + square.length - center][i - center] != 0)
                return false;
        }

        return true;
    };

    const auto growth_square = [&](square& square)
    {
        square.length += 1;
        for (int i = square.left_corner.x; i < square.left_corner.x + square.length; ++i)
        {
            for (int j = square.left_corner.y; j < square.left_corner.y + square.length; ++j)
            {
                field[i - center][j - center] = 1;
            }
        }
    };

    for (int i = center; i < n; ++i)
    {
        for (int j = center; j < n; ++j)
        {
            if (field[i - center][j - center] == 0)
            {
                auto tmp_square = square(i, j, 1);
                field[i - center][j - center] = 1;
                while (can_growth(tmp_square))
                {
                    growth_square(tmp_square);
                }

                current_stack.push_back(tmp_square);
                if (best_stack_size && current_stack.size() >= best_stack_size)
                    return false;
            }
        }
    }

    return true;
}

/// Find best fragmentation for specify diag
/// \param n size of table
/// \param center center of table
/// \param left_corner_shift left corner coord of diag
/// \return list of squares
std::vector<square> find_next_fragmentation_for_diag(int n, int center, int left_corner_shift)
{
    int** field = new int* [n - center + 1];
    for (int i = 0; i < n - center; ++i)
    {
        field[i] = new int[n - center + 1];
    }
    const auto squize_square = [&](square& square)
    {
        // Bottom
        for (int i = square.left_corner.x + square.length - 1; i > square.left_corner.x - 1; --i)
        {
            field[i - center][square.left_corner.y + square.length - 1 - center] = 0;
        }

        // Side
        for (int j = square.left_corner.y + square.length - 2; j > square.left_corner.y - 1; --j)
        {
            field[square.left_corner.x + square.length - 1 - center][j - center] = 0;
        }

        square.length -= 1;
    };

    for (int i = center; i < n; ++i)
    {
        for (int j = center; j < n; ++j)
        {
            bool is_empty =
                    (i != j || (i != center and j != center)) && (i < left_corner_shift || j < left_corner_shift);
            field[i - center][j - center] = is_empty ? 0 : 1;
        }
    }

    std::vector<square> ret_stack;
    std::vector<square> tmp_stack;

    while (!tmp_stack.empty() || ret_stack.empty())
    {
        if (ret_stack.empty() || tmp_stack.empty() || tmp_stack.size() < ret_stack.size())
        {
            bool is_better = backtracking_iter(n, center, static_cast<int**>(field), tmp_stack, ret_stack.size());
            if ((is_better && tmp_stack.size() < ret_stack.size()) || ret_stack.empty())
            {
                ret_stack = tmp_stack;
            }
        }

        while (!tmp_stack.empty() && tmp_stack.back().length == 1)
        {
            auto deleted = tmp_stack.back();
            field[deleted.left_corner.x - center][deleted.left_corner.y - center] = 0;
            tmp_stack.pop_back();
        }

        if (!tmp_stack.empty() && tmp_stack.back().length > 1)
        {
            squize_square(tmp_stack.back());
        }
    }

    return ret_stack;
}

/// Make backtracking fragmentation
/// \param n size of table
/// \return vector of square
std::vector<square> make_backtracking_fragmentation(int n)
{
    auto squares = make_base_fragmentation(n);
    std::vector<square> min_squares;
    int center = n / 2;
    for (int i = 2; i < center; ++i)
    {
        int left_corner_shit = n - i;
        std::vector<square> best_fragmentation_for_diag = {square(left_corner_shit, left_corner_shit, i)};

        auto backtack_fragmentation = find_next_fragmentation_for_diag(n, center, left_corner_shit);
        best_fragmentation_for_diag.insert(best_fragmentation_for_diag.begin() + 1, backtack_fragmentation.begin(),
                                           backtack_fragmentation.end());

        if (min_squares.empty() || best_fragmentation_for_diag.size() < min_squares.size())
        {
            min_squares = best_fragmentation_for_diag;
        }

    }

    squares.insert(squares.begin() + 3, min_squares.begin(), min_squares.end());
    return squares;
}

/// Print field state
/// \param n size of table
/// \param squares vector of squares
void print_debug_squares(int n, std::vector<square>& squares)
{
    int field[n][n];

    int square_index = 0;
    for (auto& square : squares)
    {
        for (int y = square.left_corner.y; y < square.left_corner.y + square.length; ++y)
        {
            for (int x = square.left_corner.x; x < square.left_corner.x + square.length; ++x)
            {
                field[x][y] = square_index;
            }
        }
        ++square_index;
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            std::cout << field[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

/// Print result and each square
/// \param n size of table
/// \param squares vector of squares
void print_squares(int n, std::vector<square>& squares)
{
    std::cout << squares.size() << std::endl;
    for (auto& square : squares)
    {
        std::cout << square << std::endl;
    }

#ifndef NDEBUG
    print_debug_squares(n, squares);
#endif
}


/// Main function for call
/// \return
int main()
{
    int n;
    std::cin >> n;
    auto fun = make_backtracking_fragmentation;
    if (n % 2 == 0)
        fun = make_fragmentation_for_div_2;
    else if (n % 3 == 0)
        fun = make_fragmentation_for_div_3;
    else if (n % 5 == 0)
        fun = make_fragmentation_for_div_5;
    else
        fun = make_backtracking_fragmentation;
    auto start = std::chrono::steady_clock::now();
    auto res = fun(n);
    auto end = std::chrono::steady_clock::now();

    print_squares(n, res);
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    return 0;
}

