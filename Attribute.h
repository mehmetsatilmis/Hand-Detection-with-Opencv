/* 
 * File:   Attribute.h
 * Author: msatilmis
 *
 * Created on November 2, 2014, 10:38 AM
 */

#ifndef ATTRIBUTE_H
#define	ATTRIBUTE_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#define PI 3.14159265
#include <sstream>


using namespace cv;
using namespace std;

class Attribute {
public:
    Attribute(Point first, Point second, Point frab,int depth);
    Point startPoint, endPoint, frabPoint;
    double slope1, slope2, middleAngle;
    void calculateAngle();

private:
    int depth;

};



#endif	/* ATTRIBUTE_H */

