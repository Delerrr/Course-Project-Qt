#pragma once
#include "QuadTree.h"

/// <summary>
/// ���ݵ�ǰ����Ѱ�ҿ����ӵ��ź���õ�վ��(������α��վ)
/// </summary>
/// <param name="tree">�洢����վ����Ĳ����ṹ</param>
/// <param name="position">�õ��λ������</param>
/// <returns>��վ��, ���û�п����ӵ�վ�㣬�򷵻�վ�������ֵΪ-1</returns>
Station FindBestStation(const QuadTree& tree,const Point2& position);

class Time {
public:
	double hour = -1;
	double minute = -1;
	void AddMinute(double deltaMinute) {
		if (deltaMinute < 0) throw "deltaMinute ����С��0";
		minute += deltaMinute;
		double deltaHour = 0;
		while (minute > 60) {
			minute -= 60;
			deltaHour += 1;
		}
		hour += deltaHour;
	}

	double ToSecond() {
		return hour * 3600 + minute * 60;
	}

	bool operator<(const Time& other) {
		return hour * 60 + minute < other.hour * 60 + other.minute;
	}

	bool operator>(const Time& other) {
		return hour * 60 + minute > other.hour * 60 + other.minute;
	}

	bool operator==(const Time& other) {
		return hour * 60 + minute == other.hour * 60 + other.minute;
	}

	bool operator<=(const Time& other) {
		return !(this->operator>(other));
	}

	bool operator>=(const Time& other) {
		return !(this->operator<(other));
	}

	Time(const Time& other) {
		hour = other.hour;
		minute = other.minute;
	}

	Time operator-(const Time& other) const {
		Time ret = *this;
		ret.hour -= other.hour;
		ret.minute -= other.minute;
		if (ret.minute < 0) {
			ret.minute += 60;
			ret.hour -= 1;
		}
		return ret;
	}

	double ToMinutes() {
		return hour * 60 + minute;
	}

	Time(){}
};

//һ���ƶ���Ϣ
class DeltaPath {
public:
	//��ʼλ��
	Point2 from;
	//����λ��
	Point2 to;
	//�ٶ�:����ÿСʱ
	double velocity = -1;
	//��ʼʱ��
	Time startTime;
	//�Ƿ���α��վ
	bool isPseudo = false;
	//α��վ���
	int index = -1;
};

/// <summary>
/// �ƶ��켣
/// </summary>
class MovePath {
public:
	vector<DeltaPath> paths;
	MovePath(string fileName);
	//��ʼ��
	void Init(string fileName);
	MovePath() {}
};

/// <summary>
/// ��ʱ���Ϻ�վ��
/// </summary>
struct ConnectInfo {
	Time connectTime;
	Point2 position;
	Station connectStation;
};


//�ص���վ���ʱ��
struct OverlapInfo {
	vector<Station> stations;
	Time totalTime;
};

/// <summary>
/// �����ƶ��ն��ƶ��켣������վ�����������(������α��վ)
/// </summary>
/// <param name="movePath">�ƶ��ն˵��ƶ��켣</param>
/// <param name="tree">��վ��ͼ</param>
/// <param name="deltaPos">������뾫�ȣ��ף�</param>
/// <returns>����վ����Ϣ����</returns>
vector<ConnectInfo> CalcConnection(const MovePath& movePath,const QuadTree& tree, double deltaPos = 1);

/// <summary>
/// ����ĳһ��·���״��ź��ص�ʱ�䳤��(��3�Ρ���6��)
/// �����ص�����վ��ĿΪ2�ұ��ʼ�ղ���
/// </summary>
/// <param name="movePath">�ƶ�·��</param>
/// <param name="pathIndex">·��������2��5��</param>
/// <param name="tree">��վ��ͼ</param>
/// <param name="deltaPos">���㾫�ȣ��ף�</param>
/// <returns>�ź��ص���վ����ص���ʱ��</returns>
OverlapInfo CalcOverlap(const MovePath& movePath, const unsigned& pathIndex, const QuadTree& tree, double deltaPos = 1);

/// </summary>
/// <returns>����վ����Ϣ����</reurns>

/// <summary>
/// �����ƶ��ն��ƶ��켣������α��վ���������
/// </summary>
/// <param name="movePath">�ƶ��ն˵��ƶ��켣</param>
/// <param name="pseudoPath">α��վ���ƶ��켣</param>
/// <param name="deltaTime">������뾫�ȣ��ף�</param>
/// <param name="pathIndex">������һ�Σ�11��9��</param>
/// <param name="deltaPos">������뾫�ȣ��ף�</param>
/// <returns>�������</returns>
vector<ConnectInfo> PseudoConnection(const MovePath& movePath, const MovePath& pseudoPath, const unsigned pathIndex, double deltaPos = 0.1);

struct ConnectFirst {
	Station station;
	Point2 A1;
	Point2 A2;
	Time T1;
	Time T2;
};

/// <summary>
/// ����1���ƶ��켣�����������׸���վ�ź���Ч��Χ��ʱ�䣬�����뿪�׸���վ�ź���Ч��Χ��ʱ��
/// </summary>
/// <param name="movePath">�ƶ��ն˵��ƶ��켣</param>
/// <param name="tree">��վ��ͼ</param>
/// <param name="deltaTime">������뾫�ȣ��ף�</param>
/// <returns>ʱ������ӵ�վ��</reurns>
ConnectFirst CalcFirstConnection(const MovePath& movePath,const QuadTree& tree);



