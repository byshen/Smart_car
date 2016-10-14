#include <iostream>

using namespace std;

class Car
{
	CSerialPort connect;
	unsigned char* data;
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
bool Car::setPort(UINT port) 
{
	if (!connect.InitPort(port)) 
	{
		return false;
	}
	
	if (!connect.OpenListenThread()) 
	{
		cout << "���������߳�ʧ��" << endl;
		return false;
	} 
	else 
	{
		cout << "���������̳߳ɹ�" << endl;
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