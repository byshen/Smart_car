#include "BasicFunctions.h"
#include <cmath>

// 距离的平凡
// 重载
double distsquare(CvPoint2D32f* p1, CvPoint2D32f* p2) {
	return (p1->x - p2->x)*(p1->x - p2->x) + (p1->y - p2->y)*(p1->y - p2->y);
}
double distsquare(CvPoint* p1, CvPoint* p2) {
	return (p1->x - p2->x)*(p1->x - p2->x) + (p1->y - p2->y)*(p1->y - p2->y);
}
double distsquare(CvPoint2D32f* p1, CvPoint* p2) {
	return (p1->x - p2->x)*(p1->x - p2->x) + (p1->y - p2->y)*(p1->y - p2->y);
}

double prod(CvPoint2D32f* p1, CvPoint2D32f* p2) {
	return p1->x*p2->y - p2->x*p1->y;
}

// 模的平方
double lensquare(CvPoint2D32f* p) {
	return p->x*p->x + p->y*p->y;
}

// 是否在一条线上
bool inLine(CvPoint* l1, CvPoint* l2, CvPoint2D32f* p, double sin_theta_sq) {
	if (distsquare(p, l1) > distsquare(p, l2)) {
		CvPoint* tmp = l1;
		l1 = l2;
		l2 = tmp;
	}
	double lx = l1->x - l2->x, ly = l1->y - l2->y;
	double px = p->x - l2->x, py = p->y - l2->y;

	if ((lx*py - ly*px)*(lx*py - ly*px) < (lx*lx + ly*ly)*(px*px + py*py)*sin_theta_sq) {
		return true;
	}
	return false;
}

double distsquare(double x1,double y1,double x2,double y2)
{
	return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}
double distsquare(double x1,double y1)
{
	return (x1*x1+y1*y1);
}
double dist(double x1,double y1,double x2,double y2)
{
	return sqrt(distsquare( x1, y1, x2, y2));
}
double angle(double dvx,double dvy,double ovx,double ovy)
{
	double tmp=(dvx*ovx)+(dvy*ovy);
	double fenmu=distsquare(dvx,dvy)*distsquare(ovx,ovy);
	fenmu=sqrt(fenmu);
	double cos_=tmp/fenmu;
	double waiji=dvx*ovy-dvy*ovx;
	if (waiji>0)//右转
		return -abs(acos(cos_));
	else 
		return abs(acos(cos_));

	
}

string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}