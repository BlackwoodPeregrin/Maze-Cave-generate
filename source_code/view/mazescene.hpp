#ifndef MAZESCENE_HPP
#define MAZESCENE_HPP

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

namespace s21 {

class MazeScene : public QGraphicsScene {
  Q_OBJECT

 public:
  explicit MazeScene(QObject *parent = nullptr) : QGraphicsScene(parent) {}

 signals:
  void LMousePressed(double x, double y);
  void RMousePressed();

 private:
  void mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
      emit LMousePressed(event->scenePos().x(), event->scenePos().y());
    } else if (event->buttons() == Qt::RightButton) {
      emit RMousePressed();
    }
  }
};

}  // namespace s21

#endif  // MAZESCENE_HPP
