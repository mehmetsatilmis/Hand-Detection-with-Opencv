#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/video/background_segm.hpp>
#include "Attribute.h"
#include "AttributeManager.h"
#include "Tracking.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h> // to calculate time needed
#include <limits.h>

#define PI 3.14159265

using namespace cv;
using namespace std;



Mat src;                /*RGB kamera gonrutusu*/
Mat src_gray;           /*intensity leri içeren goruntu*/
int thresh = 10;        /* threshold value*/
int max_thresh = 255;   
Tracking tracking;      /*Parmak noktalarını program basladıktan bitisine kadar takip eden class*/

RNG rng(12345);


/*ekrana algoritmanın outputu verilemediği zamanlarda threshold basan fonksiyon*/
void cleanScreen(int, void*);

/// Function header
void runAlgorithm(int, void*);
int findBiggestContour(vector<vector<Point> > contours);
void drawRotateRec(int index, vector<vector<Point> > &contours, std::vector<cv::Vec4i>& defects);

/*background ve elin intensity değerlerini gruplayığp threshold değerini bulan fonksiyon*/
void myKmeans(Mat src_my, Mat src_gray_my);


Mat fgMaskMOG; //fg mask generated by MOG method
Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
Ptr<BackgroundSubtractor> pMOG; //MOG Background subtractor
Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor

int flag = 0;

/**
 * @function main
 */
int main(int, char** argv) {
    /// Load source image and convert it to gray

    VideoCapture cap; /* open the default camera*/

    cap.open(1);
    /*daha kücük imageler üzerinde çalışabilmek için*/
    cout << cap.set(CV_CAP_PROP_FRAME_WIDTH, 320) << endl;
    cout << cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240) << endl;
    cout << "   height : " << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << "    weight : " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << endl;

    namedWindow("Hull demo", WINDOW_AUTOSIZE);

    // fps counter begin
    time_t start, end;
    int counter = 0;
    int counterx = 0;
    double sec, pre_sec = 1.0;
    int flag2 = 0;

    for (;;) {
        
        if (counter == 0) {
            time(&start);
        }
        if (!cap.read(src)) {
            break;
        }
        /*initial hazırlıklar*/
        cvtColor(src, src_gray, COLOR_BGR2GRAY);
        blur(src_gray, src_gray, Size(10, 5));

        /*algoritma outputu ekrana verilemiyecekse*/
        if (!flag) {
            cleanScreen(0, 0);
        }

        /// Create Window
        const char* source_window = "Source";
        namedWindow(source_window, WINDOW_AUTOSIZE);

        if (sec == 5 && !flag2) {
            myKmeans(src, src_gray);
            flag2 = 1;
        }
        if (sec >= 5) {
            runAlgorithm(0, 0);
        }

        /*bilgilendirme bilgileri ekrana basıldı.*/
        ostringstream ss;
        ss << "frame " << counterx+1;
        string ben = ss.str();
        const char * text = ben.c_str();
        putText(src, text, Point(30, 50), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2, 6, false);
        if (flag2) {
            ostringstream ss;
            ss << "threshold value: " << thresh;
            string thresV = ss.str();
            const char * text1 = thresV.c_str();
            putText(src, text1, Point(30, 100), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2, 6, false);
        }

        imshow(source_window, src);


        ++counterx;
        time(&end);
        counter++;
        sec = difftime(end, start);

        if (sec != pre_sec) {
            pre_sec = sec;
            cout << "fps ::" << counterx << "\n";
            counterx = 0;
        }


        int k = waitKey(3);
        if (k == 27) {
            break;
        }

    }
    cvDestroyWindow("Hull demo");

    return (0);
}

void myKmeans(Mat src_my, Mat src_gray_my) {
    
    /*kmeans fonksiyonu için hazırlık aşaması*/
    Mat samples(src_my.rows * src_my.cols, 3, CV_32F);
    for (int y = 0; y < src_my.rows; y++)
        for (int x = 0; x < src_my.cols; x++)
            for (int z = 0; z < 3; z++)
                samples.at<float>(y + x * src_my.rows, z) = src_my.at<Vec3b>(y, x)[z];


    int clusterCount = 2; /*2 sınıf istediğimizi söylüyoruz*/
    Mat labels;
    int attempts = 5;
    Mat centers;
    kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);

    /*kmeans sonrası ekrana bastırılacak imageler*/
    Mat new_image(src_my.size(), src_my.type());
    Mat new_img_gry;

    /*kmeans den elde edilen görüntüler image eklendi*/
    for (int y = 0; y < src_my.rows; y++)
        for (int x = 0; x < src_my.cols; x++) {
            int cluster_idx = labels.at<int>(y + x * src_my.rows, 0);
            new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);

            new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
            new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
            
        }

    int rows = new_image.rows;
    int cols = new_image.cols;
    /*image in size ı hesaplandı*/
    if (new_image.isContinuous()) {
        cols = rows * cols; // Loop over all pixels as 1D array.
        rows = 1;
    }

    int type1 = 0, type1Counter = 0, type2 = 0, type2Counter = 0;
    int flag = -1;

    cvtColor(new_image, new_img_gry, CV_RGB2GRAY);

    for (int i = 0; i < cols; ++i) {
        /*new image üzerindeki 2 farklı intensity değeri dolaşılarak bulundu.*/
        int intensity = (int) new_img_gry.at<uchar>(i);

        if (flag == -1) {
            flag = intensity;
        }

        if (flag == intensity) {
            type1Counter += 1;
            /*bu noktalar gerçek görüntüde nereye denk geliyorsa onun üzerinden intensity değrleri alındı*/
            type1 += (int) src_gray_my.at<uchar>(i);

        } else {
            type2 += (int) src_gray_my.at<uchar>(i);
            type2Counter += 1;
        }
    }

    int insThresHold1 = type1 / type1Counter;
    int insThresHold2 = type2 / type2Counter;

    thresh = (insThresHold2 + insThresHold1) / 2;

    cout << "------------ " << thresh << endl;

    imshow("clustered image", new_img_gry);
}

