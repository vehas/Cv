#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <vector>
//#include "image.h"
#include <windows.h>

using namespace cv;
using namespace std;
typedef vector<vector<Point> > contour;
typedef vector<Vec4i> hierarchy;

Mat showCanny(Mat input){
	Mat graySrc, outCanny;

	/// guess input is gray 
	graySrc = input;
	//cvtColor(input, graySrc, CV_BGR2GRAY);
	
	/// set up 
	int min1 = 20,
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
	
	

	while (loop){
		
		
		cv::Canny(graySrc, outCanny, th1,th2);
		imshow("canny", outCanny);

		
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

vector<Mat> findPaint(Mat input,Mat real){
	RNG rng(12345);
	Mat graySrc, outCanny;
	// guess  input in gray //else cvtColor(input, graySrc, CV_BGR2GRAY);
	graySrc = input;
	
	///  find edge  with canny & mean theashol
	
	Scalar mu, sigma;
	meanStdDev(graySrc, mu, sigma);
	//cv::Canny(graySrc, outCanny, mu.val[0] - sigma.val[0], mu.val[0] - sigma.val[0]);
	threshold(graySrc, outCanny,mu.val[0], 255, THRESH_BINARY);
	//outCanny = graySrc;
	/*  //  adaptiveThreshold
	Mat outAts;
	adaptiveThreshold(outGauss, outAts, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
	THRESH_BINARY, 11, 2);
	imshow("adaptiveThreshold", outAts);

	*/

	// blur edge for find contour 
	//GaussianBlur(outCanny, outCanny, Size(3, 3), 0, 0);
	
	//find contours
	contour contours;
	hierarchy hierarchy;
	imshow("before", input);
	findContours(outCanny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	imshow("after", input);
	
	//find outter contour
	int contourNum = contours.size();
	contour big;
	for (int i = 0; i < contourNum; i++){
		if (hierarchy[i][3] < 0)
			big.push_back(contours[i]);
	}
	Mat drawingContour = Mat::zeros(input.size(), CV_8UC3);
	for (int i = 0; i< big.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawingContour, big, i, color, 2, 8, hierarchy, 0, Point());
	}


	// find Rect
	vector<Rect> minRect(big.size());
	for (int i = 0; i < big.size(); i++)
	{
		minRect[i] = boundingRect(Mat(big[i]));

	}

	// select  large  enought aera 
	//  more than  20% of imaage
	float scaleimageSize = input.size().area()*0.1*0.1, max = 0;
	vector<Rect> BigMinRect;
	cout << " scaleimageSize " << scaleimageSize << endl;
	for (int i = 0; i < minRect.size(); i++)
	{
		if (max < minRect[i].area()) max = minRect[i].area();
		if (minRect[i].area() > scaleimageSize)
			BigMinRect.push_back(minRect[i]);
	}
	

	/// draw each image  ,  pussh it to vector img
	
	Mat drawing = Mat::zeros(input.size(), CV_8UC3);
	vector<Mat> img;
	img.reserve(minRect.size());
		for (int i = 0; i< BigMinRect.size(); i++)
	    {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		rectangle(drawing,BigMinRect[i],color);

		
		Mat roi = real(BigMinRect[i]);
		Mat last;
		roi.copyTo(last);
		img.push_back(last);
		}
	
	return img;
	waitKey(0);
	}
Mat  prepare(Mat input){
	///  convert to gray 
	Mat graySrc;
	cvtColor(input, graySrc, CV_BGR2GRAY);
	Mat Lapa;
	Laplacian(graySrc, Lapa, CV_8UC1);




	///  blur it
	Mat outGauss;
	GaussianBlur(Lapa, outGauss, Size(5, 5), 0, 0);
	
	///   prepare dilate & erode
	int dilation_type = MORPH_RECT,
		dilation_size = 3;
	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	
	/// Apply the dilation 
	Mat outDilate;
	dilate(outGauss, outDilate, element);
	
	/// apply Erode 
	Mat outErode;
	erode(outDilate, outErode, element);
	


	return outErode;

}
vector<Mat> realimg(vector<Mat> input){


	return; 
}

int main(){
	
	Mat input_image, outPrepare;
	vector<Mat> data;
	
	input_image = imread("D:\\$$333333_comPADA\\CV\\ImgProject\\in\\test1.jpg"); //path to image

    //outCanny =  showCanny(outErode);
	//outPrepare = prepare(input_image);
	outPrepare = prepare(input_image);
	data = findPaint(outPrepare,input_image);
	for (int i = 0; i < data.size(); i++){
		imwrite("D:\\$$333333_comPADA\\CV\\ImgProject\\test\\out10_"+to_string(i)+".jpg", data[i]);
	
	}

	
	
	/////////////////////////////////////////////////////
/*	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;
	int key;
	int count=0;
	bool loop = true ,inv = true;
	while (loop) {
		Mat frame;
		cap >> frame;
		//input_image = imread("D:\\$$333333_comPADA\\CV\\ImgProject\\in\\6.jpg"); //path to image

		///outCanny =  showCanny(outErode);
		//outPrepare = prepare(input_image);
		outPrepare = prepare(frame);

		data = findPaint(outPrepare);
		imwrite("D:\\$$333333_comPADA\\CV\\ImgProject\\out" + to_string(count) + ".jpg", data[0]); count++;

		switch (key = waitKey(30)) {
		case 27: loop = false; break;
		case 67: case 99: imwrite("out" + to_string(count) + ".jpg", data); count++; break;
		case 'i': inv = !inv;

		}
	}*/
	
	waitKey(0);
	return 0;
}

