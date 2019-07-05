#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv.h>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "Dlib.h"
#include "FatigueDetect.h"
#include "facedetect-dll.h"


using namespace dlib;
using namespace std;
using namespace cv;

//void ImageTest();
//void VideoTest();
void VideoTest2();
//void VideoTest3();
int main()
{
	VideoTest2();
	destroyAllWindows();
	return 0;
}

/*void ImageTest()
{
	long start, end;
	setUseOptimized(true);
	char img_file[] = "E:\\WorkStation\\Project\\DlibTest\\x64\\Debug\\image_0026.png";
	char landmark_file[] = "E:\\WorkStation\\Source\\shape_predictor_68_face_landmarks.dat";

	Mat img = imread(img_file, CV_BGR2GRAY);

	frontal_face_detector detector = get_frontal_face_detector();
	shape_predictor sp;
	deserialize(landmark_file) >> sp;

	array2d<rgb_pixel> arrImg;
	load_image(arrImg, img_file);

	start = clock();
	std::vector<dlib::rectangle> dets = detector(arrImg);


	for (unsigned long j = 0; j < dets.size(); ++j)
	{
		full_object_detection shape = sp(arrImg, dets[j]);

		for (unsigned long i = 0; i < shape.num_parts(); i++)
		{
			point pt = shape.part(i);
			int x = pt.x();
			int y = pt.y();

			line(img, Point(pt.x(), pt.y()), Point(pt.x(), pt.y()), Scalar(0, 0, 255), 2);
		}
	}
	end = clock();
	printf("%ld\n", end - start);//单位：毫秒
	imshow("img", img);
	waitKey();
}

void VideoTest()
{
	char landmark_file[] = "shape_predictor_68_face_landmarks.dat";
	Ptr<CascadeClassifier> cascade = makePtr<CascadeClassifier>("lbpcascade_frontalface_improved.xml");
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		std::cout << "open failed" << std::endl;
		return;
	}
	Mat frame;
	Mat merge;
	Mat convermat;
	std::vector<cv::Rect> faces;
	frontal_face_detector detector = get_frontal_face_detector();
	shape_predictor sp;
	deserialize(landmark_file) >> sp;
	clock_t start, end;
	while (true)
	{
		cap >> frame;
		if (frame.empty())
		{
			std::cout<< "frame is empty" << std:: endl;
			break;
		}
		start = clock();
		std::vector<Mat> splitFrame;
		split(frame, splitFrame);
		for (int i = 0; i < splitFrame.size(); i++)
		{
			splitFrame[i].convertTo(splitFrame[i],splitFrame[i].type(),1.5,20);
			equalizeHist(splitFrame[i], splitFrame[i]);
		}
		cv::merge(splitFrame, merge);
		imshow("equ",merge);
		
		cv_image<rgb_pixel> arrImg(merge);
		std::vector<dlib::rectangle> dets = detector(arrImg);
		for (unsigned long j = 0; j < dets.size(); ++j)
		{
			full_object_detection shape = sp(arrImg, dets[j]);

			for (unsigned long i = 0; i < shape.num_parts(); i++)
			{
				point pt = shape.part(i);
				int x = pt.x();
				int y = pt.y();

				circle(frame, Point(pt.x(), pt.y()), 0.2, Scalar(0, 0, 255), 2);
			}
		}
		end = clock();
		cout << "time elapse:" << end - start << endl;
		imshow("face align", frame);
		if (waitKey(5) == 'q')
		{
			break;
		}
	}
}
*/
void VideoTest2()
{
	// opencv人脸识别，Dlib提取特征点
	char cascade_file[] = "lbpcascade_frontalface_improved.xml";
	char landmark_file[] = "shape_predictor_68_face_landmarks.dat";
	
	Dlib dlibInstance;
	float scaleframe = 2;
	dlibInstance.Init(cascade_file, landmark_file);
	FatigueDetect fatigureInstance;
	fatigureInstance.Init();
	cout << "坐姿检测完毕" << endl;
	cout << "等待系统检索" << endl;
	// 获取摄像头视频
	VideoCapture cap(1);
	cap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M' , 'J', 'P', 'G'));
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 480);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 640);
	cap.set(CV_CAP_PROP_AUTO_EXPOSURE, 1);
	if (!cap.isOpened())
	{
		std::cout << "open failed" << std::endl;
		return;
	}
	
	Mat frame;
	clock_t start, end;
	std::vector<dlib::point> currentLandmarks;
	//fps
	double fps;
	char string[10];  // 用于存放帧率的字符串	
	double t = 0;
	while (true)
	{
		t = (double)cv::getTickCount();
		cap >> frame;
		start = clock();
		if (frame.empty())
		{
			std::cout << "frame is empty" << std::endl;
			break;
		}
		cv::Rect& rectreal = Rect(0,0,0,0);
		cv::Rect& rect60 = Rect(0,0,0,0);
		dlibInstance.Process2(frame); //于仕琪人脸检测
		//dlibInstance.Process(frame);//opencv方式人脸检测
		currentLandmarks.clear();
		currentLandmarks = dlibInstance.GetLandmarkPoint();
		if (currentLandmarks.size() != 68)
			continue;
		//检出疲劳
		fatigureInstance.IsFatigue(currentLandmarks);
		for (int i = 0; i < currentLandmarks.size(); i++)
		{
			circle(frame, cv::Point(currentLandmarks[i].x(), currentLandmarks[i].y()), 1, Scalar(0, 255, 0), 1);
		}
		
		end = clock();
		cout << "time elapse:" << end - start << endl;
		namedWindow("监控反馈后台");
		// getTickcount函数：返回从操作系统启动到当前所经过的毫秒数
		// getTickFrequency函数：返回每秒的计时周期数
		// t为该处代码执行所耗的时间,单位为秒,fps为其倒数
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		fps = 1.0 / t;

		sprintf_s(string, "%.2f", fps);      // 帧率保留两位小数
		std::string fpsString("Computable FPS by LCS-->");
		fpsString += string;                    // 在"FPS:"后加入帧率数值字符串
												// 将帧率信息写在输出帧上
		putText(frame,                  // 图像矩阵
			fpsString,                  // string型文字内容
			cv::Point(5, 20),           // 文字坐标，以左下角为原点
			cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
			0.5,                    // 字体大小
			cv::Scalar(0, 0, 0));           // 字体颜色
		std::string fpsString2("Driver：LEE WEN JUN");
		putText(frame,                  // 图像矩阵
			fpsString2,                  // string型文字内容
			cv::Point(30, 50),           // 文字坐标，以左下角为原点
			cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
			0.5,                    // 字体大小
			cv::Scalar(0, 0, 0));

		imshow("监控反馈后台", frame);
		if (waitKey(10) == 'q')
		{
			break;
		}
		
	}
	
}
/*
void VideoTest3()
{
	// opencv人脸识别，Dlib提取特征点
	char cascade_file[] = "lbpcascade_frontalface_improved.xml";
	char landmark_file[] = "shape_predictor_68_face_landmarks.dat";

	double framescale = 3;
	Dlib dlibInstance;
	dlibInstance.Init(cascade_file, landmark_file);
	FatigueDetect fatigureInstance;
	fatigureInstance.Init();
	cout << "Loading detector model..." << endl;
	// 获取摄像头视频
	VideoCapture cap(0);
	cap.set(CAP_PROP_FPS, 40);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 480);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 640);

	if (!cap.isOpened())
	{
		std::cout << "open failed" << std::endl;
		return;
	}

	Mat frame;
	clock_t start, end;
	std::vector<dlib::point> currentLandmarks;
	while (true)
	{
		cap >> frame;
		start = clock();
		if (frame.empty())
		{
			std::cout << "frame is empty" << std::endl;
			break;
		}
		int ret = dlibInstance.ProcessOpencv(frame);
		if (ret == -1)
			continue;
		currentLandmarks.clear();
		currentLandmarks = dlibInstance.GetLandmarkPoint();

		//检出疲劳
		fatigureInstance.IsFatigue(currentLandmarks);
		for (int i = 0; i < currentLandmarks.size(); i++)
		{
			circle(frame, cv::Point(currentLandmarks[i].x(), currentLandmarks[i].y()), 1, Scalar(0, 255, 0), 1);
		}
		end = clock();
		cout << "time elapse:" << end - start << endl;
		namedWindow("face align");
		imshow("face align", frame);
		if (waitKey(20) == 'q')
		{
			break;
		}
	}
}
*/