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
	const char* cascadeFile;			// Opencv����ʶ��ģ��·��
	const char* landmarkFile;			// Dlib����������ѵ��ģ��·��
	Ptr<CascadeClassifier> cascade;		// ����ģ��
	shape_predictor sp;					// ������ģ��
	frontal_face_detector detector;		// �����
	std::vector<cv::Rect> faces;		// ������
	std::vector<point> landmarks;		// ������
	cv::Rect faceCompare[FRAMECOUNT];	// ���5֡����,�ж�����λ�ñ仯��С����������������������һ֡������
	uint frame_index = 0;
};
#endif