#include "Dlib.h"

int Dlib::Init(const char* cascadeFile, const char* landmarkFile)
{
	this->cascadeFile = cascadeFile;
	this->landmarkFile = landmarkFile;
	this->cascade = makePtr<CascadeClassifier>(cascadeFile);
	if (this->cascade == NULL)
	{
		std::cout << "cascade is null，inition failed" << std::endl;
		return -1;
	}
	deserialize(landmarkFile) >> sp;
	this->detector = get_frontal_face_detector();
	return 0;
}

int Dlib::Process(cv::Mat frame)
{
	// 三个通道进行均衡化提高对比度
	//std::vector<Mat> splitFrame;
	//Mat merge;
	//Mat merge1;
	//Mat merge2;
	//Mat gray;
	//split(frame, splitFrame);
	//for (int i = 0; i < splitFrame.size(); i++)
	//{
	//	splitFrame[i].convertTo(splitFrame[i], splitFrame[i].type(), 1.5, 20);
	//	equalizeHist(splitFrame[i], splitFrame[i]);
	//}
	//cv::merge(splitFrame, merge);

	//Mat logEnhance(frame.size(), CV_32FC3);
	//for (int i = 0; i < frame.rows; i++)
	//{
	//	for (int j = 0; j < frame.cols; j++)
	//	{
	//		logEnhance.at<Vec3f>(i, j)[0] = log(1 + frame.at<Vec3f>(i, j)[0]);
	//		logEnhance.at<Vec3f>(i, j)[1] = log(1 + frame.at<Vec3f>(i, j)[1]);
	//		logEnhance.at<Vec3f>(i, j)[2] = log(1 + frame.at<Vec3f>(i, j)[2]);
	//	}
	//}
	//normalize(logEnhance, logEnhance, 0, 255, CV_MINMAX);
	//convertScaleAbs(logEnhance, logEnhance);
	////std::cout << logEnhance << std::endl;
	//imshow("log", logEnhance);
	//// Opencv人脸检出，提高速度
	//cvtColor(frame, gray, CV_RGB2GRAY);//转成灰度图
	//equalizeHist(gray, gray);
	//cascade->detectMultiScale(gray, faces, 1.2, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, Size(30, 30));

	//double widthScale = 2;
	//double heightScale = 2;
	//double scaleWidth;
	//double scaleHeight;
	//double leftTopX;
	//double leftTopY;

	////检出的人脸放大
	//if (CheckEd(frame, faces[0], widthScale, heightScale))
	//{
	//	scaleWidth = faces[0].width * widthScale;
	//	scaleHeight = faces[0].height * heightScale;
	//	leftTopX = (faces[0].x + faces[0].width) / 2 - scaleWidth / 2;
	//	leftTopY = (faces[0].y + faces[0].height) / 2 - scaleHeight / 2;
	//}
	//else
	//{
	//	leftTopX = faces[0].x;
	//	leftTopY = faces[0].y;
	//	scaleWidth = faces[0].width;
	//	scaleHeight = faces[0].height;
	//}
	//dlib::rectangle drect(leftTopX, leftTopY, leftTopX + scaleWidth, leftTopY + scaleHeight);
	
	//dlib::rectangle drect(faces[0].x - 30, faces[0].y - 30, faces[0].x + faces[0].width + 60, faces[0].y + faces[0].height + 60);
	// 根据识别的人脸框提取人脸特征
	cv_image<bgr_pixel> dimg(frame);
	std::vector<dlib::rectangle> dets = detector(dimg);
	if (dets.size() < 1)
		return -1;
	full_object_detection object = sp(dimg, dets[0]);
	this->landmarks.clear();
	for (unsigned int i = 0; i < object.num_parts(); i++)
		landmarks.push_back(object.part(i));
	return 0;
}

