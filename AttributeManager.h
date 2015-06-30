/* 
 * File:   AttributeManager.h
 * Author: msatilmis
 *
 * Created on November 4, 2014, 7:02 PM
 */

#ifndef ATTRIBUTEMANAGER_H
#define	ATTRIBUTEMANAGER_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#include "Attribute.h"

#define fingerLenghtTolerans 70

class AttributeManager {
public:
    AttributeManager(int contourSize);
    void addAttr(Attribute);
    Mat& eliminateAndDraw_Version1(Mat& drawing);
    Mat& eliminateAndDraw(Mat& drawing);
    Mat& findCenterOfHand_Handful(vector<vector<Point> > contours, int index, Mat& drawing);

    vector<Point> fingerList;

private:
    bool checkIsFound(Point& point1);
    bool checkIsNear(Attribute attr);

    double calculateAngle(Point, Point, Point);
    double findDistanceBetweenTwoPoint(Point p1, Point2f p2);

    vector<Attribute> attrList;

    double minBorderAngle, maxBorderAngel;
    int thumbIndex;

    int contourSize;



};
#endif	/* ATTRIBUTEMANAGER_H */

