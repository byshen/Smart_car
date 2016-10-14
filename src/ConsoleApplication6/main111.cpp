#include "SerialPort.h"
#include <iostream>
#include "highgui.h"
#include "cv.h"
#include "constant.h"

using namespace std;
using namespace cv;

class Car{
	// ��ǰ����
	int currentCmd;
	// ��������
	CSerialPort connect;
	char recv;
public:
	// ���ö˿�
	bool setPort(UINT port);
	// �Ͽ�����
	bool release() { return connect.CloseListenTread(); }
	// ��ǰ��
	void goForward();
	// �����
	void goBackward();
	// ����ת
	void turnLeft();
	// ����ת
	void turnRight();
	// ֹͣ
	void stop();
};
// ���ö˿�
bool Car::setPort(UINT port) {
	if (!connect.InitPort(port)) {
		return false;
	}
	
	if (!connect.OpenListenThread()) {
		cout << "���������߳�ʧ��" << endl;
		return false;
	} else {
		cout << "���������̳߳ɹ�" << endl;
	}
	return true;
}

// ��ǰ
void Car::goForward() {
	unsigned char* forward = new unsigned char;
	*forward = 'A';
	connect.WriteData(forward,1);
}
// ���
void Car::goBackward() {
	unsigned char* backward = new unsigned char;
	*backward = 'B';
	connect.WriteData(backward,1);
}
// ����
void Car::turnLeft() {
	unsigned char* left = new unsigned char;
	*left = 'L';
	connect.WriteData(left,1);
}
// ����
void Car::turnRight() {
	unsigned char* right = new unsigned char;
	*right = 'R';
	connect.WriteData(right,1);
}
// ֹͣ
void Car::stop() {
	unsigned char* st = new unsigned char;
	*st = 'P';
	connect.WriteData(st,1);
}




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
//����
void AddPoint(IplImage* img,int addx,int addy,int PointValue)
{
	if (addy >= 0 && addy < img -> height && addx >= 0 && addx < img -> width)  //�Ұ�||�ĳ�&&��
	for (int k = 0;k < img -> nChannels ;k++) 
		img -> imageData [addy * img -> widthStep + addx * img -> nChannels + k] = PointValue;
};

void Mark(char* win,IplImage* img,int mx,int my,int rad=3)
{
	for (int i=1;i<=rad;i++)
	{
		AddPoint(img,mx-i,my,100);
		AddPoint(img,mx+i,my,100);
		AddPoint(img,mx,my-i,100);
		AddPoint(img,mx,my+i,100);
		//cout<<"# i="<<i<<endl;
	};
	AddPoint(img,mx,my,-1);
	cvShowImage(win,img);
};


//��ȡ��
int mouseX,mouseY,mouseXY[12];
void point(int mouseevent, int x, int y, int flags, void* param) 
{ 
	if (mouseevent == CV_EVENT_LBUTTONDOWN) //������� 
	{
		mouseX = x; mouseY = y; //��¼��ǰλ��
	};
};

void getpoint(char* win) 
{ 
	cvSetMouseCallback(win, point); //���ûص����� 
	cvWaitKey(); //�ȴ���������������� 
};

