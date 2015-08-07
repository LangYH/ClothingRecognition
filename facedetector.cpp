#include "facedetector.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <QString>

FaceDetector::FaceDetector()
{
}

static void loadFaceDetector( CascadeClassifier &faceDetector, QString path )
{
    try{
        faceDetector.load( path.toLocal8Bit().data() );
    } catch( cv::Exception e ){}


    if( faceDetector.empty() ){
        std::cerr<< "ERROR:counldn't load face detector";
        exit(1);
    }

}

static float prepareImageForFaceDection( const Mat &image, Mat &result )
{
    Mat gray;
    if( image.channels() == 3 )
        cvtColor( image, gray, CV_BGR2GRAY );
    else if( image.channels() == 4 )
        cvtColor( image, gray, CV_BGRA2GRAY );
    else
        gray = image;

    const int DETECTION_WIDTH = 320;
    Mat small_image;
    float scale = gray.cols / (float)DETECTION_WIDTH;
    if( gray.cols > DETECTION_WIDTH ){
        int scaled_height = cvRound( gray.rows / scale );
        cv::resize( gray, small_image, Size( DETECTION_WIDTH, scaled_height ) );
    }
    else{
        small_image = gray;
    }

    equalizeHist( small_image, result );

    return scale;

}

static void detectFaces(const Mat &image, std::vector<Rect> &faces,
                                    QString detector_file_name )
{
    CascadeClassifier faceDetector;
    loadFaceDetector( faceDetector, detector_file_name );
    Mat equalized_image;
    float scale = prepareImageForFaceDection( image, equalized_image );

    int flags = CASCADE_SCALE_IMAGE | CASCADE_FIND_BIGGEST_OBJECT;
    Size minFeatureSize( 20, 20 );
    float searchScaleFactor = 1.1f;
    int minNeighbors = 4;

    faceDetector.detectMultiScale( equalized_image, faces, searchScaleFactor,
                                   minNeighbors, flags, minFeatureSize );

    //resize the result to original size
    const int DETECTION_WIDTH = 320;
    if( image.cols > DETECTION_WIDTH ){
        for( int i = 0; i < (int)faces.size(); i++ ){
            faces[i].x = cvRound( faces[i].x * scale );
            faces[i].y = cvRound( faces[i].y * scale );
            faces[i].width = cvRound( faces[i].width * scale );
            faces[i].height = cvRound( faces[i].height * scale );
        }
    }

    for( int i = 0; i < (int)faces.size(); i++ ){
        if( faces[i].x < 0 )
            faces[i].x = 0;
        if(  faces[i].y < 0 )
            faces[i].y = 0;
        if( faces[i].x + faces[i].width > image.cols )
            faces[i].x = image.cols - faces[i].width;
        if( faces[i].y + faces[i].height > image.rows )
            faces[i].y = image.cols - faces[i].height;
    }

}

void FaceDetector::detectFrontFaces(const Mat &image, std::vector<Rect> &faces )
{
    QString face_cascade_front_file_name = "/home/lang/opencv/opencv-2.4.9/data/"
                                     "haarcascades/haarcascade_frontalface_default.xml";

    detectFaces( image, faces, face_cascade_front_file_name );


}

void FaceDetector::detectProfileFaces(const Mat &image, std::vector<Rect> &faces)
{
    QString face_cascade_profile_file_name = "/home/lang/opencv/opencv-2.4.9/data/"
                                     "haarcascades/haarcascade_profileface.xml";

    detectFaces( image, faces, face_cascade_profile_file_name );

}
