/* 
 * File:   Attribute.cpp
 * Author: msatilmis
 * 
 * Created on November 2, 2014, 10:38 AM
 */

#include "Attribute.h"

Attribute::Attribute(Point first, Point second, Point frab, int depth) {
   
    startPoint = first;
    endPoint = second;
    frabPoint = frab;
    this->depth = depth;

}

void Attribute::calculateAngle() {

    /*  if (startPoint.y - frabPoint.y != 0)
          slope1 = (double) (startPoint.x - frabPoint.x) / (double) (startPoint.y - frabPoint.y);
      else {
          slope1 = (startPoint.x - frabPoint.x) / 0.1;
      }
      if (endPoint.y - frabPoint.y != 0)
          slope2 = (endPoint.x - frabPoint.x) / (double) (endPoint.y - frabPoint.y);
      else {
          slope2 = (endPoint.x - frabPoint.x) / 0.1;
      }


      double tanValue = slope1 - slope2 / 1 + slope1*slope2;

      double x1 = (float) startPoint.x - frabPoint.x;
      double y1 = (float) startPoint.y - frabPoint.y;
      double x2 = (float) endPoint.x - frabPoint.x;
      double y2 = (float) endPoint.y - frabPoint.y;

      if (x1 != 0.0f)
          slope1 = atan(y1 / x1);
      else
          slope1 = 3.14159 / 2.0; // 90 degrees
      //
      if (x2 != 0.0f)
          slope2 = atan(y2 / x2);
      else
          slope2 = 3.14159 / 2.0; // 90 degrees
      //
      middleAngle = fabs(slope2 - slope1);
      middleAngle = middleAngle * 180.0 / PI;
    

    //  middleAngle = (atan(tanValue)*180 / PI);*/

    double dy = startPoint.y - frabPoint.y;

    double dx = startPoint.x - frabPoint.x;

    double theta1 = atan2(dy, dx);

    slope1 = theta1 * 180 / CV_PI + 180; // rads to degs

    double dy1 = endPoint.y - frabPoint.y;

    double dx1 = endPoint.x - frabPoint.x;

    double theta2 = atan2(dy1, dx1);

    slope2 = theta2 * 180 / CV_PI + 180; // rads to degs

    middleAngle = slope2 - slope1;
    if (middleAngle < 0)
        middleAngle *= -1;

    if(middleAngle > 220)
        middleAngle = 360-middleAngle;

}



