#include "tictactoe.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>

// clang-format off
template <typename T, typename ValidateFn>
    requires requires(T v, ValidateFn fn) {
        { fn(v) } -> std::same_as<bool>;
        { std::cin >> v };
    }
// clang-format on
static T tanya_input(const char* prompt, ValidateFn fn)
{
    T out = {};

    while (true) {
        std::cout << prompt;
        std::cin >> out;

        if (!std::cin) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (fn(out)) {
            break;
        }
    }

    return out;
}

template <typename T>
static T tanya_input(const char* prompt)
{
    using type = typename std::conditional<std::is_trivially_copyable<T>::value, T, const T&>::type;
    return tanya_input(prompt, []([[maybe_unused]] type val) { return true; });
}

TicTacToe::TicTacToe()
{
    std::for_each(m_board.begin(), m_board.end(), [](decltype(m_board[0])& cell) {
        std::fill(cell.begin(), cell.end(), TicTacToe::EMPTY_CELL);
    });
}

void TicTacToe::print_board() const
{
    // +---+---+---+
    // | X | O | X |
    // +---+---+---+
    // | X | X | X |
    // +---+---+---+
    // | O | O | O |
    // +---+---+---+

    constexpr const auto BORDER = "+---+---+---+";

    std::cout << BORDER << '\n';

    for (std::size_t row = 0, row_size = m_board.size(); row < row_size; ++row) {
        for (std::size_t col = 0, col_size = m_board[row].size(); col < col_size; ++col) {
            std::cout << "| " << m_board[row][col] << (col + 1 == col_size ? " |\n" : " ");
        }

        std::cout << BORDER << '\n';
    }
}

void TicTacToe::play()
{
    constexpr auto clear_screen = [] {
#if defined(_WIN32) || defined(_WIN64)
        std::system("cls");
#else
        // Assume POSIX
        std::system("clear");
#endif
    };

    State state = get_state();

    while (state == State::PLAYING) {
        clear_screen();

        print_board();

        std::cout << (m_x_turn ? TicTacToe::X_CHAR : TicTacToe::O_CHAR) << "'s turn\n";

        std::size_t row = static_cast<std::size_t>(-1);
        std::size_t col = static_cast<std::size_t>(-1);

        if (m_x_turn) {
            get_best_move(m_x_turn, row, col);
        } else {
            do {
                row = tanya_input<decltype(row)>("row (1 - 3): ", [](const auto val) {
                    return val >= 1 && val <= 3;
                });

                col = tanya_input<decltype(col)>("col (1 - 3): ", [](const auto val) {
                    return val >= 1 && val <= 3;
                });
            } while (!is_valid_move(--row, --col));
        }

        make_move(row, col);

        state = get_state();
    }

    clear_screen();

    print_board();

    if (state != State::DRAW) {
        std::cout << (state == State::X_WINS ? TicTacToe::X_CHAR : TicTacToe::O_CHAR)
                  << " won!\n";
    } else {
        std::cout << "Draw!\n";
    }
}

constexpr bool TicTacToe::is_valid_move(std::size_t row, std::size_t col) const
{
    return row < m_board.size() && col < m_board[row].size() && m_board[row][col] == TicTacToe::EMPTY_CELL;
}

constexpr TicTacToe::State TicTacToe::get_state() const
{
    for (std::size_t row = 0, row_size = m_board.size(); row < row_size; ++row) {
        if (
            m_board[row][0] != TicTacToe::EMPTY_CELL
            && m_board[row][0] == m_board[row][1]
            && m_board[row][1] == m_board[row][2]
        ) {
            return m_board[row][0] == TicTacToe::X_CHAR ? State::X_WINS : State::O_WINS;
        }
    }

    for (std::size_t col = 0, col_size = m_board[0].size(); col < col_size; ++col) {
        if (
            m_board[0][col] != TicTacToe::EMPTY_CELL
            && m_board[0][col] == m_board[1][col]
            && m_board[1][col] == m_board[2][col]
        ) {
            return m_board[0][col] == TicTacToe::X_CHAR ? State::X_WINS : State::O_WINS;
        }
    }

    if (
        m_board[0][0] != TicTacToe::EMPTY_CELL
        && m_board[0][0] == m_board[1][1]
        && m_board[1][1] == m_board[2][2]
    ) {
        return m_board[0][0] == X_CHAR ? State::X_WINS : State::O_WINS;
    }

    if (
        m_board[0][2] != TicTacToe::EMPTY_CELL
        && m_board[0][2] == m_board[1][1]
        && m_board[1][1] == m_board[2][0]
    ) {
        return m_board[0][2] == X_CHAR ? State::X_WINS : State::O_WINS;
    }

    if (!any_space_left()) {
        return State::DRAW;
    }

    return State::PLAYING;
}

constexpr bool TicTacToe::any_space_left() const
{
    for (std::size_t row = 0, row_size = m_board.size(); row < row_size; ++row) {
        for (std::size_t col = 0, col_size = m_board[row].size(); col < col_size; ++col) {
            if (m_board[row][col] == TicTacToe::EMPTY_CELL) {
                return true;
            }
        }
    }
    return false;
}

constexpr void TicTacToe::make_move(std::size_t row, std::size_t col)
{
    m_board[row][col] = m_x_turn ? TicTacToe::X_CHAR : TicTacToe::O_CHAR;
    m_x_turn = !m_x_turn;
}

constexpr int TicTacToe::evaluate_board() const
{
    switch (get_state()) {
        case State::X_WINS: {
            return 1;
        }
        case State::O_WINS: {
            return -1;
        }
        default: {
            return 0;
        }
    }
}

constexpr int TicTacToe::minimax(bool maximizing)
{
    if (get_state() != State::PLAYING) {
        return evaluate_board();
    }

    auto best_score = maximizing ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    for (std::size_t row = 0, row_size = m_board.size(); row < row_size; ++row) {
        for (std::size_t col = 0, col_size = m_board[row].size(); col < col_size; ++col) {
            if (m_board[row][col] == TicTacToe::EMPTY_CELL) {
                m_board[row][col] = maximizing ? TicTacToe::X_CHAR : TicTacToe::O_CHAR;
                const auto score = minimax(!maximizing);
                m_board[row][col] = TicTacToe::EMPTY_CELL;
                best_score = maximizing ? std::max(best_score, score) : std::min(best_score, score);
            }
        }
    }

    return best_score;
}

constexpr void TicTacToe::get_best_move(bool x_turn, std::size_t& row, std::size_t& col)
{
    auto best_score = x_turn ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    for (std::size_t _row = 0, row_size = m_board.size(); _row < row_size; ++_row) {
        for (std::size_t _col = 0, col_size = m_board[_row].size(); _col < col_size; ++_col) {
            if (m_board[_row][_col] == TicTacToe::EMPTY_CELL) {
                m_board[_row][_col] = x_turn ? TicTacToe::X_CHAR : TicTacToe::O_CHAR;
                const auto score = minimax(!x_turn);
                m_board[_row][_col] = TicTacToe::EMPTY_CELL;

                const auto prev_best_score = best_score;
                best_score = x_turn ? std::max(best_score, score) : std::min(best_score, score);

                // best score changed
                if (prev_best_score != best_score) {
                    row = _row;
                    col = _col;
                }
            }
        }
    }
}
