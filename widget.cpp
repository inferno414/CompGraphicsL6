#include "widget.h"
#include "ui_widget.h"
#include <QtAlgorithms>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    curve = Bezier;

    ui->setupUi(this);
    //setAcceptDrops(true);
    ui->label->hide();
    ui->spinBox->hide();
    timer.setInterval(30);
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(update()));
    timer.start();
}

Widget::~Widget()
{
    delete ui;
}
bool ls(QPoint &a, QPoint &b){
    return a.x() < b.x();
}

QPoint Widget::curveBezier(QVector<QPoint> & list, float t){
    int n = list.size();
    float rx=0,ry=0;
    float c;
    for(int i=0; i<n; i++){
        c = fact(n-1,i)/fact(n-i-1,0)*pow(t,i)*(pow(1-t,n-1-i));
        rx += list[i].x()*c;
        ry += list[i].y()*c;
    }
    return QPoint(rx,ry);
}

void Widget::drawCurveBezier(QPainter &p){
    p.setPen(QColor(50,255,50));
    for(int i = 0, n = list.size(); i < n-1; i++)
        p.drawLine(list[i],list[i+1]);
    QPoint p1,p2;
    p.setPen(QColor(0,0,255));
    p1 = curveBezier(list,0.01);
    for(float t=0.01; t<1; t+=0.01){
        p2 = curveBezier(list,t);
        p.drawLine(p1,p2);
        p1 = p2;
        //std::cout<< p1.x() << ' '<< p1.y() << std::endl;
    }
}



int Widget::fact(unsigned int i,unsigned int k){
    return i==k ? 1 : i*fact(i-1,k);
}



float Widget::getWeight(int i, int k, float t, QVector<int> &knot)
{
    if (k == 1)
    {
        if (knot[i] <= t && t < knot[i+1])
            return 1;
        else return 0;
    }

    float aa = t - knot[i];
    float ab = knot[i + k - 1] - knot[i];
    float ba = knot[i + k] - t;
    float bb = knot[i + k] - knot[i + 1];
    float A = 0, B = 0;

    if (ab != 0) A = aa / ab * getWeight (i, k - 1, t, knot);
    if (bb != 0) B = ba / bb * getWeight (i + 1, k - 1, t, knot);
    return A + B;
}

void Widget::drawBSpline(QPainter &p){
    QPoint p1,p2;
    p.setPen(QColor(255,0,0));
    int k=ui->spinBox->value(), n = list.size(), N=100;
    if(n >= k){
        QVector<int> knot;
        for (int count = 0; count < k+n; count++){
            knot.push_back(count);
        }

        for(int st=0; st <= N; ++st){
            float t = (st/double(N)) * (n-(k-1)) + k-1;
            float x = 0.0, y = 0.0;
            for(int i=0; i < n; i++){
                float w = getWeight(i,k,t,knot);
                x += w*list[i].x();
                y += w*list[i].y();
            }
            p1.setX(x);
            p1.setY(y);
            //std::cout<< p1.x() << ' '<< p1.y() << std::endl;
            if(st){
                p.drawLine(p1,p2);
            }
            p2 = p1;
        }
    }
}
float Widget::LagrangeCoef(int x, int i){
    int n = list.size();
    float a = 1,b = 1;
    for (int j = 0; j<n;j++){
        a *= (j == i) ? 1 : x - list[j].x();
    }
    for (int j = 0;j<n;j++){
        b *= (list[i].x() == list[j].x()) ? 1 : list[i].x()-list[j].x();
    }
    return a/b;
}

void Widget::drawLagrangeCurve(QPainter &p){
    qSort(list.begin(),list.end(),ls);
    int n = list.size();
    QPoint p1,p2;
    p.setPen(QColor(50,255,50));
    float inf = list[0].x();
    for (float t = list[0].x(); t <= list[n-1].x() ; t+=0.03){
        float y=0;
        for (int i = 0; i<n ; i++){
            y += list[i].y()*LagrangeCoef(t,i);
        }
        p1.setX(t);
        p1.setY(y);
        if (t != inf) p.drawLine(p1,p2);
        p2 = p1;
    }
}

void Widget::mousePressEvent(QMouseEvent *e){
    bool exist = false;
    int i = 0;
    for(auto elem : list){
        if(e->x() <= elem.x()+dr+dr && \
                e->x() >= elem.x()-dr-dr && \
                e->y() <= elem.y()+dr+dr && \
                e->y() >= elem.y()-dr-dr     )
        {
            exist = true;
            if (e->button() == Qt::RightButton)
                list.remove(i);
            else {
                list[i].setX(e->x());
                list[i].setY(e->y());
            }
        }
        i++;
    }
    if (!exist && e->button() == Qt::LeftButton)
        list.push_back( QPoint(e->x(),e->y()) );
}

void Widget::paintEvent(QPaintEvent *){
    QPainter painter(this);

    painter.setBrush(QBrush(QColor(255,255,0)));
    for(auto elem :list){
        painter.drawEllipse(elem,dr,dr);
    }
    if (!list.empty()){
        switch (curve) {
        case Bezier:
            drawCurveBezier(painter);
            break;
        case BSpline:
            drawBSpline(painter);
            break;
        case Lagrange:
            drawLagrangeCurve(painter);
            break;
        }

        /*for (auto i:list)
                std::cout<< i.x() << ' '<< i.y() << ' ';
            std::cout << std::endl;*/

    }
}

void Widget::on_pushButton_clicked()
{
    list.erase(list.begin(),list.end());
}

void Widget::on_pushButton_2_clicked()
{
    close();
}

void Widget::on_comboBox_activated(const QString &arg1)
{
    if (arg1 == "Bezier"){
        curve = Bezier;
        ui->label->hide();
        ui->spinBox->hide();
    } else if (arg1 == "BSpline"){
        curve = BSpline;
        ui->label->show();
        ui->spinBox->show();
    } else if (arg1 == "Lagrange"){
        curve = Lagrange;
        ui->label->hide();
        ui->spinBox->hide();
    }
}
