#ifndef TARGET_H_
#define TARGET_H_
#include "highgui.h"
#include "cv.h"

using namespace cv;

// 目标类
class Target 
{
private:
	// 是否选择，是否跟踪
	int select_object, track_object;
	//　原点
	CvPoint origin;
	// 选择范围
	CvRect selection;
	// 跟踪窗口
	CvRect track_window;
	CvConnectedComp track_comp;
	IplImage *hsv, *hue, *mask;
	// 直方图
	CvHistogram* hist;
public:
	IplImage *image, *backproject;
	CvBox2D track_box;
	// 构造函数
	Target() 
	{
		image = hsv = hue = mask = backproject = 0;
		select_object = track_object = 0;
		hist = 0;
	}
	//　析构函数
	~Target() 
	{
		cvReleaseImage(&image); 
		cvReleaseImage(&hsv);
		cvReleaseImage(&hue);
		cvReleaseImage(&mask);
		cvReleaseImage(&backproject);
	}
	// 是否正在选择
	int selected() { return select_object; }
	// 是否被跟踪
	int tracked() { return track_object; }
	// 跟踪
	void traceIt(const IplImage *);
	// 更新位置信息
	void updateSelection(int x, int y, IplImage* current);
	// 设置选择区域
	void setSelection(int x, int y, int w, int h) { selection = cvRect(x,y,0,0); }
	// 设置原点
	void setOrigin(int x, int y) { origin = cvPoint(x,y); }
	// 设置是否选择
	void setSelect(int v) { select_object = v; }
	// 设置跟踪状态
	void setTrack(int v) 
	{ 
		if (v == -1 && !(selection.width > 0 && selection.height > 0)) 
			return;
		track_object = v; 
	}
	// 展示
	void showSection(IplImage *img);
	// 设置跟踪目标
	void setTrackObject(CvPoint p1, CvPoint p2);
};


#endif