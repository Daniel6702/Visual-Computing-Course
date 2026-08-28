#include <iostream>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;


Mat grayscale(Mat input) {

    Mat gray(input.rows, input.cols, 0); //New empty image, same size, 0 is the type (8 bit i.e. 1 channel)

    for (int row = 0; row < input.rows; row++) {
        for (int col = 0; col < input.cols; col++) {

            //Get pixel values at each position
            Vec3b pixel = input.at<Vec3b>(row,col);

            //compute average
            int avg = (pixel[0] + pixel[1] + pixel[2]) / 3;

            //add avg to new image 
            gray.at<int8_t>(row, col) = avg;

        }
    }

    return gray;
}


Mat apply_kernel2D(Mat input, Mat kernel2D) {
    int rows = input.rows;
    int cols = input.cols;
    int channels = input.channels();

    int krows = kernel2D.rows;
    int kcols = kernel2D.cols;

    int type = (channels == 1) ? 0 : 16; // grayscale or color
    Mat result(rows-krows, cols-kcols, type);

    for (int row = krows/2; row < rows-krows/2; row++) {
        for (int col = kcols/2; col < cols-kcols/2; col++) {

            if (channels == 1) {
                float new_value = 0;

                for (int i = -krows/2; i <= krows/2; i++) {
                    for (int j = -kcols/2; j <= kcols/2; j++) {

                        float kernel_val = kernel2D.at<float>(i + krows/2, j + kcols/2);

                        int8_t pixel_val = input.at<int8_t>(row+i, col+j);

                        new_value += kernel_val * pixel_val;
                    }
                }

                result.at<int8_t>(row-1, col-1) = new_value;
            }
        }
    }

    return result;
}

            //Vec3b new_pixel;
            /*
            for (int chn = 0; chn < channels; chn++) {

                for (int i = -krows/2; i < krows/2; i++) {
                    for (int j = -kcols/2; j < kcols/2; j++) {

                        float kernel_val = kernel2D.at<float>(i,j);
                        Vec3b pixel = input.at<Vec3b>(row+i,col+j);

                        //

                    }
            }
            }
            */


int main(int, char**)
{
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;
    Mat frame;
    namedWindow("Camera", WINDOW_AUTOSIZE);

    //print size
    cap >> frame;
    std::cout
        << "Size: " << frame.size 
        << ", Channels: " << frame.channels() 
        << "\n";

    for(;;)
    {
        cap >> frame;
        Mat gray = grayscale(frame);
        
        //cvtColor(frame, edges, COLOR_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        imshow("Camera", gray);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}