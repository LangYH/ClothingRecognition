#include "displaywidget.h"
#include <QPalette>
#include <QPainter>
#include <QMessageBox>

#define Max_DRAWIMAGE 300

DisplayWidget::DisplayWidget(QWidget *parent) :
    QWidget(parent)
{
    //intialize key flag or count variable
    mouseDown = false;
    imageSelected = false;
    drawImageCount = 0;

    //intialize window size
    resize( 800, 600 );
    //maximum size, full window
    setAutoFillBackground(true);

    QPalette palette;
    palette.setColor( QPalette::Background, QColor( 128, 128, 128 ));
    setPalette( palette );
}


bool DisplayWidget::setPaintImage( Mat const &sourceImage )
{
    if( drawImageCount >= Max_DRAWIMAGE )
    {
        QMessageBox::information( NULL, tr( "Full Image Pool" ), tr( "Too many Images, please delete some first! ") );
        return false;
    }
    Mat originalImage, paintImage;
    sourceImage.copyTo( originalImage );
    if( originalImage.type() != CV_8UC1 || originalImage.type() != CV_8UC3 ){
        double minVal, maxVal;
        minMaxLoc( originalImage, &minVal, &maxVal );
        double s = 255.0 / maxVal;
        originalImage *= s;
    }
    convertScaleAbs( originalImage, originalImage );//convert image to uint8 type
    originalImage.copyTo( paintImage );

    originalImageList.push_back( originalImage );
    paintImageList.push_back( paintImage );
    scaleList.push_back( 1.0 );

    QPoint drawPoint;
    drawPoint = QPoint( ( frameSize().width() - sourceImage.cols )/2,
                        ( frameSize().height() - sourceImage.rows ) /2 );
    drawPointList.push_back( drawPoint );

    update();
    return true;
}

void DisplayWidget::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    std::list<Mat>::iterator paintImageListIter = paintImageList.begin();
    std::list<QPoint>::iterator drawPointListIter = drawPointList.begin();
    while ( paintImageListIter != paintImageList.end() )
    {
        Mat paintImage = *paintImageListIter;
        if( !paintImage.empty() )
        {
            Mat temp;
            QImage img;
            if( paintImage.channels() == 3 )
            {
                cvtColor(paintImage,temp,CV_BGR2RGB);
            }
            else
            {
                //before display in Qt, image has to be changed to RGB format
                cvtColor( paintImage, temp, CV_GRAY2RGB );

            }
            img= QImage((const unsigned char*)(temp.data),
            temp.cols,temp.rows,temp.cols*temp.channels(),QImage::Format_RGB888);

            painter.drawPixmap( *drawPointListIter, QPixmap::fromImage( img ) );
        }
        paintImageListIter++;
        drawPointListIter++;
    }

    //draw a rectangle for tagging the current image
    if( !paintImageList.empty())
    {
        int topLeftX = ( *drawPointList.rbegin() ).x();
        int topLeftY = ( *drawPointList.rbegin() ).y();
        int width = ( paintImageList.rbegin() ) -> cols;
        int height = ( paintImageList.rbegin() ) -> rows;
        painter.drawRect( QRect( topLeftX, topLeftY, width, height ));
    }
}

void DisplayWidget::mousePressEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton && clickInImage( event->pos() ) )
    {
        mouseDown = true;
        previousPoint = event->pos();
    }

}

void DisplayWidget::mouseMoveEvent( QMouseEvent *event )
{
    if( event ->buttons() & Qt::LeftButton )
    {
        if( mouseDown)
        {
            presentPoint = event->pos();
            moveMent = presentPoint - previousPoint;
            *drawPointList.rbegin() += moveMent;
            previousPoint = presentPoint;
            update();
        }
    }

}

void DisplayWidget::mouseReleaseEvent( QMouseEvent *event)
{
    if( event->button() == Qt::LeftButton )
    {
        mouseDown = false;
        moveMent = QPoint( 0, 0 );
    }

}

