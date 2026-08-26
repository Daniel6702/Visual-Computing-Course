#include <iostream>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

int main(int, char**)
{
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;
    Mat frame;
    namedWindow("Camera", WINDOW_AUTOSIZE);

    //print size
    cap >> frame;
    std::cout << "Cols: " << frame.cols << ", Rows: " << frame.rows 
        << ", Size: " << frame.size 
        << ", Channels: " << frame.channels() 
        << "\n";

    for(;;)
    {
        cap >> frame;
        //cvtColor(frame, edges, COLOR_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        imshow("Camera", frame);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}