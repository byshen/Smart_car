#ifndef TARGET_H_
#define TARGET_H_
#include "highgui.h"
#include "cv.h"

using namespace cv;

// Ŀ����
class Target 
{
private:
	// �Ƿ�ѡ���Ƿ����
	int select_object, track_object;
	//��ԭ��
	CvPoint origin;
	// ѡ��Χ
	CvRect selection;
	// ���ٴ���
	CvRect track_window;
	CvConnectedComp track_comp;
	IplImage *hsv, *hue, *mask;
	// ֱ��ͼ
	CvHistogram* hist;
public:
	IplImage *image, *backproject;
	CvBox2D track_box;
	// ���캯��
	Target() 
	{
		image = hsv = hue = mask = backproject = 0;
		select_object = track_object = 0;
		hist = 0;
	}
	//����������
	~Target() 
	{
		cvReleaseImage(&image); 
		cvReleaseImage(&hsv);
		cvReleaseImage(&hue);
		cvReleaseImage(&mask);
		cvReleaseImage(&backproject);
	}
	// �Ƿ�����ѡ��
	int selected() { return select_object; }
	// �Ƿ񱻸���
	int tracked() { return track_object; }
	// ����
	void traceIt(const IplImage *);
	// ����λ����Ϣ
	void updateSelection(int x, int y, IplImage* current);
	// ����ѡ������
	void setSelection(int x, int y, int w, int h) { selection = cvRect(x,y,0,0); }
	// ����ԭ��
	void setOrigin(int x, int y) { origin = cvPoint(x,y); }
	// �����Ƿ�ѡ��
	void setSelect(int v) { select_object = v; }
	// ���ø���״̬
	void setTrack(int v) 
	{ 
		if (v == -1 && !(selection.width > 0 && selection.height > 0)) 
			return;
		track_object = v; 
	}
	// չʾ
	void showSection(IplImage *img);
	// ���ø���Ŀ��
	void setTrackObject(CvPoint p1, CvPoint p2);
};


#endif