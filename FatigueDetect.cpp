#include "FatigueDetect.h"

bool FatigueDetect::Init()
{
	//ISpVoice * pVoice = NULL;
	//if (FAILED(::CoInitialize(NULL)))
	//	return false;
	//hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	//if (SUCCEEDED(hr))
	//{
	//	hr = pVoice->Speak(L"音频初始化成功", 0, NULL);
	//	/*pVoice->Release();
	//	pVoice = NULL;*/
	//}
	return true;
}

bool FatigueDetect::IsFatigue(std::vector<point> landmarks)
{
	if (landmarks.size() != 68)
		return false;
	// 计算右眼睛中垂线上的两个点坐标
	cv::Point2d eye_uper = cv::Point2d((landmarks[37].x() + landmarks[38].x()) / 2, (landmarks[37].y() + landmarks[38].y()) / 2);
	cv::Point2d eye_lower = cv::Point2d((landmarks[40].x() + landmarks[41].x()) / 2, (landmarks[40].y() + landmarks[41].y()) / 2);
	double righteye_verti_distance = GetDistance(eye_uper, eye_lower);
	double righteye_horiz_distance = GetDistance(cv::Point2d(landmarks[36].x(), landmarks[36].y()), cv::Point2d(landmarks[39].x(), landmarks[39].y()));
	float radio = righteye_verti_distance / righteye_horiz_distance;
	righteye_detect_data[frame_index % fps] = radio;
	frame_index++;

	int invalid_count = 0;
	if (frame_index > fps)
	{
		for (unsigned int i = 0; i < fps; i++)
		{
			if (righteye_detect_data[i] < this->eye_width_height_radio_threshold)
				invalid_count++;
		}
		float invalid_radio = (float)invalid_count / (float)fps;
		if (invalid_radio > 0.4)
		{
			cout << "预警：：请注意不要疲劳驾驶！！" << endl;
		}
	}
	return true;
}

float FatigueDetect::GetDistance(cv::Point2d a, cv::Point2d b)
{
	return sqrt(pow((float)(a.x - b.x), 2) + pow((float)(a.y - b.y), 2));
}