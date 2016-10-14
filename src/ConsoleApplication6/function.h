#include "target.h"
#include "highgui.h"
#include "cv.h"
#define PI 3.1415926535898 
#define SHIFT 0
//1.打开摄像头，固定图像

//2.透视变换
//获取点
int mouseX,mouseY;
void point(int mouseevent, int x, int y, int flags, void* param) 
{ 
	if (mouseevent == CV_EVENT_LBUTTONDOWN) 
	{
		mouseX = x; mouseY = y; 
	};
};

void getpoint(char *win,int &x,int &y) 
{ 
	cvSetMouseCallback(win, point); 
	cvWaitKey();
	x = mouseX;
	y = mouseY;
};

//3.细化图像
//------------定义滑块
int	H_MIN = 0;
int	H_MAX = 255;
int	S_MIN = 0;
int	S_MAX = 255;
int	V_MIN = 0;
int	V_MAX = 255;
int B_H_MAX = 255;
int B_S_MAX = 255;
int B_V_MAX = 255;

void createTrackbars()
{
	namedWindow("Trackbars",0);
	char TrackbarName[50];
	sprintf_s(TrackbarName, "H_MIN", H_MIN);
	sprintf_s(TrackbarName, "H_MAX", H_MAX);
	sprintf_s(TrackbarName, "S_MIN", S_MIN);
	sprintf_s(TrackbarName, "S_MAX", S_MAX);
	sprintf_s(TrackbarName, "V_MIN", V_MIN);
	sprintf_s(TrackbarName, "V_MAX", V_MAX);
	createTrackbar("H_MIN", "Trackbars", &H_MIN, B_H_MAX);
	createTrackbar("H_MAX", "Trackbars", &H_MAX, B_H_MAX);
	createTrackbar("S_MIN", "Trackbars", &S_MIN, B_S_MAX);
	createTrackbar("S_MAX", "Trackbars", &S_MAX, B_S_MAX);
	createTrackbar("V_MIN", "Trackbars", &V_MIN, B_V_MAX);
	createTrackbar("V_MAX", "Trackbars", &V_MAX, B_V_MAX);
}

void Invert(IplImage* img)
{
    int height,width,step,channls;
    uchar* data;
    int i,j,k;
    //获取图片信息
    height = img->height;
    width = img->width;
    step = img->widthStep;
    channls = img->nChannels;
    data = (uchar*)img->imageData;
    //反色图像
    for (i=0;i<height;i++)
    {
        for (j=0;j<width;j++)
        {
            for (k=0;k<channls;k++)
            {
                data[i*step+j*channls+k] = 255 - data[i*step+j*channls+k];
            }
        }
    }
};

template<class T> 
class Image 
{
private:
	IplImage* imgp;
public:
	Image(IplImage* img=0) {imgp=img;}
	~Image(){imgp=0;}
	inline T* operator[](const int rowIndx) 
	{
		return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));
	}
};
typedef struct
{
	unsigned char b,g,r;
} RgbPixel;
typedef struct{ 
  float h,s,v;
} HsvPixelFloat;
typedef Image<RgbPixel> RgbImage;
typedef Image<unsigned char> BwImage;
typedef Image<HsvPixelFloat>  HsvFloatImage;

//细化
void cvThin(IplImage* src, IplImage* dst, int iterations = 1) 
{
	cvCopyImage(src, dst);
	BwImage dstdat(dst);
	IplImage* t_image = cvCloneImage(src);
	BwImage t_dat(t_image);
	for (int n = 0; n < iterations; n++)
		for (int s = 0; s <= 1; s++) 
		{
			cvCopyImage(dst, t_image);
			for (int i = 0; i < src->height; i++)
				for (int j = 0; j < src->width; j++)
					if (t_dat[i][j]) 
					{
						int a = 0, b = 0;
						int d[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1},
										{1, 0}, {1, -1}, {0, -1}, {-1, -1}};
						int p[8];
						p[0] = (i == 0) ? 0 : t_dat[i-1][j];
						for (int k = 1; k <= 8; k++) 
						{
							if (i+d[k%8][0] < 0 || i+d[k%8][0] >= src->height ||
								j+d[k%8][1] < 0 || j+d[k%8][1] >= src->width)
								p[k%8] = 0;
							else 
								p[k%8] = t_dat[ i+d[k%8][0] ][ j+d[k%8][1] ];
							if (p[k%8]) 
							{
								b++;
							if (!p[k-1]) a++;
							}
						}
						if (b >= 2 && b <= 6 && a == 1)
							if (!s && !(p[2] && p[4] && (p[0] || p[6])))
								dstdat[i][j] = 0;
							else if (s && !(p[0] && p[6] && (p[2] || p[4])))
								dstdat[i][j] = 0;
					}
		}
	cvReleaseImage(&t_image);
}



