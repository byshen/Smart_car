#include <iostream>

using namespace std;

class Car
{
	CSerialPort connect;
	unsigned char* data;
public:
	// 设置端口
	bool setPort(UINT port);
	// 断开连接
	bool release() { return connect.CloseListenTread(); }
	// 向前进
	void goForward();
	// 向后退
	void goBackward();
	// 向左转
	void turnLeft();
	// 向右转
	void turnRight();
	// 停止
	void stop();
};
bool Car::setPort(UINT port) 
{
	if (!connect.InitPort(port)) 
	{
		return false;
	}
	
	if (!connect.OpenListenThread()) 
	{
		cout << "开启监听线程失败" << endl;
		return false;
	} 
	else 
	{
		cout << "开启监听线程成功" << endl;
	}
	return true;
}

void Car::goForward() 
{
	data = new unsigned char('A');
	connect.WriteData(data,1);
	delete data;
}

void Car::goBackward() 
{
	data = new unsigned char('B');
	connect.WriteData(data,1);
	delete data;
}

void Car::turnLeft() 
{
	data = new unsigned char('L');
	connect.WriteData(data,1);
	delete data;
}

void Car::turnRight() 
{
	data = new unsigned char('R');
	connect.WriteData(data,1);
	delete data;
}

void Car::stop() 
{
	data = new unsigned char('P');
	connect.WriteData(data,1);
	connect.WriteData(data,1);
	connect.WriteData(data,1);
	connect.WriteData(data,1);
	connect.WriteData(data,1);
	delete data;
}