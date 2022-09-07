#include "mainwindow.hpp"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QGraphicsRectItem>
#include <QMessageBox>

#include "ui_mainwindow.h"

namespace s21 {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      controller_(new Controller),
      cave_scene_(new QGraphicsScene),
      maze_scene_(new MazeScene),
      timer_(new QTimer) {
  ui->setupUi(this);

  ui->graphicsView->setScene(maze_scene_);
  ui->slider_maze_width->setValue(10);
  ui->slider_maze_height->setValue(10);
  controller_->set_maze_size(10, 10);

  ui->slider_cave_birth->setValue(4);
  ui->slider_cave_death->setValue(4);
  ui->slider_cave_init->setValue(50);
  ui->slider_cave_delay->setValue(10);
  ui->slider_cave_width->setValue(10);
  ui->slider_cave_height->setValue(10);
  controller_->set_cave_size(10, 10);

  valid_cave_size_.setRange(1, 50);
  valid_cave_limit_.setRange(0, 7);
  valid_cave_perc_.setRange(0, 100);
  ui->line_cave_width->setValidator(&valid_cave_size_);
  ui->line_cave_height->setValidator(&valid_cave_size_);
  ui->line_cave_birth->setValidator(&valid_cave_limit_);
  ui->line_cave_death->setValidator(&valid_cave_limit_);
  ui->line_cave_init->setValidator(&valid_cave_perc_);
  ui->line_cave_delay->setValidator(&valid_cave_perc_);
  ui->line_maze_height->setValidator(&valid_cave_size_);
  ui->line_maze_width->setValidator(&valid_cave_size_);

  connect(maze_scene_, SIGNAL(LMousePressed(double, double)), this,
          SLOT(SlotLClick(double, double)));
  connect(maze_scene_, SIGNAL(RMousePressed()), this, SLOT(SlotRClick()));
  connect(timer_, SIGNAL(timeout()), this, SLOT(on_but_cave_step_clicked()));
}

MainWindow::~MainWindow() {
  delete ui;
  delete controller_;
  delete cave_scene_;
  delete maze_scene_;
  delete timer_;
}

void MainWindow::FormCaveScene() {
  cave_scene_->clear();
  unsigned num_x{controller_->get_cave_cols()},
      num_y{controller_->get_cave_rows()};
  if (num_x > 0 && num_y > 0) {
    double tile_height = kSceneSize / num_y;
    double tile_width = kSceneSize / num_x;
    for (unsigned i{}; i < num_y; i++) {
      for (unsigned j{}; j < num_x; j++) {
        if (controller_->CaveValue(i, j))
          cave_scene_->addRect(tile_width * j, tile_height * i, tile_width,
                               tile_height, QPen(Qt::black), QBrush(Qt::black));
        else
          cave_scene_->addRect(tile_width * j, tile_height * i, tile_width,
                               tile_height, QPen(Qt::white), QBrush(Qt::white));
      }
    }
    ui->graphicsView->fitInView(cave_scene_->sceneRect(),
                                Qt::IgnoreAspectRatio);
  }
}

void MainWindow::ResizeCave() {
  controller_->set_cave_size(ui->slider_cave_height->value(),
                             ui->slider_cave_width->value());
  FormCaveScene();
}

void MainWindow::ResizeMaze() {
  controller_->set_maze_size(ui->slider_maze_height->value(),
                             ui->slider_maze_width->value());
  points_set_ = 0;
}

void MainWindow::FormMazeScene() {
  maze_scene_->clear();
  size_t num_x{controller_->get_maze_cols()};
  size_t num_y{controller_->get_maze_rows()};
  if (num_x > 0 && num_y > 0) {
    QPen my_pen(this->palette().text(), kWallThickness);
    double field_size = kSceneSize - kWallThickness;
    double shift = kWallThickness / 2;
    double tile_height = (field_size) / num_y;
    double tile_width = (field_size) / num_x;
    maze_scene_->addLine(shift, shift, shift, field_size, my_pen);
    maze_scene_->addLine(shift, shift, field_size, shift, my_pen);
    for (size_t i{}; i < num_y; i++) {
      for (size_t j{}; j < num_x; j++) {
        std::pair<bool, bool> one = controller_->MazeWalls(i, j);
        if (one.first)
          maze_scene_->addLine(shift + tile_width * (1 + j),
                               shift + i * tile_height,
                               shift + tile_width * (1 + j),
                               shift + tile_height * (i + 1), my_pen);
        if (one.second)
          maze_scene_->addLine(shift + tile_width * j,
                               shift + tile_height * (i + 1),
                               shift + tile_width * (1 + j),
                               shift + tile_height * (i + 1), my_pen);
      }
    }
    if (points_set_ == 1) {
      maze_scene_->addEllipse(
          shift + startStep_.first * tile_width + kWallThickness,
          shift + startStep_.second * tile_height + kWallThickness,
          tile_width - 2 * kWallThickness, tile_height - 2 * kWallThickness,
          QPen(Qt::green), QBrush(Qt::green));
    } else if (points_set_ == 2) {
      auto path = controller_->SearchPath();
      if (path.empty()) {
        maze_scene_->addEllipse(
            shift + startStep_.first * tile_width + kWallThickness,
            shift + startStep_.second * tile_height + kWallThickness,
            tile_width - 2 * kWallThickness, tile_height - 2 * kWallThickness,
            QPen(Qt::red), QBrush(Qt::red));
        maze_scene_->addEllipse(
            shift + finishStep_.first * tile_width + kWallThickness,
            shift + finishStep_.second * tile_height + kWallThickness,
            tile_width - 2 * kWallThickness, tile_height - 2 * kWallThickness,
            QPen(Qt::red), QBrush(Qt::red));
      } else {
        for (size_t i{}; i < path.size() - 1; i++) {
          maze_scene_->addLine(
              shift + tile_width / 2 + path[i].second * tile_width,
              shift + tile_height / 2 + path[i].first * tile_height,
              shift + tile_width / 2 + path[i + 1].second * tile_width,
              shift + tile_height / 2 + path[i + 1].first * tile_height,
              QPen(Qt::green));
        }
      }
    }
    ui->graphicsView->fitInView(maze_scene_->sceneRect(),
                                Qt::IgnoreAspectRatio);
  }
}

void MainWindow::on_but_cave_load_clicked() {
  QString filename = QFileDialog::getOpenFileName(
      this, "Select a file to open...");  // QDir::homePath()
  controller_->LoadCave(filename);
  ui->slider_cave_width->setValue(controller_->get_cave_cols());
  ui->slider_cave_height->setValue(controller_->get_cave_rows());
  ui->line_cave_width->setText(QString::number(controller_->get_cave_cols()));
  ui->line_cave_height->setText(QString::number(controller_->get_cave_rows()));
  FormCaveScene();
}

void MainWindow::on_tabWidget_currentChanged(int index) {
  if (index == 0) {
    ui->graphicsView->setScene(maze_scene_);
    if (timer_->isActive()) {
      timer_->stop();
      ui->but_cave_auto->setText("Auto");
    }
  } else if (index == 1) {
    ui->graphicsView->setScene(cave_scene_);
  }
}

void MainWindow::on_but_cave_step_clicked() {
  controller_->IterateCave();
  FormCaveScene();
}

void MainWindow::on_but_cave_save_clicked() {
  QString filename =
      QFileDialog::getSaveFileName(this, "Select file to save to...");
  controller_->SaveCave(filename);
}

void MainWindow::on_but_cave_generate_clicked() {
  controller_->RandomizeCave();
  FormCaveScene();
}

void MainWindow::on_slider_cave_birth_valueChanged(int value) {
  ui->line_cave_birth->setText(QString::number(value));
  controller_->set_cave_birth_lim(value);
}

void MainWindow::on_slider_cave_death_valueChanged(int value) {
  ui->line_cave_death->setText(QString::number(value));
  controller_->set_cave_death_lim(value);
}

void MainWindow::on_slider_cave_init_valueChanged(int value) {
  ui->line_cave_init->setText(QString::number(value));
  controller_->set_cave_fill_coef(value);
}

void MainWindow::on_slider_cave_delay_valueChanged(int value) {
  ui->line_cave_delay->setText(QString::number(value * 10));
  timer_->setInterval(ui->line_cave_delay->text().toInt());
}

void MainWindow::on_slider_cave_width_valueChanged(int value) {
  ui->line_cave_width->setText(QString::number(value));
}

void MainWindow::on_slider_cave_height_valueChanged(int value) {
  ui->line_cave_height->setText(QString::number(value));
}

void MainWindow::on_slider_cave_width_sliderReleased() { ResizeCave(); }

void MainWindow::on_slider_cave_height_sliderReleased() { ResizeCave(); }

void MainWindow::on_line_cave_width_editingFinished() {
  ui->slider_cave_width->setValue(ui->line_cave_width->text().toInt());
  ResizeCave();
}

void MainWindow::on_line_cave_height_editingFinished() {
  ui->slider_cave_height->setValue(ui->line_cave_height->text().toInt());
  ResizeCave();
}

void MainWindow::on_line_cave_birth_editingFinished() {
  ui->slider_cave_birth->setValue(ui->line_cave_birth->text().toInt());
  controller_->set_cave_birth_lim(ui->slider_cave_birth->value());
}

void MainWindow::on_line_cave_death_editingFinished() {
  ui->slider_cave_death->setValue(ui->line_cave_death->text().toInt());
  controller_->set_cave_death_lim(ui->slider_cave_death->value());
}

void MainWindow::on_line_cave_init_editingFinished() {
  ui->slider_cave_init->setValue(ui->line_cave_init->text().toInt());
  controller_->set_cave_fill_coef(ui->slider_cave_init->value());
}

void MainWindow::on_line_cave_delay_editingFinished() {
  ui->slider_cave_delay->setValue(ui->line_cave_delay->text().toInt() / 10);
  timer_->setInterval(ui->line_cave_delay->text().toInt());
}

void MainWindow::on_but_cave_auto_clicked() {
  if (timer_->isActive()) {
    ui->but_cave_auto->setText("Auto");
    timer_->stop();
  } else {
    ui->but_cave_auto->setText("Stop");
    timer_->start(ui->line_cave_delay->text().toInt());
  }
}

void MainWindow::on_but_maze_load_clicked() {
  points_set_ = 0;
  QString filename =
      QFileDialog::getOpenFileName(this, "Select a file to open...");

  if (!filename.isEmpty()) {
      QString resultLoad = QString::fromLocal8Bit(controller_->LoadMaze(filename).c_str());
      QMessageBox::information(this, "Result Load", resultLoad);
      if (resultLoad == "Load maze sucsess") {
          ui->slider_maze_width->setValue(controller_->get_maze_cols());
          ui->slider_maze_height->setValue(controller_->get_maze_rows());
          ui->line_maze_width->setText(QString::number(controller_->get_maze_cols()));
          ui->line_maze_height->setText(QString::number(controller_->get_maze_rows()));
          FormMazeScene();
      }
  }
}

void MainWindow::on_but_maze_save_clicked() {
  QString filename =
      QFileDialog::getSaveFileName(this, "Select file to save to...");
  controller_->SaveMaze(filename);
}

void MainWindow::on_but_maze_generate_clicked() {
  points_set_ = 0;
  ResizeMaze();
  controller_->GenerateMaze();
  FormMazeScene();
}

void MainWindow::on_slider_maze_width_valueChanged(int value) {
  ui->line_maze_width->setText(QString::number(value));
}

void MainWindow::on_slider_maze_height_valueChanged(int value) {
  ui->line_maze_height->setText(QString::number(value));
}

void MainWindow::on_line_maze_width_editingFinished() {
  ui->slider_maze_width->setValue(ui->line_maze_width->text().toInt());
}

void MainWindow::on_line_maze_height_editingFinished() {
  ui->slider_maze_height->setValue(ui->line_maze_height->text().toInt());
}

void MainWindow::SlotLClick(double x, double y) {
  double tile_height =
      (kSceneSize - kWallThickness) / controller_->get_maze_rows();
  double tile_width =
      (kSceneSize - kWallThickness) / controller_->get_maze_cols();
  size_t row = (y - kWallThickness / 2) / tile_height;
  size_t col = (x - kWallThickness / 2) / tile_width;
  if (col < controller_->get_maze_cols() &&
      row < controller_->get_maze_rows()) {
    if (points_set_ == 0) {
      startStep_ = {col, row};
      controller_->set_path_start({row, col});
      points_set_ = 1;
    } else {
      finishStep_ = {col, row};
      controller_->set_path_finish({row, col});
      points_set_ = 2;
    }
    FormMazeScene();
  }
}

void MainWindow::SlotRClick() {
  points_set_ = 0;
  FormMazeScene();
}

}  // namespace s21
