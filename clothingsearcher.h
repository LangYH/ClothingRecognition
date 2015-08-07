#ifndef CLOTHINGSEARCHER_H
#define CLOTHINGSEARCHER_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class ClothingSearcher
{
public:
    ClothingSearcher();
    static void getTheMaskOfCloth(const Mat &image , Mat &mask, const std::vector<int> floodfill_thresholds);
};

#endif // CLOTHINGSEARCHER_H
