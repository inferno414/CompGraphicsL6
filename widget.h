#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <iostream>
#include "curves.h"

namespace Ui {
class Widget;
}
enum Curves{Bezier,BSpline,Lagrange};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    Curves curve;
    QVector<QPoint> list;
    const int dr = 3;
    QTimer timer;
    int mp;
    int mx,my;

    int fact(unsigned int, unsigned int);

    float getWeight(int i,int k,float t, QVector<int> &knot);
    QPoint curveBezier(QVector<QPoint> &, float);

    void drawBSpline(QPainter &p);
    void drawCurveBezier(QPainter &p);

    float LagrangeCoef(int x, int i);
    void drawLagrangeCurve(QPainter &p);

    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_activated(const QString &arg1);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
