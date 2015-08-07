#include "clothingsearcher.h"
#include <opencv2/highgui/highgui.hpp>
#include "facedetector.h"

ClothingSearcher::ClothingSearcher()
{
}

static void computeEdgeMap( const Mat &image, Mat &edges )
{
    //get edge map
    Mat gray;
    cvtColor( image, gray, CV_BGR2GRAY );
    const int MEDIAN_BLUR_FILTER_SIZE = 7;
    medianBlur( gray, gray, MEDIAN_BLUR_FILTER_SIZE );

    const int LAPLACIAN_FILTER_SIZE = 5;
    Laplacian( gray, edges, CV_8U, LAPLACIAN_FILTER_SIZE );
    //***************

}

static void closeOperation( Mat &edges )
{
    //close operation, close the gap of edge map
    const int EDGES_THRESHOLD = 90;
    threshold( edges, edges, EDGES_THRESHOLD, 255, THRESH_BINARY );
    dilate( edges, edges, Mat() );
    erode( edges, edges, Mat() );

}

static void drawSeedsOnImage( Mat &small_image, std::vector<cv::Point> clothPts )
{
    for( int i = 0; i < (int)clothPts.size(); i++ )
        circle( small_image, clothPts[i], 3, Scalar( 0, 0, 255 ) );
    //imshow( "seeds", small_image);
    waitKey(0);
}

static void prepareSeedsRegionForFloodfill( const Mat &image,
                                      cv::Rect &rect)
{
    std::vector<cv::Rect> faces;
    FaceDetector::detectFrontFaces( image, faces );

    int sw = image.cols;
    int sh = image.rows;

    if( faces.size() != 0 ){
            rect.x = faces[0].x;
            rect.y = faces[0].y + faces[0].height * 1.5;
            rect.width = faces[0].width;
            rect.height = faces[0].height * 1.5;
    }


    if( faces.size() == 0 ){
        FaceDetector::detectProfileFaces( image, faces );
        if( faces.size() != 0 ){
            rect.x = faces[0].x + faces[0].width /3;
            rect.y = faces[0].y + faces[0].height * 1.2;
            rect.width = faces[0].width * 2 / 3;
            rect.height = faces[0].height;
        }
        else{
            rect.x = sw / 2 - sw / 12;
            rect.y = sh / 2 - sh / 5;
            rect.width = sw / 6;
            rect.height = sh / 3;
        }

    }


    //make sure the seeds are in bound of image
    if( rect.x + rect.width > image.cols )
        rect.width = image.cols - rect.x;
    if( rect.y + rect.height > image.rows )
        rect.height = image.rows - rect.y;

}

void ClothingSearcher::getTheMaskOfCloth(const Mat &image, Mat &mask,
                                    const std::vector<int> floodfill_thresholds )
{
    if( image.channels() == 1 )
        return;
    //get the mask of cloth region using floodfill algorithm
    Mat edges;
    computeEdgeMap( image, edges );


    //to lower down the complexity, resize the original image to smaller one
    Mat small_image;
    Size small_size( image.cols, image.rows );
    if( image.cols > 320 ){
        float scale = image.cols / 320;
        small_size.width = image.cols / scale;
        small_size.height = image.rows / scale;
        cv::resize( image, small_image, small_size );
    }
    else
        small_image = image.clone();


    //convert to yuv color space
    Mat yuv = Mat( small_size, CV_8UC3 );
    cvtColor( small_image, yuv, CV_BGR2RGB );

    //the mask used in floodfill should be 2 pixels widther and higher than the image
    int sw = small_size.width;
    int sh = small_size.height;
    Mat mask_plus_border;
    mask_plus_border = Mat::zeros( sh + 2, sw + 2, CV_8UC1 );
    mask = mask_plus_border( cv::Rect( 1, 1, sw, sh ) );
    cv::resize( edges, mask, small_size );

    closeOperation( mask );

    cv::Rect rect;
    prepareSeedsRegionForFloodfill( small_image, rect );

    rectangle( small_image, rect, Scalar( 0, 255, 0 ) );

    //imshow( "tets", small_image );
    waitKey(0);


    Scalar lowerDiff = Scalar( floodfill_thresholds[0], floodfill_thresholds[0], floodfill_thresholds[0] );
    Scalar upperDiff = Scalar( floodfill_thresholds[0], floodfill_thresholds[0], floodfill_thresholds[0]);
    const int CONNECTED_COMPONENTS = 4;
    const int flags = CONNECTED_COMPONENTS |  FLOODFILL_FIXED_RANGE | FLOODFILL_MASK_ONLY;
    Mat edge_mask = mask.clone();

    const int X_POINTS = 8;
    const int Y_POINTS = 8;
    const int X_STEP = rect.width / X_POINTS;
    const int Y_STEP = rect.height / Y_POINTS;
    for( int i = rect.x + X_STEP; i < (int)(rect.x + rect.width ); i += X_STEP ){
        for( int j = rect.y + Y_STEP; j < (int)(rect.y + rect.height ); j += Y_STEP ){
            circle( small_image, cv::Point( i, j), 3, Scalar( 0, 0, 255 ) );
            floodFill( yuv, mask_plus_border, cv::Point( i, j), Scalar(), NULL,
                       lowerDiff, upperDiff, flags );
        }
    }

    //imshow( "mask", small_image );
    waitKey(0);
    mask -= edge_mask;

}

