#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat img, imgHsv, Mask, Dil;

int main() {
    int hmin = 0, smin = 0, vmin = 153;
    int hmax = 19, smax = 240, vmax = 255;

    namedWindow("Webcam Feed", WINDOW_NORMAL);
    resizeWindow("Webcam Feed", 1280, 720);

    VideoCapture cap(0);
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);

    namedWindow("Trackbars", WINDOW_NORMAL);
    resizeWindow("Trackbars", 640, 200);
    createTrackbar("Hue Min", "Trackbars", &hmin, 179);
    createTrackbar("Hue Max", "Trackbars", &hmax, 179);
    createTrackbar("Sat Min", "Trackbars", &smin, 255);
    createTrackbar("Sat Max", "Trackbars", &smax, 255);
    createTrackbar("Val Min", "Trackbars", &vmin, 255);
    createTrackbar("Val Max", "Trackbars", &vmax, 255);


    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

    while (true) {
        Scalar lower(hmin, smin, vmin);
        Scalar upper(hmax, smax, vmax);
        cap.read(img);
        cvtColor(img, imgHsv, COLOR_BGR2HSV);
        inRange(imgHsv, lower, upper, Mask);
        dilate(Mask, Dil, kernel);

        flip(Mask, Mask, 1);
        flip(Dil, Dil, 1);
        imshow("Webcam Feed", Mask);

        imshow("dil", Dil);
        imshow("mask", imgHsv);
        waitKey(1);
    }

    return 0;
}
