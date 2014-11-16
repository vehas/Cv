#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
//#include "image.h"
#include <windows.h>

using namespace cv;
using namespace std;
typedef vector<vector<Point> > contour;
typedef vector<Vec4i> hierarchy;
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
	createTrackbar("cannyT1", "canny", &th1, max1);
	createTrackbar("cannyT2", "canny", &th2, max2);
	//Mat canny_output;
	contour contours;
	hierarchy hierarchy;
	RNG rng(12345);
	Mat drawing = Mat::zeros(input.size(), CV_8UC3);

	while (loop){
		
		cv::Canny(graySrc, outCanny, th1, th1*3);

		GaussianBlur(outCanny, outCanny, Size(3,3), 0, 0);
		imshow("canny", outCanny);
		
		findContours(outCanny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		
		//find outter contour
		int contourNum = contours.size();
		contour big;
		for (int i = 0; i < contourNum; i++){
			if (hierarchy[i][3] < 0)
				big.push_back(contours[i]);
		}

		//end find outter contour


		/// Draw contours
		drawing = Mat::zeros(input.size(), CV_8UC3);
		for (int i = 0; i< big.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, big, i, color, 2, 8, hierarchy, 0, Point());
		}

		imshow("Contours", drawing);

		
		switch (key = waitKey(30)) {
		case 'w': th1 += step1; cout << th1 << endl; break;
		case 's': if (th1 >= min1){ th1 -= step1; cout << th1 << endl; } break;
		case 'q': th2 += step2; cout << th2 << endl; break;
		case 'a': if (th2 >= min1){ th2 -= min2; cout << th2 << endl; } break;
		case 'o': loop=!loop;
		}
		setTrackbarPos("cannyT1", "canny", th1);
		setTrackbarPos("cannyT2", "canny", th2);
		Sleep(300);

	}
	destroyWindow("canny");
	return outCanny;
}



int main(){
	Mat input_image, graySrc,outCanny,outGauss;
	input_image = imread("c:\\opencv\\p1.jpg"); //path to image

	imshow("Hello OpenCV!", input_image);
	
	cvtColor(input_image, graySrc, CV_BGR2GRAY);

	GaussianBlur(graySrc, outGauss, Size(4,4), 0, 0);
	imshow("GaussianBlur", outGauss);
	/*  //  adaptiveThreshold
	Mat outAts;
	adaptiveThreshold(outGauss, outAts, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
		THRESH_BINARY, 11, 2);
		imshow("adaptiveThreshold", outAts);

		*/

	/// Apply the dilation operation
	int dilation_type = MORPH_RECT, dilation_size = 3;
	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	Mat outDilate;
	dilate(outGauss,outDilate, element);
	imshow("Dilation Demo", outDilate);
	// 
	Mat outErode;
	erode(outDilate, outErode, element);
	imshow("Dilation Demo", outErode);

	outCanny =  showCanny(outErode);
	
	
	waitKey(0);
	return 0;
}

