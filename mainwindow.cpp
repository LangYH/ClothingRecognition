#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "clothingsearcher.h"
#include "facedetector.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <QFileDialog>
#include <QImageReader>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ImView->addAction( ui->actionFloodFill );
    ui->ImView->addAction( ui->actionDetect_face );
    ui->ImView->addAction( ui->actionClose_operation );
    ui->ImView->setContextMenuPolicy( Qt::ActionsContextMenu );
    lastPath = "/home/lang/Pictures/Clothes";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    QString filename = QFileDialog::getOpenFileName( this, tr( "Open Image"), lastPath,
                                            tr("Image Files (*.png *.jpg *.jpeg *.bmp *.yaml)"));

    lastPath = QFileInfo( filename ).absolutePath();

    Mat image;

    if( filename.length() == 0)
    {
        return;
    }
    else
    {
        if( QFileInfo(filename).suffix() == "yaml" ){
            FileStorage fs;
            if( fs.open( filename.toLocal8Bit().data(), FileStorage::READ ) ){
                fs[ "depth" ] >> image;
            }
            fs.release();
        }else{
            image = imread( filename.toLocal8Bit().data() );
        }
    }

    if( !image.empty() )
    {
        ui->ImView->setPaintImage(image);
        statusBar()->showMessage( filename + QString( " loaded!") );
    }


}

void MainWindow::on_actionFloodFill_triggered()
{
    if( ui->ImView->isEmpty() )
        return;

    Mat image = ui->ImView->getCurrentImage();
    std::vector<int> floodfill_threshold;
    floodfill_threshold.push_back(ui->ydownScrollBar->value() );
    floodfill_threshold.push_back(ui->yupScrollBar->value() );
    floodfill_threshold.push_back(ui->crdownScrollBar->value() );
    floodfill_threshold.push_back(ui->crupScrollBar->value() );
    floodfill_threshold.push_back(ui->cbdownScrollBar->value() );
    floodfill_threshold.push_back(ui->cbupScrollBar->value() );

    Mat mask;
    ClothingSearcher::getTheMaskOfCloth( image, mask, floodfill_threshold );
    dilate( mask, mask, Mat() );
    erode( mask, mask, Mat() );
    ui->ImView->setPaintImage( mask );
}

void MainWindow::on_actionDetect_face_triggered()
{
    if( ui->ImView->isEmpty() )
        return;

    Mat image = ui->ImView->getCurrentImage();

    std::vector<cv::Rect> faces;
    FaceDetector::detectFrontFaces( image, faces );

    for( int i = 0; i < (int)faces.size(); i++ ){
        rectangle( image, faces[i], Scalar(0,255,0) );
        if( faces.size() != 0){
            Mat tmp  = image.clone();
            cv::Rect rect;
            rect.x = faces[0].x;
            rect.y = faces[0].y + faces[0].height * 1.2;
            rect.width = faces[0].width;
            rect.height = faces[0].height * 1.5;
            rectangle( tmp, rect, Scalar( 0, 255, 0 ) );

            ui->ImView->setPaintImage( tmp );

        }
    }

    if( faces.size() == 0 ){
        std::vector<cv::Rect> profile_faces;
        FaceDetector::detectProfileFaces( image, profile_faces );
        for( int i = 0; i < (int)profile_faces.size(); i++ ){
            rectangle( image, profile_faces[i], Scalar(0,255,0) );
        }
        if( profile_faces.size() != 0){
            Mat tmp  = image.clone();
            cv::Rect rect;
            rect.x = profile_faces[0].x + profile_faces[0].width /3;
            rect.y = profile_faces[0].y + profile_faces[0].height * 1.2;
            rect.width = profile_faces[0].width * 2 / 3;
            rect.height = profile_faces[0].height;
            rectangle( tmp, rect, Scalar( 0, 255, 0 ) );

            ui->ImView->setPaintImage( tmp );
        }
    }

    ui->ImView->setPaintImage( image );
}

void MainWindow::on_actionClose_operation_triggered()
{
   if( ui->ImView->isEmpty() )
       return;
   Mat image = ui->ImView->getCurrentImage();
   dilate( image, image, Mat() );
   erode( image, image, Mat() );

   ui->ImView->setPaintImage( image );
}

void MainWindow::on_action_Save_triggered()
{
    QString filename = QFileDialog::getSaveFileName( this, tr("Save Image"),
                                                     lastPath, tr( "Image Files: (*.jpg *.png *.jpeg *.bmp" ) );
    lastPath = QFileInfo( filename ).absolutePath();

    if( filename.length() == 0 ){
        return;
    }

    if( !ui->ImView->isEmpty() ){
        imwrite( filename.toLocal8Bit().data(), ui->ImView->getCurrentImage() );
    }

}

void getImageList( const QString path, QStringList &image_list )
{
    QDir dir( path );

    QStringList filters;
    foreach( QByteArray format, QImageReader::supportedImageFormats() )
        filters += "*." + format;

    foreach(QString file, dir.entryList(filters, QDir::Files))
        image_list.push_back( path + QDir::separator() + file );

    foreach (QString subDir, dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot ))
        getImageList( path + QDir::separator() + subDir, image_list );

    return;
}

void MainWindow::on_actionBatch_Processing_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this);

    QStringList image_list;
    getImageList( path, image_list );

    std::vector<int> floodfill_threshold;
    floodfill_threshold.push_back(ui->ydownScrollBar->value() );
    floodfill_threshold.push_back(ui->yupScrollBar->value() );
    floodfill_threshold.push_back(ui->crdownScrollBar->value() );
    floodfill_threshold.push_back(ui->crupScrollBar->value() );
    floodfill_threshold.push_back(ui->cbdownScrollBar->value() );
    floodfill_threshold.push_back(ui->cbupScrollBar->value() );


    foreach( QString file, image_list ){
        QFileInfo file_info(file);
        QString out_file = file_info.absolutePath() + QDir::separator() +
                        file_info.completeBaseName() + "_cloth2.jpg";
        Mat image = imread( file.toLocal8Bit().data() );

        Mat mask;
        ClothingSearcher::getTheMaskOfCloth( image, mask, floodfill_threshold );
        dilate( mask, mask, Mat() );
        erode( mask, mask, Mat() );

        cv::normalize( mask, mask, 0, 255, NORM_MINMAX,
                       CV_8UC1);
        imwrite( out_file.toLocal8Bit().data(), mask );

    }

}
