#pragma once
#include <string>
#include <cmath>
using namespace std;
/*
* ����һЩ�������ʵĽṹ�ͺ���
*/
/// <summary>
/// һ���򵥵Ķ�ά����ṹ
/// </summary>
class Point2 {
public:
	double x;
	double y;
	Point2() {
		x = 0;
		y = 0;
	}

	Point2(double _x, double _y) : x(_x), y(_y) {}

	Point2(const Point2& other) { 
		x = other.x; 
		y = other.y;
	}

	Point2 operator+(const Point2& other) {
		return Point2(x + other.x, y + other.y);
	}

	Point2 operator-(const Point2& other) {
		return Point2(x - other.x, y - other.y);
	}

	double Dis(const Point2& other) {
		return sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
	}

	//������׼��
	Point2 Normalise() {
		double dis = sqrt(x * x +  y * y);
		return Point2(x / dis, y / dis);
	}

	Point2 operator*(double rate) {
		return Point2(x * rate, y * rate);
	}
};

/// <summary>
/// ��վ
/// </summary>
struct Station {
	// ����
	Point2 coordinate;
	// ����/����/����
	string stationType = "undefined";
	// �źŹ���ǿ��
	double signalStrength = 1;
	// ���
	int index;
	Station(const Station& other) {
		coordinate = other.coordinate;
		stationType = other.stationType;
		signalStrength = other.signalStrength;
		index = other.index;
	}
	Station() {
		index = -1;
	}
};


/// <summary>
/// �жϵ�point�Ƿ���leftBottom��rightTop��ɵ�������
/// </summary>
/// <param name="point">��Ҫ�жϵĵ������</param>
/// <param name="leftBottom">�������½�����</param>
/// <param name="rightTop">�������Ͻ�����</param>
bool IsInRegion(Point2 point, Point2 leftBottom, Point2 rightTop, int index);

/// <summary>
/// �ж��������������Ƿ��ص�
/// </summary>
bool IsOverlapped(Point2 leftBottom1, Point2 rightTop1, Point2 leftBottom2, Point2 rightTop2);
