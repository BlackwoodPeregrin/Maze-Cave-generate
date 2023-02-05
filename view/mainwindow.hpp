#ifndef SRC_MAINWINDOW_HPP_
#define SRC_MAINWINDOW_HPP_

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QIntValidator>
#include <QMainWindow>
#include <QTimer>

#include "../controller/controller.hpp"
#include "mazescene.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace s21 {

constexpr double kSceneSize = 500;
constexpr double kWallThickness = 2.01;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_but_cave_load_clicked();
  void on_tabWidget_currentChanged(int index);
  void on_but_cave_step_clicked();
  void on_but_cave_save_clicked();
  void on_but_cave_generate_clicked();
  void on_slider_cave_birth_valueChanged(int value);
  void on_slider_cave_death_valueChanged(int value);
  void on_slider_cave_init_valueChanged(int value);
  void on_slider_cave_delay_valueChanged(int value);
  void on_slider_cave_width_valueChanged(int value);
  void on_slider_cave_height_valueChanged(int value);
  void on_slider_cave_width_sliderReleased();
  void on_slider_cave_height_sliderReleased();
  void on_line_cave_width_editingFinished();
  void on_line_cave_height_editingFinished();
  void on_line_cave_birth_editingFinished();
  void on_line_cave_death_editingFinished();
  void on_line_cave_init_editingFinished();
  void on_line_cave_delay_editingFinished();
  void on_but_cave_auto_clicked();

  void on_but_maze_load_clicked();
  void on_but_maze_save_clicked();
  void on_but_maze_generate_clicked();
  void on_slider_maze_width_valueChanged(int value);
  void on_slider_maze_height_valueChanged(int value);
  void on_line_maze_width_editingFinished();
  void on_line_maze_height_editingFinished();
  void SlotLClick(double x, double y);
  void SlotRClick();

 private:
  void FormCaveScene();
  void ResizeCave();
  void ResizeMaze();
  void FormMazeScene();

  Ui::MainWindow *ui;
  Controller *controller_;
  QGraphicsScene *cave_scene_;
  MazeScene *maze_scene_;
  QIntValidator valid_cave_size_, valid_cave_limit_, valid_cave_perc_;
  QTimer *timer_;
  int points_set_{};
  std::pair<size_t, size_t> startStep_;
  std::pair<size_t, size_t> finishStep_;
};

}  // namespace s21

#endif  // SRC_MAINWINDOW_HPP_
