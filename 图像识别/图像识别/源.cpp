#include<opencv.hpp>  //opencv1
#include<cv.h>		 //opencv2
#include<highgui.h> //opencv3
#include<vector>	//阵列
#include<iostream> //Cpp
#include"loc.h"//参数
#include <omp.h>//多线程
#include <fstream>  //读取txt
#include <string>	//string
#include <windows.h>//sleep

using namespace cv;
using namespace std;

Mat CheckColor(Mat &inImg);	//颜色判断
void DrawFire(Mat &inputImg, Mat foreImg);	//输出图形
void on_mouse(int event, int x, int y, int flags, void* img);
void txtout(int Fx, int Fy);//输出TXT



int main()
{
	VideoCapture cap("F.avi");
	//VideoCapture cap(0);
	while (1)
	{
		Mat frame;

		cap >> frame;
		if (frame.empty())
			break;
		namedWindow("Control", CV_WINDOW_AUTOSIZE);

		cvCreateTrackbar("redThre", "Control", &redThre, 255);
		cvCreateTrackbar("saturationTh", "Control", &saturationTh, 255);
		CheckColor(frame);	//延时问题
		waitKey(1);
	}
	return 0;
}
void on_mouse(int event, int x, int y, int flags, void* img)
{
	CvPoint pt = cvPoint(x, y);
	char temp[16];
	//std::cout << "x:" << x << "       " << "y:" << y << std::endl;

}
//The Color Check is According to "An Early Fire-Detection Method Based on Image Processing"    
//The Author is:Thou-Ho (Chao-Ho) Chen, Ping-Hsueh Wu, and Yung-Chuen Chiou    

Mat CheckColor(Mat &inImg)
{
	Mat fireImg;
	fireImg.create(inImg.size(), CV_8UC1);
	Mat multiRGB[3];
	int a = inImg.channels();
	split(inImg, multiRGB); //将图片拆分成R,G,B,三通道的颜色    

	for (int i = 0; i < inImg.rows; i++)
	{
		//#pragma omp parallel for
		for (int j = 0; j < inImg.cols; j++)
		{
			
			float B, G, R;
			B = multiRGB[0].at<uchar>(i, j); //每个像素的R,G,B值,动态地址计算法    
			G = multiRGB[1].at<uchar>(i, j);
			R = multiRGB[2].at<uchar>(i, j);

			float maxValue = max(max(B, G), R);
			float minValue = min(min(B, G), R);
			//与HSI中S分量的计算公式  
			double S = (1 - 3.0*minValue / (R + G + B));//  

														//R > RT  R>=G>=B  S>=((255-R)*ST/RT)    
			if (R > redThre &&R >= G && G >= B && S > ((255 - R) * saturationTh / redThre))
			{
				fireImg.at<uchar>(i, j) = 255;
			}
			else
			{
				fireImg.at<uchar>(i, j) = 0;
			}
		}
	}

	erode(fireImg, fireImg, Mat(3, 3, CV_8UC1));
	medianBlur(fireImg, fireImg, 5);
	dilate(fireImg, fireImg, Mat(5, 5, CV_8UC1));
	imshow("Binary", fireImg);	cvSetMouseCallback("Binary", on_mouse);

	DrawFire(inImg, fireImg);
	return fireImg;
}

