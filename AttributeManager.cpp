/* 
 * File:   AttributeManager.cpp
 * Author: msatilmis
 * 
 * Created on November 4, 2014, 7:02 PM
 */

#include "AttributeManager.h"

AttributeManager::AttributeManager(int cs) : contourSize(cs) {
    minBorderAngle = 10;
    maxBorderAngel = 50;

    thumbIndex = -1;
}

void AttributeManager::addAttr(Attribute attr) {
    if (!checkIsNear(attr)) {
        attr.calculateAngle();
        attrList.push_back(attr);
    }
}

Mat& AttributeManager::findCenterOfHand_Handful(vector<vector<Point> > contours, int index, Mat& drawing) {
    vector<Moments> mu;

    mu.push_back(moments(contours[index], false));

    ///  Get the mass centers:
    vector<Point2f> mc;

    mc.push_back(Point2f(mu[0].m10 / mu[0].m00, mu[0].m01 / mu[0].m00));


    vector<Attribute>::iterator itr = attrList.begin();

    double minDis = -1;

    for (; itr != attrList.end(); ++itr) {
        double returnValue = findDistanceBetweenTwoPoint(itr->frabPoint, mc[0]);
        if (minDis == -1) {
            minDis = returnValue;
        } else if (returnValue < minDis)
            minDis = returnValue;
    }

    Scalar color = Scalar(255, 0, 0);
    circle(drawing, mc[0], minDis, color, 2, 8, 0);

    return drawing;
}



Mat& AttributeManager::eliminateAndDraw(Mat& drawing) {

    vector<Attribute>::iterator itr = attrList.begin();
    vector<Attribute> secondChance;
    vector<int> indexList;

    double temp;

    Scalar color = Scalar(0, 0, 255);
    Scalar color1 = Scalar(0, 255, 0);
    Scalar color2 = Scalar(255, 0, 0);

    int index = 0;

    for (; itr != attrList.end(); ++itr, ++index) {
        temp = itr->middleAngle;
        //
        //        circle(drawing, itr->frabPoint, 10, color2, 2, 8, 0);
        //        circle(drawing, itr->startPoint, 10, color1, 2, 8, 0);

        if (temp >= minBorderAngle && temp <= maxBorderAngel && findDistanceBetweenTwoPoint(itr->startPoint, itr->frabPoint) > fingerLenghtTolerans) {
            ostringstream ss;
            ss << "angle " << temp;
            string ben = ss.str();
            const char * text = ben.c_str();
            putText(drawing, text, itr->frabPoint, FONT_HERSHEY_SIMPLEX, 0.5, color2, 2, 6, false);


            if (!checkIsFound(itr->startPoint)) {
                circle(drawing, itr->startPoint, 10, color, 2, 8, 0);
                circle(drawing, itr->frabPoint, 10, color2, 2, 8, 0);
                fingerList.push_back(itr->startPoint);
            }
            if (!checkIsFound(itr->endPoint)) {
                circle(drawing, itr->endPoint, 10, color, 2, 8, 0);
                circle(drawing, itr->frabPoint, 10, color2, 2, 8, 0);

                fingerList.push_back(itr->endPoint);

            }

        } else {
            secondChance.push_back(*itr);
            indexList.push_back(index);
        }
    }

    vector<Attribute>::iterator itr1 = secondChance.begin();
    index = 0;
    int flagmy = 0;
    for (; itr1 != secondChance.end(); ++itr1, ++index) {

        if (!checkIsFound(itr1->startPoint)) {
            flagmy = 1;
            int i = indexList[index];
            int other_i_1, other_i_2;
            if (i + 1 >= attrList.size())
                other_i_1 = 0;
            else
                other_i_1 = i + 1;

            if (i - 1 <= 0)
                other_i_2 = attrList.size() - 1;
            else
                other_i_2 = i - 1;


            double angle = calculateAngle(itr1->startPoint, attrList[i].frabPoint, attrList[other_i_2].frabPoint);

            if (angle >= 5 && angle <= 40 && findDistanceBetweenTwoPoint(itr1->startPoint, itr1->frabPoint) > fingerLenghtTolerans) {
                ostringstream ss;
                ss << "angle " << angle;
                string ben = ss.str();
                const char * text = ben.c_str();
                putText(drawing, text, itr1->startPoint, FONT_HERSHEY_SIMPLEX, 0.5, color1, 2, 8, false);

                circle(drawing, attrList[i].frabPoint, 10, color1, 2, 8, 0);
                circle(drawing, attrList[other_i_2].frabPoint, 10, color1, 2, 8, 0);


                circle(drawing, itr1->startPoint, 10, color, 2, 8, 0);
                fingerList.push_back(itr1->startPoint);
                imshow("Hull demo", drawing);

            }


        }

    }
    return drawing;

}


double AttributeManager::calculateAngle(Point startPoint, Point frabPoint1, Point frabPoint2) {
    double dy = startPoint.y - frabPoint1.y;

    double dx = startPoint.x - frabPoint1.x;

    double theta1 = atan2(dy, dx);

    double slope1 = theta1 * 180 / CV_PI + 180; // rads to degs

    double dy1 = startPoint.y - frabPoint2.y;

    double dx1 = startPoint.x - frabPoint2.x;

    double theta2 = atan2(dy1, dx1);

    double slope2 = theta2 * 180 / CV_PI + 180; // rads to degs

    double returnValue = slope2 - slope1;
    if (returnValue < 0)
        returnValue *= -1;

    if (returnValue > 220)
        returnValue = 360 - returnValue;

    return returnValue;
}

double AttributeManager::findDistanceBetweenTwoPoint(Point p1, Point2f p2) {
    return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}

bool AttributeManager::checkIsFound(Point & point1) {

    for (int i = 0; i < fingerList.size(); ++i) {

        if (fingerList[i].x + contourSize / 10 >= point1.x && fingerList[i].x - contourSize / 10 <= point1.x)
            if (fingerList[i].y + contourSize / 20 >= point1.y && fingerList[i].y - contourSize / 20 <= point1.y)
                return true;
    }

    return false;

}

bool AttributeManager::checkIsNear(Attribute attr) {

    if (attr.startPoint.x + 30 >= attr.frabPoint.x && attr.startPoint.x - 30 <= attr.frabPoint.x)
        if (attr.startPoint.y + 10 >= attr.frabPoint.y && attr.startPoint.y - 10 <= attr.frabPoint.y)
            return true;

    if (attr.startPoint.x + 30 >= attr.endPoint.x && attr.startPoint.x - 30 <= attr.endPoint.x)
        if (attr.startPoint.y + 10 >= attr.endPoint.y && attr.startPoint.y - 10 <= attr.endPoint.y)
            return true;

    if (attr.frabPoint.x + 30 >= attr.endPoint.x && attr.frabPoint.x - 30 <= attr.endPoint.x)
        if (attr.frabPoint.y + 10 >= attr.endPoint.y && attr.frabPoint.y - 10 <= attr.endPoint.y)
            return true;

}

