#include "cave.hpp"

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace s21 {

BoolMatrix::BoolMatrix(unsigned rows, unsigned cols)
    : rows_(rows), cols_(cols) {
  if (rows_ > 0 && cols > 0) {
    AllocateMemory();
  }
}

BoolMatrix::~BoolMatrix() { Clear(); }

void BoolMatrix::set_size(unsigned rows, unsigned cols) {
  if (rows > kMaxCaveSize) rows = kMaxCaveSize;
  if (cols > kMaxCaveSize) cols = kMaxCaveSize;
  if (rows_ != rows || cols_ != cols) {
    Clear();
    rows_ = rows;
    cols_ = cols;
    AllocateMemory();
  }
}

void BoolMatrix::Swap(BoolMatrix& other) {
  if (this != &other) {
    std::swap(rows_, other.rows_);
    std::swap(cols_, other.cols_);
    std::swap(matrix_, other.matrix_);
  }
}

unsigned BoolMatrix::CountNeighbours(int x, int y) {
  unsigned res{};
  for (int i{x - 1}; i < x + 2; i++) {
    for (int j{y - 1}; j < y + 2; j++) {
      if (i < 0 || j < 0 || i >= (int)rows_ || j >= (int)cols_)
        res++;
      else if (i != x || j != y)
        if (matrix_[i][j]) res++;
    }
  }
  return res;
}

void BoolMatrix::AllocateMemory() {
  matrix_ = new bool*[rows_];
  matrix_[0] = new bool[rows_ * cols_];
  for (unsigned i{1}; i < rows_; i++) matrix_[i] = matrix_[0] + cols_ * i;
  for (unsigned i{}; i < rows_ * cols_; i++) matrix_[0][i] = false;
}

void BoolMatrix::Clear() {
  if (matrix_ != nullptr) {
    if (matrix_[0] != nullptr) {
      delete[] matrix_[0];
      matrix_[0] = nullptr;
    }
    delete[] matrix_;
    matrix_ = nullptr;
  }
}

bool& BoolMatrix::operator()(unsigned x, unsigned y) {
  if (x >= rows_ || y >= cols_) throw std::out_of_range("wrong index");

  return matrix_[x][y];
}

void BoolMatrix::Save(const std::string& filename) {
  std::ofstream stream(filename);
  if (stream.is_open()) {
    stream << get_rows() << " " << get_cols() << std::endl;
    for (unsigned i{}; i < get_rows(); i++) {
      for (unsigned j{}; j < get_cols(); j++) {
        stream << matrix_[i][j];
        if (j != get_cols() - 1) stream << " ";
      }
      stream << std::endl;
    }
    stream.close();
  }
}

void BoolMatrix::Load(const std::string& filename) {
  std::ifstream stream(filename);
  if (stream.is_open()) {
    std::string line{};
    size_t pos{};

    std::getline(stream, line);
    pos = line.find(' ');
    if (pos != std::string::npos) {
      set_size(stoi(line.substr(0, pos)), stoi(line.substr(pos + 1)));
      for (unsigned i{}; i < get_rows(); i++) {
        std::getline(stream, line);
        for (unsigned j{}; j < get_cols(); j++) {
          if (line[j * 2] == '0')
            matrix_[i][j] = false;
          else
            matrix_[i][j] = true;
          if (j > 0 && line[2 * j - 1] != ' ')
            throw std::invalid_argument("incorrect file");
        }
      }
    }
    stream.close();
  }
}

Cave::Cave(unsigned rows, unsigned cols)
    : death_limit_(3), birth_limit_(6), fill_coef_(50), map_(rows, cols), random_seed_(time(0)) {}

void Cave::set_death_limit(unsigned a) {
  if (a <= kMaxLimits) death_limit_ = a;
}

void Cave::set_birth_limit(unsigned a) {
  if (a <= kMaxLimits) birth_limit_ = a;
}

void Cave::set_fill_coef(unsigned a) {
  if (a <= kMaxFillCoef) fill_coef_ = a;
}

void Cave::FillRandomly() {
  for (unsigned i{}; i < map_.get_rows(); i++) {
    for (unsigned j{}; j < map_.get_cols(); j++) {
      if (Roll(100) < fill_coef_)
        map_(i, j) = true;
      else
        map_(i, j) = false;
    }
  }
}

void Cave::Iterate() {
  if (map_.get_rows() > 0 && map_.get_cols() > 0) {
    BoolMatrix res(map_.get_rows(), map_.get_cols());
    for (unsigned i{}; i < map_.get_rows(); i++) {
      for (unsigned j{}; j < map_.get_cols(); j++) {
        unsigned population = map_.CountNeighbours(i, j);
        if (map_(i, j) == true) {
          if (population < death_limit_)
            res(i, j) = false;
          else
            res(i, j) = true;
        } else {
          if (population > birth_limit_)
            res(i, j) = true;
          else
            res(i, j) = false;
        }
      }
    }
    map_.Swap(res);
  }
}

int Cave::Roll(int base) {
  int res{};
  if (base > 0) {
    res = rand_r(&random_seed_) % base;
  }
  return res;
}

}  // namespace s21