void cleanScreen(int, void*) {
    Mat threshold_output;

    threshold(src_gray, threshold_output, thresh, 255, THRESH_BINARY);
    imshow("Hull demo", threshold_output);
}


void runAlgorithm(int, void*) {
    Mat src_copy = src.clone();
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;


    /// Detect edges using Threshold

    threshold(src_gray, threshold_output, thresh, 255, THRESH_BINARY);
    /// Find contours
    findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    /// Find the convex hull object for each contour

    if (contours.size() > 0) {
        vector<int> hull_I;
        vector<vector<Point> >hull(contours.size());
        std::vector<cv::Vec4i> defects;

        /*Ekrandaki en büyük contour elindir.*/
        int index = findBiggestContour(contours);
        
        int contourSize = contours[index].size();
        /*El üzerinde çalışılmayacak kadar küçük mü*/
        if (contourSize > 250) {
            flag = 1;
            
            /*convex hull ve defectler hesaplandı.*/
            convexHull(contours[index], hull_I, true);
            convexHull(contours[index], hull[index], false);
            cv::convexityDefects(contours[index], hull_I, defects);
            Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);

            /*ekrana çizdirildi*/
            Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            drawContours(drawing, contours, (int) index, color, 1, 8, vector<Vec4i>(), 0, Point());
            drawContours(drawing, hull, (int) index, color, 1, 8, vector<Vec4i>(), 0, Point());


            //drawRotateRec(index, contours, defects);
            
            /*defectlerden alınan her sonuç bir attribute olarak attributemanager a verildi.*/
            AttributeManager mngr(contourSize);

            for (int cDefIt = 0; cDefIt < defects.size(); cDefIt++) {
                int startIdx = defects[cDefIt].val[0];
                int endIdx = defects[cDefIt].val[1];
                int defectPtIdx = defects[cDefIt].val[2];
                double depth = (double) defects[cDefIt].val[3] / 256.0f; // see documentation link below why this


                Point ptStart(contours[index][startIdx]);
                Point ptEnd(contours[index][endIdx]);
                Point ptFar(contours[index][defectPtIdx]);

                Scalar color2 = Scalar(0, 255, 0);

                line(drawing, ptEnd, ptFar, color2, 2, 8, 0);
                line(drawing, ptStart, ptFar, color2, 2, 8, 0);


                Attribute attr(ptStart, ptEnd, ptFar, depth);
                mngr.addAttr(attr);
            }

            /*noktaların parmak olanların algılanması istendi.*/
            drawing = mngr.eliminateAndDraw(drawing);
            /*elin merkezi bulundu*/
            drawing = mngr.findCenterOfHand_Handful(contours, index, drawing);
            /*bu parmak noktaları önceden neredeydi şeklinde bakılarak takip edilmeye çalışıldı.*/
            tracking.identfiyAndNameFinger(drawing, mngr.fingerList, contourSize);

            imshow("Hull demo", drawing);
        } else
            flag = 0;
    }
}

void drawRotateRec(int index, vector<vector<Point> > &contours, std::vector<cv::Vec4i>& defects) {
    /// Draw polygonal contour + bonding rects + circles
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    Mat drawing = Mat::zeros(src_gray.size(), CV_8UC3);


    approxPolyDP(Mat(contours[index]), contours_poly[index], 3, true);
    boundRect[index] = boundingRect(Mat(contours_poly[index]));

    Point_<int> pid = boundRect[index].br();

    Scalar color = Scalar(255, 0, 0);
    drawContours(drawing, contours_poly, index, color, 1, 8, vector<Vec4i>(), 0, Point());
    rectangle(drawing, boundRect[index].tl(), boundRect[index].br(), color, 2, 8, 0);


    imshow("Contours", drawing);


}

int findBiggestContour(vector<vector<Point> > contours) {
    int biggiestIndex = -1;
    int biggiestSize = 0;
    for (int i = 0; i < contours.size(); i++) {
        if (contours[i].size() > biggiestSize) {
            biggiestSize = contours[i].size();
            biggiestIndex = i;
        }
    }
    return biggiestIndex;
}
