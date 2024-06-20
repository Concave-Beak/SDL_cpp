#pragma once

#include <cstddef>
#include <memory>

#include "../../../lib/utils/math_utils.hh"
#include "./Item.hh"

class Inventory {
   public:
    Inventory(size_t rows, size_t cols);
    ~Inventory() = default;

    std::shared_ptr<Item>& At(size_t row, size_t col);

    const std::shared_ptr<Item>& At(size_t row, size_t col) const;

    void Set(size_t row, size_t col, std::shared_ptr<Item> value);

    size_t Rows() const;
    size_t Cols() const;

   private:
    Matrix2D<std::shared_ptr<Item>> data;
};
