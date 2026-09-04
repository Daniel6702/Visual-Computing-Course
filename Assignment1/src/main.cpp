#include <iostream>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

#include "filters.hpp"

int main(int, char**)
{
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;
    Mat frame;
    namedWindow("Camera", WINDOW_AUTOSIZE);

    int current_image = 0;

    for(;;)
    {
        cap >> frame;

        Mat gray = grayscale(frame);
        Mat blur = gaussian_blur(frame);
        Mat edge = show_edges(frame);

        /*
        //combine the images into a 2x2 grid

        Mat gray_color, edge_color;
        cvtColor(gray, gray_color, COLOR_GRAY2BGR);
        cvtColor(edge, edge_color, COLOR_GRAY2BGR);
        
        Mat top, bottom, combined;
        hconcat(frame, blur, top);
        hconcat(gray_color, edge_color, bottom);
        vconcat(top, bottom, combined);
        */

        Mat images[] = {frame, gray, blur, edge};

        if (waitKey(30) == 'a') {
            current_image += 1;
            if (current_image>3) {
                current_image = 0;
            }
        } 

        imshow("Camera", images[current_image]);

        if(waitKey(30) >= 0) break;
    }
}







    //print size
    /*
    cap >> frame;
    std::cout
        << "Size: " << frame.size 
        << ", Channels: " << frame.channels() 
        << "\n";
              //cvtColor(frame, edges, COLOR_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
    */