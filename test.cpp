
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

Mat img, imgHsv, Mask, Dil, imgGray, Blendimg, overlayImg, imgClone;
int timeToVirtualDraw = 0;
vector<Scalar> myColorValues{ {255, 0, 255},		// Purple
 { 0,255,0 } };// Green 	
vector<vector<int>> PointsDrawn;  // to store all points
bool drawingEn;

int alpha = 50;  // Initial alpha value for blending
int gammaValue = 0;  // Initial gamma value for brightness adjustment


vector<int> lastPoint;



// Trackbar callback function in blending
void onTrackbar(int, void*) {
    addWeighted(imgClone, (double)alpha/100.0, overlayImg, 1 - ((double)alpha/100.0),  gammaValue, Blendimg);
    //imshow("blend", Blendimg);
    
}


void getContours(Mat image) {


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;


    findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    //drawContours(img, contours, -1, Scalar(0, 0, 255), 2);

    //drawContours(img, contours, -1, Scalar(, 0, 255), 2);
    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());
    int largestContourIndex = -1;
    int largestContourArea = 0;
    for (int i = 0; i < contours.size(); i++)
    {
        int area = contourArea(contours[i]);
        


        if (area > 2500 and area<22000)
        {
           // cout << area << endl;
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.03 * peri, true);
            drawContours(img, contours, -1, Scalar(0, 0, 255), 2);

            boundRect[i] = boundingRect(conPoly[i]);

            int objCor = (int)conPoly[i].size();
          //  cout << objCor << endl;

            if (area > largestContourArea) {
                largestContourArea = area;
                largestContourIndex = i;
            }

        }
    }

    if (largestContourIndex != -1) {
        // Moments m = moments(contours[largestContourIndex]);
        int cx = boundRect[largestContourIndex].x + boundRect[largestContourIndex].width / 2;
        int cy = boundRect[largestContourIndex].y;

         Point tipPosition = Point(cx, cy); //contours[largestContourIndex].back();

         

        // Update pen position
        Point penPosition = tipPosition;

        // Draw a circle (point) at the pen position
        circle(img, Point(penPosition.x, penPosition.y ), 15, myColorValues[0], FILLED);
        circle(imgClone, Point(penPosition.x, penPosition.y), 15, myColorValues[0], FILLED);
        cout << drawingEn;
        if (drawingEn) {


            PointsDrawn.push_back({ penPosition.x, penPosition.y,1 });

            lastPoint = { penPosition.x, penPosition.y };
            cout << timeToVirtualDraw << endl;
            if (1) {
                if (PointsDrawn.size() > 1) {
                    lastPoint = PointsDrawn[PointsDrawn.size() - 2];
                }
                else {
                    lastPoint = { penPosition.x + 4, penPosition.y + 4 };
                    PointsDrawn.push_back({ penPosition.x+4, penPosition.y+4,1 });
                }


                double m = ((double)lastPoint[1] - (double)penPosition.y) / ((double)lastPoint[0] - (double)penPosition.x);
                double gap = (-(double)lastPoint[0] + (double)penPosition.x) / 4;
                int gapInt = (int)gap;
                int virtualX = lastPoint[0];
                int virtualY = lastPoint[1];
                // PointsDrawn.push_back({ virtualX+1, virtualY+1 , 0 });
                if (virtualY > 15 and virtualX>20 and abs(gap)<15 ){
                    for (int i = 0; i < 3; i++) {
                        cout << "work intp " + virtualX;
                        virtualX = static_cast<int>(virtualX + std::ceil(gap));
                        virtualY = static_cast<int>(virtualY + std::ceil(gap * m));
                        

                        PointsDrawn.push_back({ virtualX, virtualY , 1 });
                    }

                }
                


            }
            
        }
        
        
    }
}


void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
{

    for (int i = 3; i < newPoints.size(); i++)
    {
        int flippedX = 1280 - newPoints[i][0];
        circle(img, Point(flippedX, newPoints[i][1]), 17, myColorValues[newPoints[i][2]], FILLED);
        circle(overlayImg, Point(flippedX, newPoints[i][1]), 17, myColorValues[newPoints[i][2]], FILLED);
       
        
    }
}


int main() {
    int hmin = 0, smin = 0, vmin = 0;
    int hmax = 151, smax = 255, vmax = 255;

    

    namedWindow("Webcam Feed", WINDOW_NORMAL);
    resizeWindow("Webcam Feed", 1280, 720);

    VideoCapture cap(0);
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);

    overlayImg = imread("b.jpg");
    

    namedWindow("Trackbars", WINDOW_NORMAL);
    resizeWindow("Trackbars", 640, 200);
    createTrackbar("Hue Min", "Trackbars", &hmin, 179);
    createTrackbar("Hue Max", "Trackbars", &hmax, 179);
    createTrackbar("Sat Min", "Trackbars", &smin, 255);
    createTrackbar("Sat Max", "Trackbars", &smax, 255);
    createTrackbar("Val Min", "Trackbars", &vmin, 255);
    createTrackbar("Val Max", "Trackbars", &vmax, 255);


    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    cap.read(img);
    cap.read(imgClone);
    
    resize(overlayImg, overlayImg, img.size());

    //for drawing contourd
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    // Create a window for the blended image
    namedWindow("Blended Image");

    // Create trackbars for alpha and gamma adjustment
    createTrackbar("Alpha", "Blended Image", &alpha, 100, onTrackbar);
    createTrackbar("Gamma", "Blended Image", &gammaValue, 100, onTrackbar);
    



    while (true) {
        Scalar lower(hmin, smin, vmin);
        Scalar upper(hmax, smax, vmax);

        int key = waitKey(1);
       

        if (key == 27) {
            PointsDrawn.clear();
            overlayImg = imread("b.jpg");
            resize(overlayImg, overlayImg, img.size());

        }
       
        
        if (key == 32) {
            drawingEn = true;
           
        }
        if (key == 'n') {
            drawingEn = false;
       }
        


        cap.read(img);
       cap.read(imgClone);
        //imgClone = img.clone();
       // resize(overlayImg, overlayImg, img.size());
        cvtColor(img, imgHsv, COLOR_BGR2HSV);
        inRange(imgHsv, lower, upper, Mask);
        dilate(Mask, Dil, kernel);

        //flip(Mask, Mask, 1);
        //flip(Dil, Dil, 1);
        
        //imshow("Webcam Feed", Mask);

        imshow("dil", Dil);
        imshow("mask", imgHsv);

        cvtColor(imgHsv, imgGray, cv::COLOR_BGR2GRAY);

        

        //draw contours
        getContours(Mask);
        flip(img, img, 1);
        flip(imgClone, imgClone, 1);
       
        


        drawOnCanvas(PointsDrawn, myColorValues);
        onTrackbar(0, nullptr);
        
        imshow("origi", img);
        imshow("blend", Blendimg);
        waitKey(1);

        
       
    }

    return 0;
}
