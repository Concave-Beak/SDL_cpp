#include "../../../include/game/items/Inventory.hh"

#include <memory>

namespace Items {
Inventory::Inventory(size_t rows, size_t cols) : data(rows, std::vector<std::shared_ptr<Item>>(cols)) {}

std::shared_ptr<Item>& Inventory::At(size_t row, size_t col) {
    return data[row][col];
}

const std::shared_ptr<Item>& Inventory::At(size_t row, size_t col) const {
    return data[row][col];
}

void Inventory::Set(size_t row, size_t col, std::shared_ptr<Item> value) {
    data[row][col] = value;
}

size_t Inventory::Rows() const { return data.size(); }
size_t Inventory::Cols() const { return data[0].size(); }
}  // namespace Items
