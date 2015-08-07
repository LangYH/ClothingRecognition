#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <opencv2/core/core.hpp>
using namespace cv;

class FaceDetector
{
public:
    FaceDetector();
    static void detectFrontFaces(const Mat &image, std::vector<cv::Rect> &faces );
    static void detectProfileFaces( const Mat &image, std::vector<cv::Rect> &faces );
};

#endif // FACEDETECTOR_H
