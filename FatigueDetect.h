#ifndef _FATIGUE_DETECT_H
#define _FATIGUE_DETECT_H
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv.h>
//#include <sapi.h>
//#include <sphelper.h>
//#pragma comment(lib, "sapi.lib");

#define EYE_LANDMARK_CNT (6)

using namespace std;
using namespace dlib;

class FatigueDetect
{
public:
	bool Init();
	bool IsFatigue(std::vector<point> landmarks);
	float GetDistance(cv::Point2d a, cv::Point2d b);
private:
	float eye_open_max = 6.5;
	float eye_open_min = 4.5;
	float eye_width_height_radio_threshold = 0.20;
	unsigned int fps = 4;
	uint frame_index = 0;
	float righteye_detect_data[80];	//40÷°
	//HRESULT hr;	//“Ù∆µΩ”ø⁄
	//ISpVoice* pVoice;	//“Ù∆µ
};
#endif