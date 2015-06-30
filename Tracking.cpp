/* 
 * File:   Tracking.cpp
 * Author: msatilmis
 * 
 * Created on November 5, 2014, 7:48 PM
 */

#include "Tracking.h"

Tracking::Tracking() {
    initialFlag = 1;

    fingerTrackList.push_back(FingerTrackObject(0));
    fingerTrackList.push_back(FingerTrackObject(1));
    fingerTrackList.push_back(FingerTrackObject(2));
    fingerTrackList.push_back(FingerTrackObject(3));
    fingerTrackList.push_back(FingerTrackObject(4));

}

Mat& Tracking::identfiyAndNameFinger(Mat& drawing, vector<Point>& fl, int contourSize) {
    double minDis = -1;
    int minIndex = -1;

    vector<int> notMatchingFromFl_Index;

    for (int i = 0; i < fl.size(); ++i) {
        for (int j = 0; j < 5; ++j) {
            if (fingerTrackList[j].isHavePrePoint != 0) {
                double rv = findDistanceBetweenTwoPoint(fl[i], fingerTrackList[j].prePoint);

                if (minDis == -1) {
                    minDis = rv;
                    minIndex = j;

                } else if (rv < minDis) {
                    minDis = rv;
                    minIndex = j;
                }
            }
        }
        if (minIndex != -1 && minDis < contourSize / 20) {
            fingerTrackList[minIndex].isHaveCurrentPoint = 1;
            fingerTrackList[minIndex].currentPoint = fl[i];
        } else {
            notMatchingFromFl_Index.push_back(i);
        }
        minIndex = -1;
        minDis = -1;

    }

    int j = 0;
    for (int i = 0; i < fingerTrackList.size(); ++i) {
        if (!fingerTrackList[i].isHaveCurrentPoint) {
            if (j < notMatchingFromFl_Index.size()) {
                fingerTrackList[i].isHaveCurrentPoint = 1;
                fingerTrackList[i].currentPoint = fl[notMatchingFromFl_Index[j]];
                ++j;
            }

        }
    }

    for (int i = 0; i < fingerTrackList.size(); ++i) {
        if (fingerTrackList[i].isHaveCurrentPoint) {
            fingerTrackList[i].frameCounter = 0;
            ostringstream ss;

            ss << "id " << fingerTrackList[i].fingerId;
            string ben = ss.str();
            const char * text = ben.c_str();
            putText(drawing, text, fingerTrackList[i].prePoint, FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 100, 100), 2, 8, false);
        }else{
            fingerTrackList[i].frameCounter+=1;
            if(fingerTrackList[i].frameCounter == 3){
                fingerTrackList[i].frameCounter=0;
                fingerTrackList[i].isHavePrePoint = 0;
            }
        }
        
        fingerTrackList[i].isHaveCurrentPoint = 0;
        fingerTrackList[i].isHavePrePoint = 1;
        fingerTrackList[i].prePoint = fingerTrackList[i].currentPoint;
    }
}


double Tracking::findDistanceBetweenTwoPoint(Point p1, Point2f p2) {
    return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}

FingerTrackObject::FingerTrackObject(int id) {
    this->fingerId = id;
    isHavePrePoint = 0;
    isHaveCurrentPoint = 0;
    frameCounter=0;
}
