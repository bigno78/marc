#pragma once

#include "types.hpp"
#include "utils.hpp"

#include <vector>


struct Grid {

    Grid(const Header& header, size_t max_grid_rows, size_t max_grid_cols)
        : block_size_( get_block_size(header.rows, header.cols, max_grid_rows, max_grid_cols) ),
          grid_rows_( div_ceil(header.rows, block_size_) ),
          grid_cols_( div_ceil(header.cols, block_size_) ),
          data_( grid_rows_*grid_cols_, 0 ),
          matrix_symmetry_(header.symmetry) { }

    void on_entry(size_t row, size_t col) {
        add_entry(row, col);
        if (matrix_symmetry_ != Symmetry::general && row != col) {
            add_entry(col, row);
        }
    }

    size_t count_at(size_t row, size_t col) const {
        return at(row, col);
    }

    size_t block_capacity() const {
        return block_size_*block_size_;
    }

    size_t block_size() const {
        return block_size_;
    }

    size_t rows() const {
        return grid_rows_;
    }

    size_t cols() const {
        return grid_cols_;
    }

    size_t entries() const {
        return entries_count_;
    }

private:

    const size_t& at(size_t row, size_t col) const {
        return data_[row*grid_cols_ + col];
    }

    size_t& at(size_t row, size_t col) {
        return const_cast<size_t&>(static_cast<const Grid&>(*this).at(row, col));
    }

    void add_entry(size_t row, size_t col) {
        at(row/block_size_, col/block_size_)++;
        entries_count_++;
    }

    size_t get_block_size(size_t matrix_rows,
                          size_t matrix_cols,
                          size_t max_grid_rows,
                          size_t max_grid_cols) const 
    {
        size_t block_height = get_bin_size(matrix_rows, max_grid_rows);
        size_t block_width = get_bin_size(matrix_cols, max_grid_cols);

        return block_height >= block_width ? block_height : block_width;
    }

    size_t get_bin_size(size_t num_values, size_t max_bins) const {
        return num_values <= max_bins ? 1 : div_ceil(num_values, max_bins);
    }

private:
    size_t block_size_;

    size_t grid_rows_;
    size_t grid_cols_;

    std::vector<size_t> data_;

    size_t entries_count_ = 0;

    Symmetry matrix_symmetry_;
};
