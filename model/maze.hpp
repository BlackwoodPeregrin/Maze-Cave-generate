#include <stdlib.h>

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <stack>
#include <string>
#include <vector>

namespace s21 {

/*---------------Класс ячеки лабиринта-------------------*/

class Cell {
 public:
  Cell(const int &value = 0, const bool &rightBorder = false,
       const bool &downBorder = false);

  void SetDefaultParametrs(const int &value = 0,
                           const bool &rightBorder = false,
                           const bool &downBorder = false);

  bool operator==(const Cell &other) const;
  bool operator!=(const Cell &other) const;

  const int &GetValue() const;
  void SetValue(const int &value);

  const bool &HaveRightBorder() const;
  const bool &HaveDownBorder() const;

  void BuildRightBorder();
  void BuildDouwBorder();

  void DestroyRightBorder();
  void DestroyDownBorder();

 private:
  int value_;
  bool rightBorder_;
  bool downBorder_;
};

/*------------------------------------------------------*/

/*---------------Класс строки лабиринта-----------------*/

class RowCells {
 public:
  explicit RowCells(const size_t &sumCells);
  RowCells(const RowCells &other);
  void operator=(const RowCells &row);
  Cell &operator[](const size_t &index);

  void GenerateFirstRow(unsigned *seed);  // генерация первой страки
  void GenerateAnyRow(const RowCells &prevRow, unsigned *seed);  // генерация псоледующих строк
  void GenerateLastRow();  // генерация заключительнйо строки
  std::pair<bool, bool> CellWalls(const size_t &column);

 private:
  void BuildRightBorders(unsigned *seed);  // идем слева направо и строим правые границы
  void BuildDownBorders(unsigned *seed);  // идем слева направо и строим нижние границы
  void DestroyRightBorders();  // удаляем все правые краницы ячеек
  void DestroySetsWithDownBorders();  // удаляем нижние границы ячеек
  void MergeTwoSets(const Cell &resSet, const Cell changeSet);
  bool GenerateRandomBool(unsigned *seed);
  bool IsTheSameSets(const Cell &left, const Cell &right);
  size_t SearchMaxElement();  // поиск максимального значения ячейки в строке
  void createVectorSameElements(std::vector<size_t> *indexSameElement,
                                const int &value);
  bool isElementInSet(const std::set<int> &uniqueSet, int value);

 private:
  size_t sumCells_;
  std::vector<Cell> cell_;
};

/*------------------------------------------------------*/

/*-------------Основной класс лабиринта-----------------*/

class Maze {
 public:
  using coord = std::pair<size_t, size_t>;

  Maze();
  Maze(const size_t &rows, const size_t &columns);
  void InitMaze(const size_t &rows, const size_t &columns);
  bool IsMazeInit();
  ~Maze();

  void SaveMaze(const std::string &fileName);
  void LoadMaze(const std::string &fileName);

  void GenerationIdealMaze();
  std::vector<coord> SearchPath(const coord &initial_position,
                                const coord &end_position);
  std::vector<coord> SearchPath() { return SearchPath(start_, finish_); }

  std::pair<bool, bool> CellWalls(const size_t &row, const size_t &column);

  const size_t &get_rows();
  const size_t &get_columns();
  void set_start(const coord &val);
  void set_finish(const coord &val);
  const coord &get_start();
  const coord &get_finish();

  std::vector<bool> getMarixRightBorder();
  std::vector<bool> getMarixDownBorder();

 private:
  void SaveCoordsMaxStep(std::vector<coord> *maxStep, const int &valueMaxStep);
  bool IsFinishStep(const std::vector<coord> &cell, const coord &finishCoord);
  bool SearchPossibleStepNext(const coord &coordCell);
  std::vector<coord> GetPath(const coord &initial_position,
                             const coord &end_position);
  bool IsStartStep(const coord &searchCoord, const coord &currentCoord);
  void SearchPossibleStepPrev(coord *coordCurrentCell);

  /*---функции для загрузки лабиринта из файла---*/
  void installRowsColumnsMaze(const std::string &lineData);
  void installRightBordersRowMaze(const size_t &indexRow,
                                  const std::string &lineData);
  void installDownBordersRowMaze(const size_t &indexRow,
                                 const std::string &lineData);

  std::vector<RowCells> rowMaze_;
  size_t rows_;
  size_t columns_;
  unsigned random_seed_;
  coord start_{}, finish_{};
};

/*------------------------------------------------------*/
}  // namespace s21
