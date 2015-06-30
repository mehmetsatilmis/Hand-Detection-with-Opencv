/* 
 * File:   Tracking.h
 * Author: msatilmis
 *
 * Created on November 5, 2014, 7:48 PM
 */

#ifndef TRACKING_H
#define	TRACKING_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

class FingerTrackObject{
   
public: 
    FingerTrackObject(int id);
    
    int isHavePrePoint;
    int isHaveCurrentPoint;
    int fingerId;
    int frameCounter;
    Point prePoint;
    Point currentPoint;
    
};

class Tracking {
public:
    Tracking();

    Mat& identfiyAndNameFinger(Mat& drawing, vector<Point>& fl, int contourSize);    
    
private:
    int initialFlag;
   
    vector<FingerTrackObject> fingerTrackList;
    
    double findDistanceBetweenTwoPoint(Point p1, Point2f p2);
};



#endif	/* TRACKING_H */