void get4points(int *mouse_xy,IplImage* org)
{
	cout<<"#Get points..."<<endl;			//ȡ�ӽ�ת����
	cout<<"#GetPoint \n[Click & press key to confirm]"<<endl;
	getpoint("win1");
	cout<<"#x0="<<mouseX<<" y0="<<mouseY<<endl;
	mouse_xy[0]=mouseX;mouse_xy[1]=mouseY;
	Mark("win1",org,mouseX,mouseY,10);
	getpoint("win1");
	cout<<"#x1="<<mouseX<<" y1="<<mouseY<<endl;
	mouse_xy[2]=mouseX;mouse_xy[3]=mouseY;
	Mark("win1",org,mouseX,mouseY,10);
	getpoint("win1");
	cout<<"#x2="<<mouseX<<" y2="<<mouseY<<endl;
	mouse_xy[4]=mouseX;mouse_xy[5]=mouseY;
	Mark("win1",org,mouseX,mouseY,10);
	getpoint("win1");
	cout<<"#x3="<<mouseX<<" y3="<<mouseY<<endl;
	mouse_xy[6]=mouseX;mouse_xy[7]=mouseY;
	Mark("win1",org,mouseX,mouseY,10);
};
void PerspectiveTrans(IplImage* src, IplImage* dst,int* mouse_xy ,CvMat *transmat)
{	
	//CvMat* transmat = cvCreateMat(3, 3, CV_32FC1); 
	CvPoint2D32f originpoints[4]; 
	CvPoint2D32f newpoints[4]; 
	originpoints[0]=cvPoint2D32f(mouse_xy[0],mouse_xy[1]);
	newpoints[0]=cvPoint2D32f(0,0);
	originpoints[1]=cvPoint2D32f(mouse_xy[2],mouse_xy[3]);
	newpoints[1]=cvPoint2D32f(D_WIDTH,0);
	originpoints[2]=cvPoint2D32f(mouse_xy[4],mouse_xy[5]);
	newpoints[2]=cvPoint2D32f(0,D_HEIGHT);
	originpoints[3]=cvPoint2D32f(mouse_xy[6],mouse_xy[7]);
	newpoints[3]=cvPoint2D32f(D_WIDTH,D_HEIGHT);
	cvGetPerspectiveTransform(originpoints, newpoints, transmat);
	cvWarpPerspective(src, dst, transmat); 
					//cvNamedWindow("PerspectiveTrans");
					//cvShowImage("PerspectiveTrans", dst);
};
void Invert(IplImage* img)
{
    int height,width,step,channls;
    uchar* data;
    int i,j,k;
    //��ȡͼƬ��Ϣ
    height = img->height;
    width = img->width;
    step = img->widthStep;
    channls = img->nChannels;
    data = (uchar*)img->imageData;
								//printf("%dx%d image,%d channels,%d widthStep(width*chann)\n",width,height,channls,step);
    //��ɫͼ��
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

//------------���廬��
int	H_MIN = 0;
int	H_MAX = 255;
int	S_MIN = 0;
int	S_MAX = 255;
int	V_MIN = 25;
int	V_MAX = 190;
int B_H_MAX = 255;

int B_S_MAX = 255;

int B_V_MAX = 255;
const string trackbarWindowName = "Trackbars";
const string windowName2 = "Thresholded Image";

//---------��������������������������������������������---���廬��
void on_trackbar( int, void* )
{//This function gets called whenever a
	// trackbar position is changed

}

void createTrackbars(){
	//create window for trackbars


	namedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, B_H_MAX, on_trackbar );
	createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, B_H_MAX, on_trackbar );
	createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, B_S_MAX, on_trackbar );
	createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, B_S_MAX, on_trackbar );
	createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, B_V_MAX, on_trackbar );
	createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, B_V_MAX, on_trackbar );

	//����һ��
}


//ϸ��
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

void thinWindow(IplImage* thin,IplImage* src,int thinx)
{
	cvThin(src,thin,thinx);
	//cvShowImage("win2",thin);
};
void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);


	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);

}


//ȡ��һ��Ŀ���
CvPoint2D32f nextPoint(CvPoint2D32f* points,CvPoint2D32f center )
{
	double min=100000,tmp;
	int minN=29;
	CvPoint2D32f nextP;
	nextP.x=0;nextP.y=0;
	CvPoint tmpPoint;
	for(int i=0;i<50;i++)
	{
		tmpPoint=cvPointFrom32f(points[i]);
		if(tmpPoint.x!=0 || tmpPoint.y!=0)
		{
			tmp=sqrt((points[i].x-center.x)*(points[i].x-center.x)+(points[i].y-center.y)*(points[i].y-center.y));
			/*tmp=distance(points[i],center);*/
			if(tmp<min) { min=tmp;nextP.x=points[i].x;nextP.y=points[i].y;minN=i;} //�ҵ�Ŀ����ֵ��nextP����������ǵ㸳0
		}

	}
	points[minN].x=0;points[minN].y=0;

	 return nextP;
}


