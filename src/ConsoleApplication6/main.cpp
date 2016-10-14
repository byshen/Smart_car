#include "SerialPort.h"
#include <iostream>
#include "highgui.h"
#include "cv.h"
#include "car.h"
#include "function.h"
#include "target.h"

using namespace std;
using namespace cv;

#define CAMWIDTH 640
#define CAMHEIGHT 480

int main()
{
	IplImage *img = cvCreateImage(cvSize(CAMWIDTH,CAMHEIGHT),IPL_DEPTH_8U,3);
	IplImage *trans	= cvCreateImage(cvSize(CAMWIDTH,CAMHEIGHT),IPL_DEPTH_8U,3);
	CvMat *transmat = cvCreateMat(3, 3, CV_32FC1);
	IplImage *thin = cvCreateImage(cvGetSize(trans),IPL_DEPTH_8U,1);
	CvCapture *cam = cvCreateCameraCapture(0);

	//1.打开摄像头，固定图像
	cout << "1.打开摄像头，固定图像" << endl;
	cvNamedWindow("win1");
	if (!cam)
	{
		cout << "摄像头打开不成功！" << endl;
		return false;
	}
	cout << "摄像头打开成功！按任意键固定图像..." << endl; 
	while (1)
	{
		img = cvQueryFrame(cam);
		cvShowImage("win1",img); 
		if (cvWaitKey(1) >= 0)
			break; 
	}
	cvShowImage("win1",img);

	//2.透视变换
	cout << "\n2.透视变换" << endl;
	cout << "获取边界点，单击然后按任意键确认..." << endl;
	int mouseXY[8];
	for (int i = 0;i < 4;++i)
	{
		getpoint("win1",mouseXY[2 * i],mouseXY[2 * i + 1]);
		cout << "x = "<< mouseXY[2 * i] << " y = " << mouseXY[2 * i + 1] << endl;
		//画出标注的点
		cvCircle(img, cvPoint(mouseXY[2 * i],mouseXY[2 * i + 1]),1, CV_RGB(255,0,0),3);
		cvShowImage("win1",img);
	}
	CvPoint2D32f originpoints[4];
	CvPoint2D32f newpoints[4]; 
	originpoints[0]=cvPoint2D32f(mouseXY[0],mouseXY[1]);
	newpoints[0]=cvPoint2D32f(0,0);
	originpoints[1]=cvPoint2D32f(mouseXY[2],mouseXY[3]);
	newpoints[1]=cvPoint2D32f(CAMWIDTH,0);
	originpoints[2]=cvPoint2D32f(mouseXY[4],mouseXY[5]);
	newpoints[2]=cvPoint2D32f(0,CAMHEIGHT);
	originpoints[3]=cvPoint2D32f(mouseXY[6],mouseXY[7]);
	newpoints[3]=cvPoint2D32f(CAMWIDTH,CAMHEIGHT);
	cvGetPerspectiveTransform(originpoints, newpoints, transmat);
	cvWarpPerspective(img, trans, transmat); 
	cvShowImage("win1", trans);

	
	//3.细化图像
	cout << "\n3.细化图像" << endl;
	IplImage *hsvImg = cvCreateImage(cvGetSize(trans), IPL_DEPTH_8U, 3);
	IplImage *transimg = cvCreateImage(cvGetSize(trans), IPL_DEPTH_8U,1);
	cvNamedWindow("win2");
	createTrackbars();
	while (1)
	{
		cout<<"调节滑块使跑道最清晰，按y键确认..."<<endl;
		while (1)
		{
			if (cvWaitKey(1)=='y') 
				break;
			cvCvtColor(trans,hsvImg, CV_BGR2HSV);
			cvInRangeS(hsvImg,cvScalar(H_MIN,S_MIN,V_MIN), cvScalar(H_MAX,S_MAX,V_MAX), transimg);
			Invert(transimg);
			cvShowImage("win2",transimg);
		}
		cvThin(transimg, thin, 20);
		cvShowImage("win2",thin);
		cout<<"细化图像，按y键确认..."<<endl;
		if (cvWaitKey() == 'y')
			break;
	}


	//4.寻找角点
	cout << "\n4.寻找角点" << endl;
	CvPoint2D32f corners[50];
	IplImage *trackImage = cvCreateImage(cvGetSize(thin),IPL_DEPTH_8U,3);
	IplImage *tmp1 = cvCreateImage(cvGetSize(thin),IPL_DEPTH_32F,1);
	IplImage *tmp2 = cvCreateImage(cvGetSize(thin),IPL_DEPTH_32F,1);
	int cornerCounts = 30;
	for(int i=0;i<50;i++)
	{
		corners[i].x=0;
		corners[i].y=0;
	}
	cvGoodFeaturesToTrack(thin,tmp1,tmp2,corners,&cornerCounts,0.05,40);  //最后一个参数为角点最小距离
	cvMerge(thin,thin,thin,0,trackImage);
    for (int i=0; i <cornerCounts;++i)
	{ 
		if(corners[i].x< CAMWIDTH - 10 && corners[i].x>10 && corners[i].y>10 && corners[i].y< CAMHEIGHT)
			cvCircle(trackImage, cvPoint((int)(corners[i].x), (int)(corners[i].y)), 6, CV_RGB(255,0,0),2);
		
	}
	cvReleaseImage(&tmp1); 
	cvReleaseImage(&tmp2); 
	
	//查看角点
	cout << "角点坐标为：" << endl;
	for(int i = 0;i < 50;i++) 
	{
		if(corners[i].x<10 || corners[i].x> CAMWIDTH - 10 || corners[i].y<10 || corners[i].y> CAMHEIGHT - 10)  
		{ corners[i].x=0;corners[i].y=0; }
		cout << int(corners[i].x) << ' ' << int(corners[i].y) << endl;
	}
	cvShowImage("win1",trackImage);

	//5.定位小车
	cout << "\n5.定位小车" << endl;
	cout << "左键拖动选择车头，右键拖动选择车尾,按y键结束选择..." << endl;
	cvNamedWindow("win2");
	cvSetMouseCallback("win2",mouse);
	cvShowImage("win2", trans);
	mouseInit(trans);

	while(1)
	{
		img = cvQueryFrame(cam);
		cvWarpPerspective(img, trans, transmat);
		head.showSection(trans);
		tail.showSection(trans);
		// 跟踪头
		head.traceIt(trans);
		// 跟踪尾
		tail.traceIt(trans);
		// 画车的位置
		if (head.tracked()) 
			cvCircle(trans, cvPoint((int)(head.track_box.center.x), (int)(head.track_box.center.y+SHIFT)),2, CV_RGB(255,0,0),3);
		// 画车尾的位置
		if (tail.tracked()) 
			cvCircle(trans, cvPoint((int)(tail.track_box.center.x), (int)(tail.track_box.center.y+SHIFT)),2, CV_RGB(0,255,0),3);
		cvShowImage("win2", trans);
		if (cvWaitKey(1) == 'y') break;		//等待1毫秒，如果期间按下任意键则退出循环 
	}
	cout<<"小车定位完毕"<<endl;
	
	//6.开始巡线
	cout << "\n6.开始巡线" << endl;
	double tmpDistance = 0,operateNum; 
	CvPoint2D32f nextPoint,headPoint,tailPoint;
	Car mycar;

	mycar.setPort(5);//设置端口号，根据实际情况调整
	while(1)
	{
		img = cvQueryFrame(cam);
		cvWarpPerspective(img, trans, transmat);
		head.showSection(trans);
		tail.showSection(trans);
		// 跟踪头
		head.traceIt(trans);
		// 跟踪尾
		tail.traceIt(trans);
		// 画车的位置
		if (head.tracked()) 
			cvCircle(trans, cvPoint((int)(head.track_box.center.x), (int)(head.track_box.center.y+SHIFT)),2,CV_RGB(255,0,0),3);
		// 画车尾的位置
		if (tail.tracked()) 
			cvCircle(trans, cvPoint((int)(tail.track_box.center.x), (int)(tail.track_box.center.y+SHIFT)),2,CV_RGB(0,255,0),3);
		headPoint.x = (float)((int)(head.track_box.center.x + SHIFT));
		headPoint.y = (float)((int)(head.track_box.center.y + SHIFT));
		tailPoint.x = (float)((int)(tail.track_box.center.x + SHIFT));
		tailPoint.y = (float)((int)(tail.track_box.center.y + SHIFT));
		//当距离当前目标点小于该阈值时认为这个点已经走到，取下一个目标点
		if (tmpDistance < 60)
			nextPoint = findNextPoint(corners,headPoint);
		
		if (int(nextPoint.x) == 0 && int(nextPoint.y) == 0)
			break;
		tmpDistance = dist(nextPoint,headPoint);
		cout << "头坐标:" << headPoint.x << ' ' << headPoint.y << ' ';
		cout << "尾坐标:" << tailPoint.x << ' ' << tailPoint.y << ' ';
		cout << "目标坐标:" << nextPoint.x << ' ' << nextPoint.y << ' ' << endl;
		operateNum = judgeDirection(headPoint,tailPoint,nextPoint);
		if (operateNum == 2)
		{
			cout<<"right!!"; mycar.turnRight(); 
		}
		else if (operateNum == 1) 
		{
			cout<<"left!!"; mycar.turnLeft();
		}
		else if (operateNum == 0)
		{ 
			cout<<"run!!"; mycar.goForward();
		}
		else if (operateNum == 3)
		{
			cout << "back!!";mycar.goBackward();
		}

        cvCircle(trackImage, cvPoint((int)(nextPoint.x), (int)(nextPoint.y)), 6, CV_RGB(0,255,0),2); 
		cvCircle(trans, cvPoint((int)(nextPoint.x), (int)(nextPoint.y)), 6, CV_RGB(255,255,255),2); 
		
		cvShowImage("win1",trackImage); //win1显示路线图		 
		cvShowImage("win2",trans);//win2显示小车实时状况

		if (cvWaitKey(1) == 'y') break; 
	}
	mycar.stop();
	cout << "成功走完全程" << endl;
	
	cvWaitKey();
	cvReleaseCapture(&cam);
	cvDestroyWindow("win1");
	cvDestroyWindow("win2");
	return 0;
}