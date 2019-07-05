#ifndef _DLIB_H_
#define _DLIB_H_

#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv.h>
#include "facedetect-dll.h"
#pragma comment(lib,"libfacedetect-x64.lib")
#define FRAMECOUNT (5)

using namespace cv;
using namespace dlib;

class Dlib
{
public:
	int Init(const char* cascadeFile, const char* landmarkFile);
	int Process(cv::Mat frame);
	int Process2(cv::Mat frame);
	int ProcessOpencv(cv::Mat frame);
	std::vector<point> GetLandmarkPoint();

protected:
	bool CheckEd(cv::Mat frame, cv::Rect rect, double widthScale, double heightScale);

private:
	const char* cascadeFile;			// Opencv人脸识别模型路径
	const char* landmarkFile;			// Dlib人脸特征点训练模型路径
	Ptr<CascadeClassifier> cascade;		// 人脸模型
	shape_predictor sp;					// 特征点模型
	frontal_face_detector detector;		// 检测器
	std::vector<cv::Rect> faces;		// 人脸框
	std::vector<point> landmarks;		// 特征点
	cv::Rect faceCompare[FRAMECOUNT];	// 检测5帧人脸,判断人脸位置变化大小，如果不大则人脸框采用上一帧人脸框
	uint frame_index = 0;
};
#endif