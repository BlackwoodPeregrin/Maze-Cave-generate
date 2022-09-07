#include <gtest/gtest.h>

#include "cave.hpp"
#include "maze.hpp"

TEST(Maze, loadMaze) {
  std::vector<bool> rightBorders_expected{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1};

  std::vector<bool> downBorders_expected{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                                         1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1};
  s21::Maze maze;
  maze.LoadMaze("files_maze_cave/test_load_maze.txt");

  std::vector<bool> rightBorders_actual(maze.getMarixRightBorder());
  std::vector<bool> downBorders_actual(maze.getMarixDownBorder());

  ASSERT_TRUE(rightBorders_expected == rightBorders_actual);
  ASSERT_TRUE(downBorders_expected == downBorders_actual);
}

TEST(Maze, saveMaze) {
  s21::Maze maze;
  maze.LoadMaze("files_maze_cave/test_load_maze.txt");
  maze.SaveMaze("files_maze_cave/test_save_maze.txt");

  std::vector<bool> rightBorders_expected{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1};

  std::vector<bool> downBorders_expected{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                                         1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1};
  maze.LoadMaze("test_save_maze.txt");

  std::vector<bool> rightBorders_actual(maze.getMarixRightBorder());
  std::vector<bool> downBorders_actual(maze.getMarixDownBorder());

  ASSERT_TRUE(rightBorders_expected == rightBorders_actual);
  ASSERT_TRUE(downBorders_expected == downBorders_actual);
}

TEST(Maze, searchPath) {
  s21::Maze maze;
  maze.LoadMaze("files_maze_cave/test_load_maze.txt");
  s21::Maze::coord start{0, 0};
  s21::Maze::coord finish{maze.get_rows() - 1, maze.get_columns() - 1};
  std::vector<s21::Maze::coord> path = maze.SearchPath(start, finish);

  ASSERT_TRUE(path.front().first == finish.first);
  ASSERT_TRUE(path.front().second == finish.second);
  ASSERT_TRUE(path.back().first == start.first);
  ASSERT_TRUE(path.back().second == start.second);
}

TEST(Cave, setsize) {
  s21::Cave cave;
  cave.set_size(5, 7);
  ASSERT_EQ(cave.get_rows(), 5);
  ASSERT_EQ(cave.get_cols(), 7);
  cave.set_size(0, 0);
  cave.set_size(100, 100);
}

TEST(Cave, setparam) {
  s21::Cave cave;
  cave.set_birth_limit(3);
  cave.set_death_limit(5);
  cave.set_fill_coef(7);
  cave.set_birth_limit(10);
  cave.set_death_limit(10);
  cave.set_fill_coef(500);
}

TEST(Cave, fill) {
  s21::Cave cave(3, 3);
  cave.set_birth_limit(4);
  cave.set_death_limit(4);
  cave.set_fill_coef(100);
  cave.FillRandomly();
  ASSERT_TRUE(cave(1, 1));
  cave.set_fill_coef(0);
  cave.FillRandomly();
  ASSERT_FALSE(cave(0, 0));
  cave.Iterate();
  ASSERT_TRUE(cave(0, 0));
}

TEST(Cave, save_load) {
  s21::Cave A(2, 1);
  A(0, 0) = true;
  A(1, 0) = false;
  A.Save("cave_test.txt");
  s21::Cave B;
  B.Load("cave_test.txt");
  ASSERT_TRUE(B(0, 0));
  ASSERT_FALSE(B(1, 0));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