//4.寻找角点

//5.定位小车
//取下一个目标点
CvPoint2D32f findNextPoint(CvPoint2D32f* points,CvPoint2D32f center )
{
	double min = 100000,tmp;
	int minN = 29;
	CvPoint2D32f nextP;
	CvPoint tmpPoint;
	nextP.x = 0;
	nextP.y = 0;
	for(int i = 0;i < 50;++i)
	{
		tmpPoint = cvPointFrom32f(points[i]);
		if(tmpPoint.x != 0 || tmpPoint.y != 0)
		{
			tmp = sqrt((points[i].x-center.x)*(points[i].x-center.x)+(points[i].y-center.y)*(points[i].y-center.y));
			if (tmp < min) 
			{
				min = tmp;
				nextP.x = points[i].x;
				nextP.y = points[i].y;
				minN = i;
			}
		}
	}
	points[minN].x = 0;
	points[minN].y = 0;
	return nextP;
}

IplImage* current = 0;
bool findTarget = false;
Target head,tail; 
// 鼠标事件
void mouse(int mouseevent, int x, int y, int flags, void* param) 
{
	if (current == 0)
		return;
    if (current -> origin) y = current -> height - y;
	// 鼠标选择目标期间
    if (findTarget) 
	{
       head.updateSelection(x, y, current);
	   tail.updateSelection(x, y, current);
    }
	// 鼠标事件
	switch (mouseevent) 
	{
		// 按下左键
		case CV_EVENT_LBUTTONDOWN:
			if (findTarget) 
			{
				// 设定原点
				head.setOrigin(x,y);
				// 设定选择区域
				head.setSelection(x,y,0,0);
				// 设定正在选定
				head.setSelect(1);
				cout << x << ' ' << y << "#1" << endl;
			}
			break;
		// 左键弹起
		case CV_EVENT_LBUTTONUP:
			// 如果是在选择车头
			// 取消选定车头车尾
			if (findTarget) 
			{
				head.setSelect(0);
				// 设定跟踪
				head.setTrack(-1);
				cout << x << ' ' << y << "#2" << endl;
			}
			break;
		// 右键按下
		case CV_EVENT_RBUTTONDOWN:
			if (findTarget) 
			{
				// 设定车尾原点
				tail.setOrigin(x,y);
				// 设定车尾选择范围
				tail.setSelection(x,y,0,0);
				// 设定车位的正在选择
				tail.setSelect(1);
				cout << x << ' ' << y << "#3" << endl;
			}
			break;
		// 右键弹起
		case CV_EVENT_RBUTTONUP:
			if (findTarget) 
			{
				// 设定车尾取消选择区域
				tail.setSelect(0);
				// 设定车尾跟踪
				tail.setTrack(-1);
				cout << x << ' ' << y << "#4" << endl;
			}
			break;
	}
};

void mouseInit(IplImage *trans)
{
	current = trans;
	findTarget = true;
}


//6.开始巡线
double dist(CvPoint2D32f p1,CvPoint2D32f p2)
{
	return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}

int judgeDirection(CvPoint2D32f &head,CvPoint2D32f &tail,CvPoint2D32f &next)
{
	double carDirection,direction;
	double a,b,c;
	a = dist(head,tail);
	b = dist(tail,next);
	c = dist(next,head);
	if (a * a + c * c > b * b)
		return 3;
	carDirection = atan2((head.y - tail.y),(head.x - tail.x))/PI;
	direction = atan2((next.y-tail.y),(next.x-tail.x))/PI;
	if (carDirection - direction > 1.5)
		direction += 2;
	else if (carDirection - direction < -1.5)
		carDirection += 2;
	cout << carDirection << ' ' << direction << endl;
	if (carDirection - direction > 0.1)
		return 1;
	else if (carDirection - direction < -0.1)
		return 2;
	else
		return 0;
}