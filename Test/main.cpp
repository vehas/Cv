#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
//#include "image.h"
#include <windows.h>

using namespace cv;
using namespace std;
Mat showCanny(Mat input){
	Mat graySrc, outCanny;


	cvtColor(input, graySrc, CV_BGR2GRAY);
	
	/// set up 
	int min1 = 5,
		max1 = 255,
		step1 =5,
		min2 = 5,
		max2 = 255,
		step2 = 5;

	/// loop 
	int th1 = min1, th2 = 20, key;
	bool loop=true;
	namedWindow("canny", 1);
	createTrackbar("CannyT1", "canny", &th1, max1);
	createTrackbar("CannyT2", "canny", &th2, max2);
	//Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345);
	Mat drawing = Mat::zeros(input.size(), CV_8UC3);

	while (loop){
		
		cv::Canny(graySrc, outCanny, th1, th2);

		//GaussianBlur(outCanny, outCanny, Size(3,3), 0, 0);
		imshow("canny", outCanny);
		
		findContours(outCanny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		/// Draw contours
		drawing = Mat::zeros(input.size(), CV_8UC3);
		for (int i = 0; i< contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		}

		imshow("Contours", drawing);

		
		switch (key = waitKey(30)) {
		case 'w': th1 += step1; cout << th1 << endl; break;
		case 's': if (th1 >= min1){ th1 -= step1; cout << th1 << endl; } break;
		case 'q': th2 += step2; cout << th2 << endl; break;
		case 'a': if (th2 >= min1){ th2 -= min2; cout << th2 << endl; } break;
		case 'o': loop=!loop;
		}
		Sleep(300);

	}
	destroyWindow("canny");
	return outCanny;
}

int main(){
	Mat input_image, graySrc,outCanny,outGauss;
	input_image = imread("c:\\opencv\\paint.jpg"); //path to image

	imshow("Hello OpenCV!", input_image);
	
	cvtColor(input_image, graySrc, CV_BGR2GRAY);

	GaussianBlur(graySrc, outGauss, Size(3,3), 0, 0);
	imshow("GaussianBlur", outGauss);
	
	outCanny =  showCanny(outGauss);
	
	
	waitKey(0);
	return 0;
}

