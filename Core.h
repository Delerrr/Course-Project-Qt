#pragma once
#include "QuadTree.h"

/// <summary>
/// 根据当前坐标寻找可连接的信号最好的站点(不考虑伪基站)
/// </summary>
/// <param name="tree">存储所有站点的四叉树结构</param>
/// <param name="position">该点的位置坐标</param>
/// <returns>该站点, 如果没有可连接的站点，则返回站点的索引值为-1</returns>
Station FindBestStation(const QuadTree& tree,const Point2& position);

class Time {
public:
	double hour = -1;
	double minute = -1;
	void AddMinute(double deltaMinute) {
		if (deltaMinute < 0) throw "deltaMinute 不能小于0";
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

//一段移动信息
class DeltaPath {
public:
	//起始位置
	Point2 from;
	//结束位置
	Point2 to;
	//速度:公里每小时
	double velocity = -1;
	//开始时间
	Time startTime;
	//是否是伪基站
	bool isPseudo = false;
	//伪基站编号
	int index = -1;
};

/// <summary>
/// 移动轨迹
/// </summary>
class MovePath {
public:
	vector<DeltaPath> paths;
	MovePath(string fileName);
	//初始化
	void Init(string fileName);
	MovePath() {}
};

/// <summary>
/// 何时连上何站点
/// </summary>
struct ConnectInfo {
	Time connectTime;
	Point2 position;
	Station connectStation;
};


//重叠区站点和时长
struct OverlapInfo {
	vector<Station> stations;
	Time totalTime;
};

/// <summary>
/// 根据移动终端移动轨迹，计算站点的链接序列(不考虑伪基站)
/// </summary>
/// <param name="movePath">移动终端的移动轨迹</param>
/// <param name="tree">基站地图</param>
/// <param name="deltaPos">计算距离精度（米）</param>
/// <returns>连接站点信息序列</returns>
vector<ConnectInfo> CalcConnection(const MovePath& movePath,const QuadTree& tree, double deltaPos = 1);

/// <summary>
/// 计算某一段路径首次信号重叠时间长度(第3段、第6段)
/// 假设重叠区基站数目为2且编号始终不变
/// </summary>
/// <param name="movePath">移动路径</param>
/// <param name="pathIndex">路径索引（2、5）</param>
/// <param name="tree">基站地图</param>
/// <param name="deltaPos">计算精度（米）</param>
/// <returns>信号重叠区站点和重叠区时长</returns>
OverlapInfo CalcOverlap(const MovePath& movePath, const unsigned& pathIndex, const QuadTree& tree, double deltaPos = 1);

/// </summary>
/// <returns>连接站点信息序列</reurns>

/// <summary>
/// 根据移动终端移动轨迹，计算伪基站的连接情况
/// </summary>
/// <param name="movePath">移动终端的移动轨迹</param>
/// <param name="pseudoPath">伪基站的移动轨迹</param>
/// <param name="deltaTime">计算距离精度（米）</param>
/// <param name="pathIndex">计算哪一段（11、9）</param>
/// <param name="deltaPos">计算距离精度（米）</param>
/// <returns>连接情况</returns>
vector<ConnectInfo> PseudoConnection(const MovePath& movePath, const MovePath& pseudoPath, const unsigned pathIndex, double deltaPos = 0.1);

struct ConnectFirst {
	Station station;
	Point2 A1;
	Point2 A2;
	Time T1;
	Time T2;
};

/// <summary>
/// 检查第1段移动轨迹，分析进入首个基站信号有效范围的时间，分析离开首个基站信号有效范围的时间
/// </summary>
/// <param name="movePath">移动终端的移动轨迹</param>
/// <param name="tree">基站地图</param>
/// <param name="deltaTime">计算距离精度（米）</param>
/// <returns>时间和连接的站点</reurns>
ConnectFirst CalcFirstConnection(const MovePath& movePath,const QuadTree& tree);