int Dlib::Process2(cv::Mat frame)
{
	// 三个通道进行均衡化提高对比度
	std::vector<Mat> splitFrame;
	Mat merge;
	split(frame, splitFrame);
	for (int i = 0; i < splitFrame.size(); i++)
	{
		splitFrame[i].convertTo(splitFrame[i], splitFrame[i].type(), 1.5, 20);
		equalizeHist(splitFrame[i], splitFrame[i]);
	}
	cv::merge(splitFrame, merge);
	
	// 根据识别的人脸框提取人脸特征
	cv_image<bgr_pixel> dimg(merge);

	//采用于仕琪人脸检测提升速度
	Mat gray;
	cvtColor(frame, gray, CV_RGB2GRAY);//转成灰度图
	int* pResults = NULL;
	unsigned char* pBuffer = (unsigned char*)malloc(0x200000);
	if (!pBuffer)
	{
		std::cout << "can not alloc buffer\n";
		return 0;
	}
	pResults = facedetect_frontal_surveillance(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step, 1.2f, 2, 48, 0, 0);
	short* p = ((short*)(pResults + 1));
	int x = p[0];
	int y = p[1];
	int w = p[2];
	int h = p[3];

	//尝试增加人脸框解决landmark点闪烁问题，结果效果不大
	//尝试前5帧判断，如果当前人脸框位置变化不大则用上一帧图像的人脸框，避免闪缩
	//判断前5帧图像框位置
	int meanX = 0;
	int meanY = 0;

	if (frame_index >= 5)
	{
		for (int i = 0; i < FRAMECOUNT; i++)
		{
			meanX += faceCompare[i].x;
			meanY += faceCompare[i].y;
		}
		meanX /= FRAMECOUNT;
		meanY /= FRAMECOUNT;
		cv::Rect faceRect(x, y, w, h);
		this->faceCompare[this->frame_index % FRAMECOUNT] = faceRect;
	}
	double disx = abs(x - meanX);
	double radiox = (double)disx / meanX + 0.0001;
	std::cout << "radiox:" << radiox << std::endl;
	if (double(x - meanX) / double(meanX + 0.0001)< 0.01 && ((double)(y - meanY) / (double)(meanY + 0.0001) < 0.1))
	{
		x = faceCompare[(frame_index - 1) % FRAMECOUNT].x;
		y = faceCompare[(frame_index - 1) % FRAMECOUNT].y;
		w = faceCompare[(frame_index - 1) % FRAMECOUNT].width;
		h = faceCompare[(frame_index - 1) % FRAMECOUNT].height;
	}
	frame_index++;
	
	dlib::rectangle drect((double)x, (double)y, (double)(x + w), (double)(y + h));
	full_object_detection object = sp(dimg, drect);
	this->landmarks.clear();
	for (unsigned int i = 0; i < object.num_parts(); i++)
		landmarks.push_back(object.part(i));

	free(pBuffer);
	return 0;
}

int Dlib::ProcessOpencv(cv::Mat frame)
{
	cv::Mat gray;
	// Opencv人脸检出，提高速度
	cvtColor(frame, gray, CV_RGB2GRAY);//转成灰度图
	//equalizeHist(gray, gray);
	clock_t timestart, timeend, t1, t2;
	timestart = clock();
	cascade->detectMultiScale(gray, faces, 1.2, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, Size(30, 30));
	timeend = clock();
	std::cout << "opencv time:" << timeend - timestart << std::endl;
	if (faces.size() < 1)
		return -1;
	dlib::rectangle drect(faces[0].x - 30, faces[0].y - 30, faces[0].x + faces[0].width + 60, faces[0].y + faces[0].height + 60);
	// 根据识别的人脸框提取人脸特征
	t1 = clock();
	cv_image<bgr_pixel> dimg(frame);
	full_object_detection object = sp(dimg, drect);
	this->landmarks.clear();
	for (unsigned int i = 0; i < object.num_parts(); i++)
		landmarks.push_back(object.part(i));
	t2 = clock();
	std::cout << "dlib time:" << t2 - t1 << std::endl;
	return 0;
}

std::vector<point> Dlib::GetLandmarkPoint()
{
	return this->landmarks;
}

bool Dlib::CheckEd(cv::Mat frame, cv::Rect rect, double widthScale, double heigthScale)
{
	if (widthScale < 0
		||heigthScale < 0
		||rect.x - rect.width * widthScale / 2 < 0
		|| rect.y - rect.width * heigthScale / 2 < 0
		|| rect.x + rect.width * widthScale > frame.cols
		|| rect.y + rect.height * heigthScale > frame.rows)
	{
		return false;
	}
	return true;
}