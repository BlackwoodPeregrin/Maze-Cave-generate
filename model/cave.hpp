#ifndef SRC_CAVE_HPP_
#define SRC_CAVE_HPP_

#include <string>

namespace s21 {

constexpr unsigned kMaxLimits = 7;
constexpr unsigned kMaxFillCoef = 100;
constexpr unsigned kMaxCaveSize = 50;

class BoolMatrix {
  unsigned rows_{}, cols_{};
  bool** matrix_{nullptr};

 public:
  BoolMatrix() {}
  BoolMatrix(unsigned rows, unsigned cols);
  ~BoolMatrix();

  void set_size(unsigned rows, unsigned cols);
  unsigned get_rows() { return rows_; }
  unsigned get_cols() { return cols_; }
  void Swap(BoolMatrix& other);

  unsigned CountNeighbours(int x, int y);

  bool& operator()(unsigned x, unsigned y);

  void Save(const std::string& filename);
  void Load(const std::string& filename);

 private:
  void AllocateMemory();
  void Clear();
};

class Cave {
  unsigned death_limit_{4}, birth_limit_{4};
  int fill_coef_{50};
  BoolMatrix map_{};
  unsigned random_seed_;

 public:
  Cave() : random_seed_(time(0)) {}
  Cave(unsigned rows, unsigned cols);
  ~Cave() {}

  void set_death_limit(unsigned a);
  void set_birth_limit(unsigned a);
  void set_fill_coef(unsigned a);
  void set_size(unsigned rows, unsigned cols) { map_.set_size(rows, cols); }
  unsigned get_rows() { return map_.get_rows(); }
  unsigned get_cols() { return map_.get_cols(); }

  void FillRandomly();
  void Iterate();

  bool& operator()(unsigned x, unsigned y) { return map_(x, y); }

  void Save(const std::string& filename) { return map_.Save(filename); }
  void Load(const std::string& filename) { return map_.Load(filename); }

 private:
  int Roll(int base);
};

}  // namespace s21

#endif  // SRC_CAVE_HPP_
