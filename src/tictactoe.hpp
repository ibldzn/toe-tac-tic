#pragma once

#include <array>

struct TicTacToe {
    TicTacToe();
    void play();

private:
    enum class State;

    void print_board() const;
    constexpr bool is_valid_move(std::size_t row, std::size_t col) const;
    constexpr State get_state() const;
    constexpr bool any_space_left() const;
    constexpr void make_move(std::size_t row, std::size_t col);

    // minmax
    constexpr int evaluate_board() const;
    constexpr int minimax(bool maximizing);
    constexpr void get_best_move(bool x_turn, std::size_t& row, std::size_t& col);

private:
    enum class State {
        PLAYING,
        X_WINS,
        O_WINS,
        DRAW,
    };

    bool m_x_turn = true;
    std::array<std::array<char, 3>, 3> m_board = {};
    static constexpr char EMPTY_CELL = '-';
    static constexpr char X_CHAR = 'X';
    static constexpr char O_CHAR = 'O';
};