void DisplayWidget::wheelEvent(QWheelEvent *event)
{
    if( !clickInImage( event->pos() ))
    {
        return ;
    }

    QPoint coorInImage = event->pos() - *drawPointList.rbegin();
    QPoint movement = QPoint( 0, 0) ;

    if( event->angleDelta().y() > 0 )
    {
        movement = coorInImage / *scaleList.rbegin() * 0.1 ;
        *scaleList.rbegin() += 0.1;

    }
    else
    {
        if( *scaleList.rbegin() > 0.2 )
        {
            movement = -coorInImage / *scaleList.rbegin() * 0.1;
            *scaleList.rbegin() -= 0.1;
        }
    }

    cv::resize( *originalImageList.rbegin(), *paintImageList.rbegin(),
                Size(int( originalImageList.rbegin()->cols * (*scaleList.rbegin()) ),
                     int( originalImageList.rbegin()->rows * (*scaleList.rbegin()) ) ) );
    *drawPointList.rbegin() -= movement;


    update();

}

void DisplayWidget::keyPressEvent( QKeyEvent *event )
{
    switch ( event->key() ) {
    case Qt::Key_Delete:
        //QMessageBox::information( NULL, tr( "Key Pressed" ), tr( "Del pressed ") );
        eraseTopImage();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void DisplayWidget::eraseTopImage()
{

    if( imageSelected && !paintImageList.empty() )
    {

        paintImageList.erase( (++paintImageList.rbegin()).base() );
        originalImageList.erase( (++originalImageList.rbegin()).base() );
        drawPointList.erase( (++drawPointList.rbegin()).base() );
        scaleList.erase( (++scaleList.rbegin() ).base() );

        update();
    }
}


bool DisplayWidget::clickInImage( QPoint clickPoint )
{
    std::list<QPoint>::reverse_iterator drawPointListIter = drawPointList.rbegin();
    std::list<Mat>::reverse_iterator paintImageListIter = paintImageList.rbegin();
    std::list<Mat>::reverse_iterator originalImageListIter = originalImageList.rbegin();
    std::list<double>::reverse_iterator scaleListIter = scaleList.rbegin();
    while ( drawPointListIter != drawPointList.rend() )
    {
        QPoint drawPoint = *drawPointListIter;
        Mat paintImage = *paintImageListIter;
        if( clickPoint.x() > drawPoint.x() && clickPoint.x() < ( drawPoint.x() + paintImage.cols ) &&
                                        clickPoint.y() > drawPoint.y() && clickPoint.y() < ( drawPoint.y() + paintImage.rows ) )
        {
            //move selected image to the end of list
            setFocus();//capture focus
            imageSelected = true;

            Mat temp_paintImage, temp_originalImage;
            QPoint temp_drawPoint;
            double temp_scale = *scaleListIter;

            paintImageListIter->copyTo( temp_paintImage );
            originalImageListIter->copyTo( temp_originalImage );
            temp_drawPoint = drawPoint;

            paintImageListIter = std::list<Mat>::reverse_iterator( paintImageList.erase( (++paintImageListIter).base() ) );
            paintImageList.push_back( temp_paintImage );

            originalImageListIter = std::list<Mat>::reverse_iterator( originalImageList.erase( (++originalImageListIter).base() ) );
            originalImageList.push_back( temp_originalImage );

            drawPointListIter = std::list<QPoint>::reverse_iterator( drawPointList.erase( (++drawPointListIter).base() ) );
            drawPointList.push_back( temp_drawPoint );

            scaleListIter = std::list<double>::reverse_iterator( scaleList.erase( (++scaleListIter).base() ) );
            scaleList.push_back( temp_scale );

            update();

            return true;
        }
        else
        {
            imageSelected = false;
            drawPointListIter++;
            paintImageListIter++;
            originalImageListIter++;
            scaleListIter++;
        }
    }
    return false;
}

Mat DisplayWidget::getCurrentImage()
{
    Mat currentImage;
    if( !paintImageList.empty())
    {
        (*originalImageList.rbegin()).copyTo(currentImage);
    }
    return currentImage;

}

Mat DisplayWidget::getSecondImage()
{
    Mat secondImage;
    if( !originalImageList.empty() && originalImageList.size() >= 2 ){
        std::list<Mat>::reverse_iterator originalImageListIter = originalImageList.rbegin();
        return *( ++originalImageListIter );

    }else{
        return secondImage;
    }


}

bool DisplayWidget::isEmpty()
{
    return originalImageList.empty();
}

void DisplayWidget::clearAll()
{
    paintImageList.clear();
    originalImageList.clear();
    drawPointList.clear();
    scaleList.clear();

    drawImageCount = 0;
    imageSelected = false;

}