void DrawFire(Mat &inputImg, Mat foreImg)
{
	std::vector<std::vector<Point>> contours_set;//保存轮廓提取后的点集及拓扑关系    
	findContours(foreImg, contours_set, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Point point1;
	Point point2;
	float a = 0.4, b = 0.75;//???
	float xmin1 = a * inputImg.cols, ymin1 = inputImg.rows, xmax1 = 0, ymax1 = 0;
	float xmin2 = b * inputImg.cols, ymin2 = inputImg.rows, xmax2 = a * inputImg.cols, ymax2 = 0;
	float xmin3 = inputImg.cols, ymin3 = inputImg.rows, xmax3 = b * inputImg.cols, ymax3 = 0;
	Rect finalRect1;
	Rect finalRect2;
	Rect finalRect3;
	std::vector<std::vector<Point> >::iterator iter = contours_set.begin();

	for (; iter != contours_set.end();)
	{

		Rect rect = boundingRect(*iter);
		float radius;
		Point2f center;
		minEnclosingCircle(*iter, center, radius);

		if (rect.area() > 0)
		{
			point1.x = rect.x;
			point1.y = rect.y;
			point2.x = point1.x + rect.width;
			point2.y = point1.y + rect.height;

			if (point2.x < a*inputImg.cols)
			{
				if (point1.x < xmin1)
					xmin1 = point1.x;
				if (point1.y < ymin1)
					ymin1 = point1.y;
				if (point2.x > xmax1 && point2.x < xmax2)
					xmax1 = point2.x;
				if (point2.y > ymax1)
					ymax1 = point2.y;
			}

			if (point2.x < b*inputImg.cols&&point2.x > a*inputImg.cols)
			{
				if (point1.x < xmin2 && point1.x>xmin1)
					xmin2 = point1.x;
				if (point1.y < ymin2)
					ymin2 = point1.y;
				if (point2.x > xmax2 && point2.x < xmax3)
					xmax2 = point2.x;
				if (point2.y > ymax2)
					ymax2 = point2.y;
			}

			if (point2.x < inputImg.cols&&point2.x > b*inputImg.cols)
			{
				if (point1.x < xmin3 && point1.x>xmin2)
					xmin3 = point1.x;
				if (point1.y < ymin3)
					ymin3 = point1.y;
				if (point2.x > xmax3)
					xmax3 = point2.x;
				if (point2.y > ymax3)
					ymax3 = point2.y;
			}

			++iter;
		}
		else
		{
			iter = contours_set.erase(iter);
		}

	}


	if (xmin1 == a * inputImg.cols&& ymin1 == inputImg.rows&&xmax1 == 0 && ymax1 == 0)
	{
		xmin1 = ymin1 = xmax1 = ymax1 = 0;
	}
	if (xmin2 == b * inputImg.cols&& ymin2 == inputImg.rows&& xmax2 == a * inputImg.cols&& ymax2 == 0)
	{
		xmin2 = ymin2 = xmax2 = ymax2 = 0;
	}
	if (xmin3 == inputImg.cols&&ymin3 == inputImg.rows&& xmax3 == b * inputImg.cols&& ymax3 == 0)
	{
		xmin3 = ymin3 = xmax3 = ymax3 = 0;
	}
	

	std::cout << "左上("  << xmin1 <<","<< ymin1<< ")	"<< "右下(" << xmax1 << "," << ymax1 << ")"<<std::endl;
	Fx = (xmin1 + xmax1) / 2;
	Fy = (ymin1 + ymax1) / 2;
	cout << "F(" << Fx << "," << Fy << ")" << endl;
	txtout(Fx, Fy);
	finalRect1 = Rect(xmin1, ymin1, xmax1 - xmin1, ymax1 - ymin1);
	finalRect2 = Rect(xmin2, ymin2, xmax2 - xmin2, ymax2 - ymin2);
	finalRect3 = Rect(xmin3, ymin3, xmax3 - xmin3, ymax3 - ymin3);
	rectangle(inputImg, finalRect1, Scalar(0, 255, 0));
	rectangle(inputImg, finalRect2, Scalar(0, 255, 0));
	rectangle(inputImg, finalRect3, Scalar(0, 255, 0));
	//imshow("Fire_Detection", inputImg);
}


void txtout(int Fx,int Fy) 
{
	/*
	屏幕参数：640x480
	摄像头参数：
	宽度广角：46.66度   一半为23.33
	高度广角：35.58度	一半为17.79
	*/
	float x = 320; float y = 480;
	x = Fx; y = Fy;
	float Ax = 0; float Ay = 0;
	x = x - 320;y = y - 240;//转化左上角xy轴坐标系为中心xy轴坐标系
	Ax = x / 320;Ay = y / 240;
	Ax = Ax * 23.33; Ay = Ay * 17.79;//转化左上角xy轴坐标系为角度参数
	Ax = 90 + Ax; Ay = 90 + Ay;
	int outx = Ax; int outy = Ay;//变成证书
	string outdata = to_string(outx) + "," + to_string(outy);
	fstream out("CTRL.txt", ios::out | ios::trunc);
	string filename;
	string line = outdata;

	if (out) // 有该文件  
	{
		out << line << endl; // 输入到2.txt中  

	}
	else // 没有该文件  
	{
		cout << "找不到TXT，文件传输失败" << endl;
		system("PAUSE");
	}
}