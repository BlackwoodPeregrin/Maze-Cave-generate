#include "maze.hpp"

namespace s21 {

/*---------------Класс ячеки лабиринта-------------------*/

Cell::Cell(const int &value, const bool &rightBorder, const bool &downBorder) {
  SetDefaultParametrs(value, rightBorder, downBorder);
}

void Cell::SetDefaultParametrs(const int &value, const bool &rightBorder,
                               const bool &downBorder) {
  value_ = value;
  rightBorder_ = rightBorder;
  downBorder_ = downBorder;
}

bool Cell::operator==(const Cell &other) const {
  return (value_ == other.value_);
}

bool Cell::operator!=(const Cell &other) const {
  return (value_ != other.value_);
}

const int &Cell::GetValue() const { return value_; }
void Cell::SetValue(const int &value) { value_ = value; }

const bool &Cell::HaveRightBorder() const { return rightBorder_; }
const bool &Cell::HaveDownBorder() const { return downBorder_; }

void Cell::BuildRightBorder() { rightBorder_ = true; }
void Cell::BuildDouwBorder() { downBorder_ = true; }

void Cell::DestroyRightBorder() { rightBorder_ = false; }
void Cell::DestroyDownBorder() { downBorder_ = false; }

/*------------------------------------------------------*/

/*---------------Класс строки лабиринта-----------------*/

RowCells::RowCells(const size_t &sumCells) : sumCells_(sumCells) {
  /*---инициализируем ячейки---*/
  for (size_t indexCell = 0; indexCell < sumCells_; ++indexCell) {
    cell_.push_back(Cell());
  }
}

RowCells::RowCells(const RowCells &other) { *this = other; }

void RowCells::operator=(const RowCells &row) {
  sumCells_ = row.sumCells_;
  cell_ = row.cell_;
}

Cell &RowCells::operator[](const size_t &index) {
  if (index >= cell_.size()) {
    throw std::out_of_range("index out of range row cells");
  }
  return cell_[index];
}

void RowCells::GenerateFirstRow(unsigned *seed) {
  /*---присваеваем каждой ячейки уникальное множество---*/
  for (size_t indexCell = 0; indexCell < sumCells_; ++indexCell) {
    cell_[indexCell].SetDefaultParametrs(indexCell + 1, false, false);
  }
  /*---строим правые границы ячеек---*/
  BuildRightBorders(seed);
  /*---строим нижние границы ячеек---*/
  BuildDownBorders(seed);
}

void RowCells::GenerateAnyRow(const RowCells &prevRow, unsigned *seed) {
  /*---выведем предидущую строку---*/
  *this = prevRow;
  /*---удаляем все правые границы---*/
  DestroyRightBorders();
  /*---Если ячейка имеет нижнюю границу, удаляем ее из множества---*/
  DestroySetsWithDownBorders();
  /*---строим правые границы ячеек---*/
  BuildRightBorders(seed);
  /*---строим нижние границы ячеек---*/
  BuildDownBorders(seed);
}

void RowCells::GenerateLastRow() {
  /*---елси ячейки из разных множеств имеют правую стенку разрушаем ее ---*/
  for (size_t indexCell = 0; indexCell < sumCells_ - 1; ++indexCell) {
    if (cell_[indexCell] != cell_[indexCell + 1]) {
      if (cell_[indexCell].HaveRightBorder()) {
        cell_[indexCell].DestroyRightBorder();
      }
      MergeTwoSets(cell_[indexCell], cell_[indexCell + 1]);
    }
    /*---строим нижние границы для ячеек---*/
    cell_[indexCell].BuildDouwBorder();
  }
  /*---строим нижнюю границу для последней ячейки---*/
  cell_.back().BuildDouwBorder();
}

std::pair<bool, bool> RowCells::CellWalls(const size_t &column) {
  return std::make_pair(cell_[column].HaveRightBorder(),
                        cell_[column].HaveDownBorder());
}

void RowCells::BuildRightBorders(unsigned *seed) {
  /*---строим правые границы ячеек---*/
  for (size_t indexCell = 0; indexCell < sumCells_ - 1; ++indexCell) {
    if (GenerateRandomBool(seed) == true ||
        IsTheSameSets(cell_[indexCell], cell_[indexCell + 1]) == true) {
      cell_[indexCell].BuildRightBorder();
    } else {
      MergeTwoSets(cell_[indexCell], cell_[indexCell + 1]);
    }
  }
  /*---Добавление правой стенки в последней ячейки---*/
  cell_.back().BuildRightBorder();
}

void RowCells::BuildDownBorders(unsigned *seed) {
  /*---строим нижние границы ячеек---*/
  std::set<int> uniqueElemnts;
  for (size_t indexCell = 0; indexCell < sumCells_; ++indexCell) {
    while (isElementInSet(uniqueElemnts, cell_[indexCell].GetValue())) {
      ++indexCell;
      if (indexCell >= sumCells_) {
        break;
      }
    }
    if (indexCell >= sumCells_) {
      break;
    }
    uniqueElemnts.insert(cell_[indexCell].GetValue());

    std::vector<size_t> indexSameElement;
    createVectorSameElements(&indexSameElement, cell_[indexCell].GetValue());

    if (indexSameElement.size() > 1) {
      size_t sumDouwnBorders = 0;
      for (const size_t &index : indexSameElement) {
        if (GenerateRandomBool(seed) == true) {
          cell_[index].BuildDouwBorder();
          ++sumDouwnBorders;
        }
      }
      if (sumDouwnBorders == indexSameElement.size()) {
        cell_[indexSameElement.back()].DestroyDownBorder();
      }
    }
  }
}

void RowCells::DestroyRightBorders() {
  for (size_t index = 0; index < sumCells_; ++index) {
    cell_[index].DestroyRightBorder();
  }
}

void RowCells::DestroySetsWithDownBorders() {
  for (size_t index = 0; index < sumCells_; ++index) {
    if (cell_[index].HaveDownBorder()) {
      cell_[index].DestroyDownBorder();
      cell_[index].SetValue(SearchMaxElement() + 1);
    }
  }
}

void RowCells::MergeTwoSets(const Cell &resSet, const Cell changeSet) {
  for (size_t indexCell = 0; indexCell < sumCells_; ++indexCell) {
    if (cell_[indexCell].GetValue() == changeSet.GetValue()) {
      cell_[indexCell].SetValue(resSet.GetValue());
    }
  }
}

bool RowCells::GenerateRandomBool(unsigned *seed) { return (rand() % 2); }

bool RowCells::IsTheSameSets(const Cell &left, const Cell &right) {
  return (left == right);
}

size_t RowCells::SearchMaxElement() {
  int max_element = cell_.front().GetValue();
  for (size_t i = 1; i < sumCells_; ++i) {
    max_element =
        max_element < cell_[i].GetValue() ? cell_[i].GetValue() : max_element;
  }
  return max_element;
}

void RowCells::createVectorSameElements(std::vector<size_t> *indexSameElement,
                                        const int &value) {
  size_t tmp = cell_.size();
  for (size_t index = 0; index < tmp; ++index) {
    if (cell_[index].GetValue() == value) {
      indexSameElement->push_back(index);
    }
  }
}

bool RowCells::isElementInSet(const std::set<int> &uniqueSet, int value) {
  if (uniqueSet.find(value) == uniqueSet.end()) {
    return false;
  }
  return true;
}

/*------------------------------------------------------*/

/*-------------Основной класс лабиринта-----------------*/

Maze::Maze() : rows_(0), columns_(0), random_seed_(time(0)) {}

Maze::Maze(const size_t &rows, const size_t &columns) : random_seed_(time(0)) {
  InitMaze(rows, columns);
}

void Maze::InitMaze(const size_t &rows, const size_t &columns) {
  rows_ = rows;
  columns_ = columns;
  if (!rowMaze_.empty()) {
    rowMaze_.clear();
  }
  for (size_t i = 0; i < rows_; ++i) {
    rowMaze_.push_back(RowCells(columns_));
  }
}

bool Maze::IsMazeInit() { return (rows_ != 0 && columns_ != 0); }

Maze::~Maze() {}

void Maze::SaveMaze(const std::string &fileName) {
  if (!IsMazeInit()) {
    throw std::out_of_range("Can't save not inizialize Maze");
  }
  std::ofstream stream(fileName);
  if (stream.is_open()) {
    /*---сохраняем размерность лабиринта---*/
    stream << rows_ << " " << columns_ << std::endl;

    /*---сохраняем матрицу правых границ---*/
    for (size_t indexRow = 0; indexRow < rows_; ++indexRow) {
      for (size_t indexColumn = 0; indexColumn < columns_ - 1; ++indexColumn) {
        stream << rowMaze_[indexRow][indexColumn].HaveRightBorder() << " ";
      }
      stream << rowMaze_[indexRow][columns_ - 1].HaveRightBorder() << std::endl;
    }

    /*---выводим разграничение между матрицами---*/
    stream << std::endl;

    /*---сохраняем матрицу нижних границ---*/
    for (size_t indexRow = 0; indexRow < rows_; ++indexRow) {
      for (size_t indexColumn = 0; indexColumn < columns_ - 1; ++indexColumn) {
        stream << rowMaze_[indexRow][indexColumn].HaveDownBorder() << " ";
      }
      if (indexRow == rows_ - 1) {
        stream << rowMaze_[indexRow][columns_ - 1].HaveDownBorder();
      } else {
        stream << rowMaze_[indexRow][columns_ - 1].HaveDownBorder()
               << std::endl;
      }
    }
    stream.close();
  }
}

void Maze::LoadMaze(const std::string &fileName) {
  std::ifstream stream(fileName);
  if (stream.is_open()) {
    std::string lineFile{};
    /*---считываем размерность лабиринта---*/
    std::getline(stream, lineFile);
    installRowsColumnsMaze(lineFile);

    /*---сохраняем матрицу правых границ---*/
    for (size_t indexRow = 0; indexRow < rows_; ++indexRow) {
      std::getline(stream, lineFile);
      installRightBordersRowMaze(indexRow, lineFile);
    }

    std::getline(stream, lineFile);  // cчитываем пустую строку

    if (!lineFile.empty()) {
      throw std::invalid_argument("Error, Incorrect LoadFile Maze");
    }

    /*---сохраняем матрицу нижних границ---*/
    for (size_t indexRow = 0; indexRow < rows_; ++indexRow) {
      std::getline(stream, lineFile);
      installDownBordersRowMaze(indexRow, lineFile);
    }

    if (!stream.eof()) {
      throw std::invalid_argument("Error, Incorrect LoadFile Maze");
    }
    stream.close();
  }
}

void Maze::GenerationIdealMaze() {
  if (!IsMazeInit()) {
    throw std::out_of_range("Can't generate not inizialize Maze");
  }
  /*---генерируем первую строку---*/
  rowMaze_.front().GenerateFirstRow(&random_seed_);
  /*---генерируем последующие строки---*/
  for (size_t indexRow = 1; indexRow < rows_; ++indexRow) {
    rowMaze_[indexRow].GenerateAnyRow(rowMaze_[indexRow - 1], &random_seed_);
  }
  /*---генерируем заключительную строку---*/
  rowMaze_.back().GenerateLastRow();
}

std::vector<Maze::coord> Maze::SearchPath(const coord &initial_position,
                                          const coord &end_position) {
  if (!IsMazeInit()) {
    throw std::out_of_range("Can't generate not inizialize Maze");
  }

  if (initial_position.first >= rows_ || end_position.first >= rows_ ||
      initial_position.second >= columns_ || end_position.second >= columns_) {
    throw std::out_of_range("index out of range maze");
  }

  int indexStep = 1;

  for (size_t i = 0; i < rows_; ++i) {
    for (size_t j = 0; j < columns_; ++j) {
      if (i == initial_position.first && j == initial_position.second) {
        rowMaze_[i][j].SetValue(indexStep);
      } else {
        rowMaze_[i][j].SetValue(0);
      }
    }
  }

  bool error = false;

  while (true) {
    std::vector<coord> maxStep;
    SaveCoordsMaxStep(&maxStep, indexStep);

    if (IsFinishStep(maxStep, end_position) == true || error == true) {
      break;
    } else {
      size_t tmp = maxStep.size();
      size_t sumCloseCells = 0;
      for (size_t index = 0; index < tmp; ++index) {
        if (SearchPossibleStepNext(maxStep[index]) == false) {
          ++sumCloseCells;
        }
      }
      if (sumCloseCells == tmp) {
        error = true;
      }
    }
    ++indexStep;
  }

  if (error == true) {
    return std::vector<coord>();
  } else {
    return GetPath(initial_position, end_position);
  }
}

std::pair<bool, bool> Maze::CellWalls(const size_t &row, const size_t &column) {
  return rowMaze_[row].CellWalls(column);
}

const size_t &Maze::get_rows() { return rows_; }

const size_t &Maze::get_columns() { return columns_; }

void Maze::set_start(const coord &val) { start_ = val; }

void Maze::set_finish(const coord &val) { finish_ = val; }

const Maze::coord &Maze::get_start() { return start_; }

const Maze::coord &Maze::get_finish() { return finish_; }

std::vector<bool> Maze::getMarixRightBorder() {
  std::vector<bool> rightBorder;
  for (size_t i = 0; i < rows_; ++i) {
    for (size_t j = 0; j < columns_; ++j) {
      rightBorder.push_back(rowMaze_[i][j].HaveRightBorder());
    }
  }
  return rightBorder;
}

std::vector<bool> Maze::getMarixDownBorder() {
  std::vector<bool> downBorder;
  for (size_t i = 0; i < rows_; ++i) {
    for (size_t j = 0; j < columns_; ++j) {
      downBorder.push_back(rowMaze_[i][j].HaveDownBorder());
    }
  }
  return downBorder;
}

void Maze::SaveCoordsMaxStep(std::vector<coord> *maxStep,
                             const int &valueMaxStep) {
  for (size_t i = 0; i < rows_; ++i) {
    for (size_t j = 0; j < columns_; ++j) {
      if (rowMaze_[i][j].GetValue() == valueMaxStep) {
        maxStep->push_back({i, j});
      }
    }
  }
}

bool Maze::IsFinishStep(const std::vector<coord> &cell,
                        const coord &finishCoord) {
  size_t tmp = cell.size();
  for (size_t index = 0; index < tmp; ++index) {
    if (cell[index].first == finishCoord.first &&
        cell[index].second == finishCoord.second) {
      return true;
    }
  }
  return false;
}

bool Maze::SearchPossibleStepNext(const coord &coordCell) {
  size_t row = coordCell.first;
  size_t column = coordCell.second;
  int valueNextCell = rowMaze_[row][column].GetValue() + 1;

  bool havePossibleStep = false;

  if (rowMaze_[row][column].HaveRightBorder() == false) {
    if (rowMaze_[row][column + 1].GetValue() == 0) {
      havePossibleStep = true;
      rowMaze_[row][column + 1].SetValue(valueNextCell);
    }
  }

  if (rowMaze_[row][column].HaveDownBorder() == false) {
    if (rowMaze_[row + 1][column].GetValue() == 0) {
      havePossibleStep = true;
      rowMaze_[row + 1][column].SetValue(valueNextCell);
    }
  }

  if (column != 0 && rowMaze_[row][column - 1].HaveRightBorder() == false) {
    if (rowMaze_[row][column - 1].GetValue() == 0) {
      havePossibleStep = true;
      rowMaze_[row][column - 1].SetValue(valueNextCell);
    }
  }

  if (row != 0 && rowMaze_[row - 1][column].HaveDownBorder() == false) {
    if (rowMaze_[row - 1][column].GetValue() == 0) {
      havePossibleStep = true;
      rowMaze_[row - 1][column].SetValue(valueNextCell);
    }
  }

  return havePossibleStep;
}

std::vector<Maze::coord> Maze::GetPath(const coord &initial_position,
                                       const coord &end_position) {
  coord start_step = end_position;
  coord finish_step = initial_position;

  std::vector<coord> HomeComing;
  HomeComing.push_back(start_step);

  while (IsStartStep(finish_step, start_step) == false) {
    SearchPossibleStepPrev(&start_step);
    HomeComing.push_back(start_step);
  }
  return HomeComing;
}

bool Maze::IsStartStep(const coord &searchCoord, const coord &currentCoord) {
  if (searchCoord.first == currentCoord.first &&
      searchCoord.second == currentCoord.second) {
    return true;
  }
  return false;
}

void Maze::SearchPossibleStepPrev(coord *coordCurrentCell) {
  size_t row = coordCurrentCell->first;
  size_t column = coordCurrentCell->second;

  int valueSearchCell = rowMaze_[row][column].GetValue() - 1;

  bool stepIsSearch = false;

  if (stepIsSearch == false) {
    if (rowMaze_[row][column].HaveRightBorder() == false) {
      if (rowMaze_[row][column + 1].GetValue() == valueSearchCell) {
        ++column;
        stepIsSearch = true;
      }
    }
  }

  if (stepIsSearch == false) {
    if (rowMaze_[row][column].HaveDownBorder() == false) {
      if (rowMaze_[row + 1][column].GetValue() == valueSearchCell) {
        ++row;
        stepIsSearch = true;
      }
    }
  }

  if (stepIsSearch == false) {
    if (column != 0 && rowMaze_[row][column - 1].HaveRightBorder() == false) {
      if (rowMaze_[row][column - 1].GetValue() == valueSearchCell) {
        --column;
        stepIsSearch = true;
      }
    }
  }

  if (stepIsSearch == false) {
    if (row != 0 && rowMaze_[row - 1][column].HaveDownBorder() == false) {
      if (rowMaze_[row - 1][column].GetValue() == valueSearchCell) {
        --row;
        stepIsSearch = true;
      }
    }
  }

  coordCurrentCell->first = row;
  coordCurrentCell->second = column;
}

/*---функции для загрузки лабиринта из файла---*/

void Maze::installRowsColumnsMaze(const std::string &lineData) {
  std::string number{};
  std::vector<int> value;
  for (size_t i = 0; i < lineData.size(); ++i) {
    if (lineData[i] == ' ') {
      value.push_back(std::stoi(number));
      number.clear();
    } else if (i == lineData.size() - 1) {
      number.push_back(lineData[i]);
      value.push_back(std::stoi(number));
      number.clear();
    } else {
      number.push_back(lineData[i]);
    }
  }
  if (value.size() != 2 || value.front() < 1 || value.back() < 1) {
    throw std::invalid_argument("Error, Incorrect LoadFile Maze");
  }
  InitMaze(value.front(), value.back());
}

void Maze::installRightBordersRowMaze(const size_t &indexRow,
                                      const std::string &lineData) {
  std::string number{};
  std::vector<bool> value;
  for (size_t i = 0; i < lineData.size(); ++i) {
    if (lineData[i] == ' ') {
      value.push_back(std::stoi(number));
      number.clear();
    } else if (i == lineData.size() - 1) {
      number.push_back(lineData[i]);
      value.push_back(std::stoi(number));
      number.clear();
    } else {
      number.push_back(lineData[i]);
    }
  }

  if (value.size() != columns_) {
    throw std::out_of_range("Error, Incorrect LoadFile Maze");
  }

  for (size_t indexColumn = 0; indexColumn < value.size(); ++indexColumn) {
    if (value[indexColumn] == true) {
      rowMaze_[indexRow][indexColumn].BuildRightBorder();
    } else {
      rowMaze_[indexRow][indexColumn].DestroyRightBorder();
    }
  }
}

void Maze::installDownBordersRowMaze(const size_t &indexRow,
                                     const std::string &lineData) {
  std::string number{};
  std::vector<bool> value;
  for (size_t i = 0; i < lineData.size(); ++i) {
    if (lineData[i] == ' ') {
      value.push_back(std::stoi(number));
      number.clear();
    } else if (i == lineData.size() - 1) {
      number.push_back(lineData[i]);
      value.push_back(std::stoi(number));
      number.clear();
    } else {
      number.push_back(lineData[i]);
    }
  }

  if (value.size() != columns_) {
    throw std::out_of_range("Error, Incorrect LoadFile Maze");
  }

  for (size_t indexColumn = 0; indexColumn < value.size(); ++indexColumn) {
    if (value[indexColumn] == true) {
      rowMaze_[indexRow][indexColumn].BuildDouwBorder();
    } else {
      rowMaze_[indexRow][indexColumn].DestroyDownBorder();
    }
  }
}

/*------------------------------------------------------*/

}  // namespace s21
