#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <QWidget>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPoint>
#include <list>

using namespace cv;

class DisplayWidget : public QWidget
{
    Q_OBJECT

public:
    DisplayWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent( QMouseEvent *event );
    void mouseMoveEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event);
    void wheelEvent( QWheelEvent *event );
    void keyPressEvent( QKeyEvent *event );

public slots:
    void eraseTopImage();

private:
    std::list<Mat> paintImageList;
    std::list<Mat> originalImageList;
    //topleft point of drawing image
    std::list<QPoint> drawPointList;
    //used in amplified operation
    std::list<double> scaleList;

    int drawImageCount;//number of painted images


    bool imageSelected;


    //used in drag operation
    bool mouseDown;
    QPoint previousPoint;//used to caculate the mouse movement
    QPoint presentPoint;//used to caculate the mouse movement
    QPoint moveMent;//mouse movement,used to repaint image
    //**************************



private:
    bool clickInImage( QPoint clickPoint );

public:
    bool setPaintImage( Mat const &image );
    Mat getCurrentImage();
    Mat getSecondImage();
    bool isEmpty();
    void clearAll();
};

#endif
