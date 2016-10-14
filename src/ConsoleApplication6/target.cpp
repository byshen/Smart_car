#include "Target.h"
#define SMIN 60
#define VMIN 60
#define VMAX 255


// 跟踪函数
void Target::traceIt(const IplImage *frame) 
{
	int bins = 30;
	float range[] = {0,180};
	float *ranges = range;

	if (!image)
	{
		image = cvCreateImage(cvGetSize(frame), 8, 3);
		image -> origin = frame -> origin;
		hsv = cvCreateImage(cvGetSize(frame), 8, 3);
		hue = cvCreateImage(cvGetSize(frame), 8, 1);
		mask = cvCreateImage(cvGetSize(frame), 8, 1);
		backproject = cvCreateImage(cvGetSize(frame), 8, 1);
		hist = cvCreateHist(1, &bins, CV_HIST_ARRAY, &ranges, 1);
	}

    cvCopy(frame, image, 0);
    cvCvtColor(image, hsv, CV_BGR2HSV);

    if (track_object != 0) 
	{
        cvInRangeS(hsv, cvScalar(0, SMIN, VMIN, 0), cvScalar(180, 256, VMAX, 0), mask);
        cvSplit(hsv, hue, 0, 0, 0);
        if (track_object < 0) 
		{
            float max_val = 0.f;
			//设置感兴趣区域
            cvSetImageROI(hue, selection);
            cvSetImageROI(mask, selection);
			// 计算颜色直方图
            cvCalcHist(&hue, hist, 0, mask);
			// 计算直方图大小差值
            cvGetMinMaxHistValue(hist, 0, &max_val, 0, 0);
            cvConvertScale(hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0);
            cvResetImageROI(hue);
            cvResetImageROI(mask);
            track_window = selection;
            track_object = 1;
        }
        cvCalcBackProject(&hue, backproject, hist);
        cvAnd(backproject, mask, backproject, 0);
		// camshift算法
        cvCamShift(backproject, track_window,
                    cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
                    &track_comp, &track_box);
        track_window = track_comp.rect;
		if (!image->origin) track_box.angle = -track_box.angle;
	}
}

// 更新跟踪区域信息
void Target::updateSelection(int x, int y, IplImage* current) 
{
	if (!current) return;
	// 动态更新选取
	// 根据鼠标的不同位置对对象进行设置
	if (select_object) 
	{
        selection.x = min(x,origin.x);
        selection.y = min(y,origin.y);
        selection.width = selection.x + abs(x - origin.x);
        selection.height = selection.y + abs(y - origin.y);

        selection.x = max(selection.x, 0);
        selection.y = max(selection.y, 0);
        selection.width = min(selection.width, current->width);
        selection.height = min(selection.height, current->height);
        selection.width -= selection.x;
        selection.height -= selection.y;
    }
}

// 显示
void Target::showSection(IplImage *img) 
{
	if (select_object && selection.width > 0 && selection.height > 0) 
	{
		cvSetImageROI(img, selection);
		cvXorS(img, cvScalarAll(255), img, 0);
		cvResetImageROI(img);
	}
}

// 设置跟踪目标
void Target::setTrackObject(CvPoint p1, CvPoint p2)
{
	selection.height = abs(p1.y - p2.y);
	selection.width = abs(p2.x - p2.x);
	selection.x = p1.x;
	selection.y = p1.y;
	origin.x = p1.x;
	origin.y = p1.y;
}