CvPoint2D32f corners[50];
// ����ͷͼ��
	IplImage* cam_img = 0;
	// �任ͼ��
	IplImage* trans_img = cvCreateImage(cvSize(D_WIDTH,D_HEIGHT),IPL_DEPTH_8U,3);
		// ��ǰͼ��
	IplImage* current = 0;

	// С��

int main()
{
	IplImage *img,*origin; 
	IplImage *trans	= cvCreateImage(cvSize(D_WIDTH,D_HEIGHT),IPL_DEPTH_8U,3);
	IplImage *thin = cvCreateImage(cvSize(D_WIDTH,D_HEIGHT),IPL_DEPTH_8U,1);
	CvMat *transmat = cvCreateMat(3, 3, CV_32FC1);

	cvNamedWindow("win1");
	CvCapture* cam = cvCreateCameraCapture(0);
	if (!cam)
	{
		cout << "����ͷ�򿪲��ɹ���" << endl;
		return 0;
	}

	while (1)
	{
		img = cvQueryFrame(cam);
		cvShowImage("win1", img);
		if (cvWaitKey(1)>=0) 
			break; 
		origin = cvQueryFrame(cam);
	}
	cvShowImage("win1", origin);


	get4points(mouseXY,origin);//˳�����ϣ����ϣ����£�����
	PerspectiveTrans(origin,trans,mouseXY,transmat);//ת����mouse_xyǰ8����ʵ�֣�����transmat
	cvShowImage("win1", trans);			//trans ���ڴ洢���ӽ�ת���󣬷�ɫ��ͼ��
	



	cvNamedWindow("win2");	
	createTrackbars();

	IplImage* caphsv = cvCreateImage(cvGetSize(trans), 8, 3);
	IplImage* transimg = cvCreateImage(cvGetSize(trans), IPL_DEPTH_8U,1);
	IplImage* transimg2 = cvCreateImage(cvGetSize(trans), IPL_DEPTH_8U,1);
	while (1)
	{
		cout<<"��ѡ�ú�y"<<endl;
		while (1)
		{
			if (cvWaitKey(1)=='y') 
				break;
			cvCvtColor(trans, caphsv, CV_BGR2HSV);
			cvInRangeS(caphsv,cvScalar(H_MIN,S_MIN,V_MIN), cvScalar(H_MAX,S_MAX,V_MAX), transimg);
			Invert(transimg);
			cvShowImage("win2",transimg);
		}

		cvThin(transimg, transimg2, 20);
		cvShowImage("win2",transimg2);
		cout<<"ϸ��ͼ���ѳ�¯��ȷ�ϰ�y"<<endl;
		if (cvWaitKey()=='y')
			break;
	}




	//Ѱ�ҽǵ�
		IplImage *TrackImage=cvCreateImage(cvGetSize(transimg2),IPL_DEPTH_8U,3);
		int cornerCounts=30;
		for(int i=0;i<50;i++) //��ʼ������
		{
			corners[i].x=0;
			corners[i].y=0;
		}
		IplImage* tmp1 = cvCreateImage(cvGetSize(transimg2),IPL_DEPTH_32F,1);
		IplImage* tmp2 = cvCreateImage(cvGetSize(transimg2),IPL_DEPTH_32F,1);
		cvGoodFeaturesToTrack(transimg2,tmp1,tmp2,corners,&cornerCounts,0.05,40);  //���һ������Ϊ�ǵ���С����
		cvMerge(transimg2,transimg2,transimg2,0,TrackImage);
        for (int i=0; i <cornerCounts;++i){ 
			if(corners[i].x< D_WIDTH - 10 && corners[i].x>10 && corners[i].y>10 && corners[i].y< D_HEIGHT)
              cvCircle(TrackImage, cvPoint((int)(corners[i].x), (int)(corners[i].y)), 6, CV_RGB(255,0,0),2); 
			}
		cvReleaseImage(&tmp1); cvReleaseImage(&tmp2); 
		
		
		//test  �鿴�ǵ�
			for(int i=0;i<50;i++) 
		{
			if(corners[i].x<10 || corners[i].x> D_WIDTH - 10 || corners[i].y<10 || corners[i].y> D_HEIGHT - 10)  
			{ corners[i].x=0;corners[i].y=0; }
			cout<<int(corners[i].x)<<' '<<int(corners[i].y)<<endl;
		}

		
		cvShowImage("win3",TrackImage);






	int Nred=0,Ngreen=0; //���ĵ㣬NredΪ�������ģ�NgreenΪ��ͷ���ģ�ʵ�ʰ���ɫ����ģ�����������ɫ��������
	int sumx=0,sumy=0;
	CvPoint2D32f Cred,Cgreen;
	IplImage* transimg3 = cvCreateImage(cvSize(D_WIDTH,D_HEIGHT),IPL_DEPTH_8U,3);
	img = cvQueryFrame(cam);    //��ȡһ֡ͼ�񣬲�����imgָ����
	cvWarpPerspective(img, transimg3, transmat);  //���ݱ任�������ͼ��ı任

	IplImage *dst_image = cvCreateImage(cvGetSize(transimg3),32,transimg3->nChannels);
	IplImage *src_image_32 = cvCreateImage(cvGetSize(transimg3),32,transimg3->nChannels);
	HsvFloatImage HSVimg(dst_image);

	while(1)
	{
		img = cvQueryFrame(cam);    //��ȡһ֡ͼ�񣬲�����imgָ����
		cvWarpPerspective(img, transimg3, transmat);  //���ݱ任�������ͼ��ı任

		cvConvertScale(transimg3,src_image_32);//��ԭͼת��Ϊ32f����
		cvCvtColor(src_image_32,dst_image,CV_BGR2HSV);//�õ�HSVͼ��������dst_image��  ������ΧH(0,360) S(0,1) V(0,255)

		sumx=0;sumy=0;
	   	Nred=0;Ngreen=0;
		for (int i=0;i<480;++i){               //Ѱ�Һ�ɫ���� H������Χ
		  for (int j=0;j<640;++j){
			  if ( ( (HSVimg[i][j].h>-1 && HSVimg[i][j].h<10)||(HSVimg[i][j].h>350 && HSVimg[i][j].h<361) ) && HSVimg[i][j].s>0.4   && HSVimg[i][j].v>100 )
			  {sumx+=j;sumy+=i;++Nred;}		
		  }
	   }
	  if(Nred==0) 	Nred=1;
	   Cred.x=float(sumx/Nred);Cred.y=float(sumy/Nred);    


	   sumx=0;sumy=0;                     //Ѱ����ɫ����,H������Χ 80,135
		for (int i=0;i<480;++i){
		  for (int j=0;j<640;++j){
			  if ( (HSVimg[i][j].h>160 && HSVimg[i][j].h<200) && HSVimg[i][j].s>0.4   && HSVimg[i][j].v>100  )
			  {sumx+=j;sumy+=i;++Ngreen;}	
		  }
	   }		
		if(Ngreen==0) 	Ngreen=1;
		Cgreen.x=float(sumx/Ngreen);Cgreen.y=float(sumy/Ngreen);

		//������������
		cvCircle(transimg3, cvPoint((int)(Cred.x), (int)(Cred.y)), 15, CV_RGB(255,0,0),4); 
		cvCircle(transimg3, cvPoint((int)(Cgreen.x), (int)(Cgreen.y)), 15, CV_RGB(0,255,0),4); 

		cvShowImage("win2",transimg3);

		if (cvWaitKey(1)==13) break; 
	}



	//��ʼѲ��
	//С�����Ʋ��ֲ�����yawΪС������ǣ�directionΪĿ��ķ���ǣ�tmpDistanceΪ����һ��Ŀ���ľ���
	double yaw,direction,tmpDistance=0; 
	CvPoint2D32f next;
	Car mycar;
	mycar.setPort(9);
	while(1)
	{
		img = cvQueryFrame(cam);   
		cvWarpPerspective(img, transimg3, transmat);  //���ݱ任�������ͼ��ı任
		cvConvertScale(transimg3,src_image_32);//��ԭͼת��Ϊ32f����
		cvCvtColor(src_image_32,dst_image,CV_BGR2HSV);//�õ�HSVͼ��������dst_image��  H(0,360) S(0,1) V(0,255)

		sumx=0;sumy=0;
	   	Nred=0;Ngreen=0;
		for (int i=0;i<480;++i){               //Ѱ�Һ�ɫ���� H������Χ 
		  for (int j=0;j<640;++j){
			  if ( ( (HSVimg[i][j].h>-1 && HSVimg[i][j].h<10)||(HSVimg[i][j].h>350 && HSVimg[i][j].h<361) ) && HSVimg[i][j].s>0.4   && HSVimg[i][j].v>60 )
			  {sumx+=j;sumy+=i;++Nred;}		
		  }
	   }
	  if(Nred==0) 	Nred=1;
	   Cred.x=float(sumx/Nred);Cred.y=float(sumy/Nred);     


	   sumx=0;sumy=0;                     //Ѱ����ɫ����,H������Χ 
		for (int i=0;i<480;++i){
		  for (int j=0;j<640;++j){
			  if ( (HSVimg[i][j].h>160 && HSVimg[i][j].h<200) && HSVimg[i][j].s>0.4   && HSVimg[i][j].v>100  )
			  {sumx+=j;sumy+=i;++Ngreen;}	
		  }
	   }		
		if(Ngreen==0) 	Ngreen=1;
		Cgreen.x=float(sumx/Ngreen);Cgreen.y=float(sumy/Ngreen);
		//��������
		cvCircle(transimg3, cvPoint((int)(Cred.x), (int)(Cred.y)), 15, CV_RGB(255,0,0),4); 
		cvCircle(transimg3, cvPoint((int)(Cgreen.x), (int)(Cgreen.y)), 15, CV_RGB(0,255,0),4); 

		
		//�����뵱ǰĿ���С�ڸ���ֵʱ��Ϊ������Ѿ��ߵ���ȡ��һ��Ŀ���
		if (tmpDistance<70) next=nextPoint(corners,Cgreen);
		
		if (int(next.x)==0 && int(next.y==0))break;   //����ȫ���˳�
		tmpDistance=sqrt((next.x-Cgreen.x)*(next.x-Cgreen.x)+(next.y-Cgreen.y)*(next.y-Cgreen.y));
		yaw=atan2((Cgreen.y-Cred.y),(Cgreen.x-Cred.x))/3.14;  
		direction=atan2((next.y-Cgreen.y),(next.x-Cgreen.x))/3.14;  
		cout<<yaw<<' '<<direction<<endl;
		if ((yaw-direction)<-0.12) {cout<<"right!!"; mycar.turnRight(); }
		else if((yaw-direction)>0.10) {cout<<"left!!"; mycar.turnLeft();}
		else { cout<<"runrunrun!!"; mycar.goForward ();}


        cvCircle(TrackImage, cvPoint((int)(next.x), (int)(next.y)), 6, CV_RGB(0,255,0),2); 
		cvCircle(transimg3, cvPoint((int)(next.x), (int)(next.y)), 6, CV_RGB(255,255,255),2); 
		
		cvShowImage("win1",TrackImage); //win1��ʾ·��ͼ		 
		cvShowImage("win2",transimg3);//win2��ʾС��ʵʱ״��


		if (cvWaitKey(1)==13) break; 
	}
	mycar.stop();
	cout<<"����ȫ�̣�"<<endl;
	
	cvWaitKey();
	cvReleaseCapture(&cam); //�ͷ�����ͷ���رճ���ǰҪ�ͷŸ�����Դ
	cvDestroyWindow("win1");
	return 0;
}