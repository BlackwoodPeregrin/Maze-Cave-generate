#ifndef SRC_CONTROLLER_HPP_
#define SRC_CONTROLLER_HPP_

#include <QString>

#include "../model/cave.hpp"
#include "../model/maze.hpp"

namespace s21 {

class Controller {
 public:
  Controller() : cave_(new Cave), maze_(new Maze) {}
  ~Controller() {
    delete cave_;
    delete maze_;
  }

  void set_cave_death_lim(unsigned a) { cave_->set_death_limit(a); }
  void set_cave_birth_lim(unsigned a) { cave_->set_birth_limit(a); }
  void set_cave_fill_coef(unsigned a) { cave_->set_fill_coef(a); }
  void set_cave_size(unsigned a, unsigned b) { cave_->set_size(a, b); }
  unsigned get_cave_rows() { return cave_->get_rows(); }
  unsigned get_cave_cols() { return cave_->get_cols(); }
  void RandomizeCave() { cave_->FillRandomly(); }
  void IterateCave() { cave_->Iterate(); }
  bool& CaveValue(unsigned x, unsigned y) {
    try {
      return (*cave_)(x, y);
    } catch (std::out_of_range &e) {
      if (cave_->get_cols() == 0 || cave_->get_rows() == 0)
        cave_->set_size(1, 1);
      return (*cave_)(0, 0);
    }
  }
  void SaveCave(const QString& filename) {
    cave_->Save(filename.toStdString());
  }
  void LoadCave(const QString& filename) {
    try {
      cave_->Load(filename.toStdString());
    } catch (std::invalid_argument &e) {}
  }

  void set_maze_size(const size_t& rows, const size_t& cols) {
    maze_->InitMaze(rows, cols);
  }
  void SaveMaze(const QString& filename) {
    try {
      maze_->SaveMaze(filename.toStdString());
    } catch (std::out_of_range &e) {}
  }
  std::string LoadMaze(const QString& filename) {
      try {
        maze_->LoadMaze(filename.toStdString());
        return "Load maze sucsess";
      }  catch (const std::exception &e) {
        return e.what();
      }
  }
  void GenerateMaze() {
    try {
      maze_->GenerationIdealMaze();
    } catch (std::out_of_range &e) {}
  }
  std::pair<bool, bool> MazeWalls(size_t row, size_t col) {
    return maze_->CellWalls(row, col);
  }
  size_t get_maze_rows() { return maze_->get_rows(); }
  size_t get_maze_cols() { return maze_->get_columns(); }

  std::vector<Maze::coord> SearchPath() {
    try {
      return maze_->SearchPath();
    } catch (std::out_of_range &e) {
      std::vector<Maze::coord> nothing{};
      return nothing;
    }
  }
  void set_path_start(Maze::coord val) { maze_->set_start(val); }
  void set_path_finish(Maze::coord val) { maze_->set_finish(val); }

 private:
  Cave* cave_;
  Maze* maze_;
};

}  // namespace s21

#endif  // SRC_CONTROLLER_HPP_
