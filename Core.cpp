#include "Core.h"
#include <cmath>
#include <fstream>
#include <algorithm>
//#define DEBUG
#ifdef DEBUG
#include <iostream>
#include <chrono>
#include <ratio>
#include <thread>
#endif // DEBUG

/// <summary>
/// 计算信号强度
/// </summary>
/// <param name="radius">有效半径</param>
/// <param name="distance">实际距离</param>
/// <returns>计算结果</returns>
double CalcSignal(double radius, double distance) {
	if (abs(distance - 0) < 0.001) distance = 0.001;
	return radius * radius / distance / distance;
}

//基准强度为1时的各区域基站的有效半径
double BaseRadius[4] = { 300.0, 1000.0, 5000.0, 40.0 };
string stationType[4] = { "城区", "乡镇", "高速", "伪基站" };

/// <summary>
/// 根据当前坐标寻找可连接的信号最好的站点(不考虑伪基站)
/// </summary>
/// <param name="tree">存储所有站点的四叉树结构</param>
/// <param name="position">该点的位置坐标</param>
/// <returns>该站点, 如果没有可连接的站点，则返回站点的索引值为-1</returns>
Station FindBestStation(const QuadTree& tree,const Point2& position) {
#ifdef DEBUG
	int totalnum = 0;
	vector<int> indexs;
#endif // 

	Station ret;
	//是否找到可连接的站点
	bool find = false;
	//搜索区域中所有站点的最大信号倍率
	double maxSignal = 0;
	//搜索半径
	double maxRadius = *max_element(BaseRadius, BaseRadius + 4) * sqrt(tree.maxBaseSignalStrength);

	//搜索区域
	Point2 leftBottom(position.x - maxRadius, position.y - maxRadius);
	Point2 rightTop(position.x + maxRadius, position.y + maxRadius);

	vector<Station> stations(tree.Find(leftBottom, rightTop));

	for (Station& station : stations) {
		for (int i = 0; i < 3; ++i) {
			//在有效通信范围内，且信号为最强
			if (station.stationType == stationType[i])
			{
				double signal = CalcSignal(BaseRadius[i] * sqrt(station.signalStrength), station.coordinate.Dis(position));
				if (signal >= 1 && signal > maxSignal) {
#ifdef DEBUG
					totalnum++;
					indexs.push_back(station.index);
#endif // DEBUG

					find = true;
					maxSignal = signal;
					ret = station;
				}
			}
		}
	}

	if (!find) {
		ret.index = -1;
	}

#ifdef DEBUG
	if (totalnum > 1) {
		cout << "totalnum: " << totalnum << ", indexs: ";
		copy(indexs.begin(), indexs.end(), ostream_iterator<int>(cout, ", ")); 
		cout<< endl;
	}
#endif // DEBUG

	return ret;
}

void MovePath::Init(string fileName) {
	paths.clear();
	//伪基站轨迹
	bool pseudo = false;
	ifstream ifs(fileName);
	if (!ifs.is_open()) {
		throw "打开文件" + fileName + "失败";
	}

	string buf;
	//读掉开头的文件名
	getline(ifs, buf);
	if (buf.find("WZ") != string::npos) {
		pseudo = true;
	} else if (buf.find("YD") == string::npos) {
		throw "不是轨迹文件";
	}

	DeltaPath path;
	//处理逗号
	char comma;
	//伪基站
	if (pseudo) {
		ifs >> path.from.x >> comma >> path.from.y;
		while (!EndInput(path.from)) {
			ifs >> comma >> path.to.x
				>> comma >> path.to.y
				>> comma >> path.velocity
				>> comma >> path.startTime.hour
				>> comma >> path.startTime.minute
				>> comma >> path.index;
			path.isPseudo = true;
			paths.push_back(path);
			ifs >> path.from.x >> comma >> path.from.y;
		}
	} else {
		//不是伪基站
		ifs >> path.from.x >> comma >> path.from.y;
		while (!EndInput(path.from)) {
			ifs >> comma >> path.to.x
				>> comma >> path.to.y
				>> comma >> path.velocity
				>> comma >> path.startTime.hour
				>> comma >> path.startTime.minute;

			paths.push_back(path);
			ifs >> path.from.x >> comma >> path.from.y;
		}
	}
}

MovePath::MovePath(string fileName) {
	//伪基站轨迹
	bool pseudo = false;
	ifstream ifs(fileName);
	if (!ifs.is_open()) {
		throw "打开文件" + fileName + "失败";
	}

	string buf;
	//读掉开头的文件名
	getline(ifs, buf);
	if (buf.find("WZ") != string::npos) {
		pseudo = true;
	} else if (buf.find("YD") == string::npos) {
		throw "不是轨迹文件";
	}

	DeltaPath path;
	//处理逗号
	char comma;
	//伪基站
	if (pseudo) {
		ifs >> path.from.x >> comma >> path.from.y;
		while (!EndInput(path.from)) {
			ifs >> comma >> path.to.x
				>> comma >> path.to.y
				>> comma >> path.velocity
				>> comma >> path.startTime.hour
				>> comma >> path.startTime.minute
				>> comma >> path.index;
			path.isPseudo = true;
			paths.push_back(path);
			ifs >> path.from.x >> comma >> path.from.y;
		}
	} else {
		//不是伪基站
		ifs >> path.from.x >> comma >> path.from.y;
		while (!EndInput(path.from)) {
			ifs >> comma >> path.to.x
				>> comma >> path.to.y
				>> comma >> path.velocity
				>> comma >> path.startTime.hour
				>> comma >> path.startTime.minute;

			paths.push_back(path);
			ifs >> path.from.x >> comma >> path.from.y;
		}
	}
}

#ifdef DEBUG
int cnt = 0;
#endif
/// <summary>
/// 根据移动终端移动轨迹，计算站点的链接序列(不考虑伪基站)
/// </summary>
/// <param name="movePath">移动终端的移动轨迹</param>
/// <param name="tree">基站地图</param>
/// <param name="deltaPos">计算距离精度（米）</param>
/// <returns>连接站点信息序列</returns>
vector<ConnectInfo> CalcConnection(const MovePath& movePath,const QuadTree& tree, double deltaPos) {
	vector<ConnectInfo> ret;
	if (movePath.paths.size() == 0) return ret;
	ConnectInfo info;
	info.connectStation.index = -2;
	for (DeltaPath deltaPath : movePath.paths) {
		//该段距离长度（米）
		double totalPos = deltaPath.from.Dis(deltaPath.to);
		info.connectTime = deltaPath.startTime;
		for (double t = 0; t < totalPos; t += deltaPos) {
#ifdef DEBUG
			//auto t1 = std::chrono::high_resolution_clock::now();
#endif // DEBUG
#ifdef d
			cnt++;
			if (cnt % 1000 == 0) {
				system("cls");
				cout << cnt;
			}
#endif
			Point2 norm = (deltaPath.to - deltaPath.from).Normalise();
			//根据当前位置寻找信号最好的站点
			Point2 newPos = deltaPath.from + norm * t;
			Station res = FindBestStation(tree, newPos);
			//没找到可连接的站点或切换到了新的站点
			if (res.index != info.connectStation.index) {
				info.connectStation = res;
				info.position = newPos;
				ret.push_back(info);
			}

			info.connectTime.AddMinute(deltaPos * 0.06 / deltaPath.velocity);
#ifdef DEBUG
			//auto t2 = std::chrono::high_resolution_clock::now();

			//// floating-point duration: no duration_cast needed
			//std::chrono::duration<double, std::milli> fp_ms = t2 - t1;

			//// integral duration: requires duration_cast
			//auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

			//// converting integral duration to integral duration of shorter divisible time unit:
			//// no duration_cast needed
			//std::chrono::duration<long, std::micro> int_usec = int_ms;

			//std::cout << "f() took " << fp_ms.count() << " ms\n";
#endif // DEBUG
		}
	}

	return ret;
}

/// <summary>
/// （迭代法）检查第1段移动轨迹，分析进入首个基站信号有效范围的时间，分析离开首个基站信号有效范围的时间
/// </summary>
/// <param name="movePath">移动终端的移动轨迹</param>
/// <param name="tree">基站地图</param>
/// <param name="deltaTime">计算距离精度（米）</param>
/// <returns>时间和连接的站点</reurns>
ConnectFirst CalcFirstConnection(const MovePath& movePath,const QuadTree& tree) {
	//初始精度/米
	double deltaPos = 10;
#ifdef DEBUG
	auto t1 = std::chrono::high_resolution_clock::now();
#endif // DEBUG
	ConnectFirst ret;
	DeltaPath deltaPath = movePath.paths[0];
	//A1, A2是粗略遍历得到的两个结果（进入信号范围和离信号范围）
	Point2 A1, A2;
	//该段距离长度（米）
	double totalPos = deltaPath.from.Dis(deltaPath.to);
	double t = 0;
	Point2 norm = (deltaPath.to - deltaPath.from).Normalise();
	for (; t < totalPos; t += deltaPos) {
		//根据当前位置寻找信号最好的站点
		Point2 newPos = deltaPath.from + norm * t;
		Station res = FindBestStation(tree, newPos);
		//进入信号范围
		if (res.index != -1) {
			A1 = newPos;
			ret.station = res;
			break;
		}
	}

	for (; t < totalPos; t += deltaPos) {
		//根据当前位置寻找信号最好的站点
		A2 = deltaPath.from + norm * t;
		Station res = FindBestStation(tree, A2);
		//离开信号范围
		if (res.index == -1) {
			break;
		}
	}

	//迭代计算
	//站点坐标
	Point2 stationPos = ret.station.coordinate;
	//计算有效半径
	double radius = 0;
	for (int i = 0; i < 3; ++i) {
		if (ret.station.stationType == stationType[i]) {
			radius = BaseRadius[i] * sqrt(ret.station.signalStrength);
			break;
		}
	}

	//迭代计算A1
	Point2 outerPos = deltaPath.from;
	Point2 innerPos = A2;
	Point2 midPos = (outerPos + innerPos) * 0.5;
	double dis = stationPos.Dis(midPos) - radius;
	while (abs(dis) > 0.1) {
		if (dis > 0) {
			outerPos = midPos;
		} else {
			innerPos = midPos;
		}
		midPos = (outerPos + innerPos) * 0.5;
		dis = stationPos.Dis(midPos) - radius;
	}

	ret.A1 = midPos;
	//迭代计算A2
	outerPos = deltaPath.to;
	innerPos = A1;
	midPos = (outerPos + innerPos) * 0.5;
	dis = stationPos.Dis(midPos) - radius;
	while (abs(dis) > 0.1) {
		if (dis > 0) {
			outerPos = midPos;
		} else {
			innerPos = midPos;
		}
		midPos = (outerPos + innerPos) * 0.5;
		dis = stationPos.Dis(midPos) - radius;
	}
	ret.A2 = midPos;
#ifdef DEBUG
			auto t2 = std::chrono::high_resolution_clock::now();

			// floating-point duration: no duration_cast needed
			std::chrono::duration<double, std::milli> fp_ms = t2 - t1;

			// integral duration: requires duration_cast
			auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

			// converting integral duration to integral duration of shorter divisible time unit:
			// no duration_cast needed
			std::chrono::duration<long, std::micro> int_usec = int_ms;

			std::cout << "f() took " << fp_ms.count() << " ms\n";
#endif // DEBUG
			ret.T1 = deltaPath.startTime; 
			ret.T2 = deltaPath.startTime; 
			ret.T1.AddMinute(deltaPath.from.Dis(ret.A1) / deltaPath.velocity * 0.06);
			ret.T2.AddMinute(deltaPath.from.Dis(ret.A2) / deltaPath.velocity * 0.06);
	return ret;
}

/// <summary>
/// 计算当前位置、当前时间是否会连接伪基站
/// </summary>
/// <param name="position">当前位置</param>
/// <param name="time">当前时间</param>
/// <param name="pseudoPath">伪基站移动数据</param>
/// <returns>连接的伪基站vector，如果没有则vector的size为0</returns>
vector<Station> FindPseudo(const Point2& position, const Time& time,const MovePath& pseudoPaths) {
	vector<Station> ret;
	Station station;
	station.index = -1;
	for (DeltaPath pseudoPath : pseudoPaths.paths) {
		if (pseudoPath.startTime <= time) {
			double dis = pseudoPath.from.Dis(pseudoPath.to);
			Time endTime = pseudoPath.startTime;
			endTime.AddMinute(dis / pseudoPath.velocity / (50.0 / 3));
			//time处于伪基站的移动时间范围内
			if (endTime >= time) {
				//移动方向
				Point2 norm = (pseudoPath.to - pseudoPath.from).Normalise();
				//计算伪基站移动时间
				double pseudoMoveTime = (time - pseudoPath.startTime).ToMinutes();
				//伪基站此刻的位置
				Point2 newPos = pseudoPath.from + norm * pseudoPath.velocity * pseudoMoveTime * (50.0 / 3);
				if (newPos.Dis(position) <= BaseRadius[3]) {
					station.coordinate = newPos;
					station.index = pseudoPath.index;
					ret.push_back(station);
				}
			}
		}
	}
	if (ret.size() == 0) ret.push_back(station);
	return ret;
}

/// <summary>
/// 根据移动终端移动轨迹，计算伪基站的连接情况(假设同时最多只有一个伪基站被连上）
/// </summary>
/// <param name="movePath">移动终端的移动轨迹</param>
/// <param name="pseudoPath">伪基站的移动轨迹</param>
/// <param name="deltaTime">计算距离精度（米）</param>
/// <param name="pathIndex">计算哪一段（11、9）</param>
/// <param name="deltaPos">计算距离精度（米）</param>
/// <returns>连接情况</returns>
vector<ConnectInfo> PseudoConnection(const MovePath& movePath, const MovePath& pseudoPath, const unsigned pathIndex, double deltaPos) {
	vector<ConnectInfo> ret;
	if (movePath.paths.size() == 0) return ret;
	ConnectInfo info;
	info.connectStation.index = -1;
	//该段距离长度（米）
	DeltaPath path = movePath.paths[pathIndex];
	double totalPos = path.from.Dis(path.to);
	info.connectTime = path.startTime;
	for (double t = 0; t < totalPos; t += deltaPos) {
		//if (ret.size() == 2) break;
		Point2 norm = (path.to - path.from).Normalise();
		//根据当前位置寻找信号最好的站点
		Point2 newPos = path.from + norm * t;
		vector<Station> res = FindPseudo(newPos, info.connectTime, pseudoPath);
		//找到伪基站
		if (res.size() > 1) {
			//假定同时只能找到一个伪基站
			throw "伪基站的数目大于1，出乎意料";
		}

		if (info.connectStation.index != res[0].index) {
			info.connectStation = res[0];
			info.position = newPos;
			ret.push_back(info);
		}

		info.connectTime.AddMinute(deltaPos * 0.06 / path.velocity);
	}

	return ret;
}

/// <summary>
/// 根据当前坐标寻找可连接的站点(不考虑伪基站)
/// </summary>
/// <param name="tree">存储所有站点的四叉树结构</param>
/// <param name="position">该点的位置坐标</param>
/// <returns>站点</returns>
vector<Station> FindStations(const QuadTree& tree, const Point2& position) {
	vector<Station> ret;
	//搜索半径
	double maxRadius = *max_element(BaseRadius, BaseRadius + 4) * sqrt(tree.maxBaseSignalStrength);

	//搜索区域
	Point2 leftBottom(position.x - maxRadius, position.y - maxRadius);
	Point2 rightTop(position.x + maxRadius, position.y + maxRadius);

	vector<Station> stations(tree.Find(leftBottom, rightTop));

	for (Station& station : stations) {
		for (int i = 0; i < 3; ++i) {
			//在有效通信范围内
			if (station.stationType == stationType[i])
			{
				double signal = CalcSignal(BaseRadius[i] * sqrt(station.signalStrength), station.coordinate.Dis(position));
				if (signal >= 1) {
					ret.push_back(station);
				}
			}
		}
	}

	return ret;
}

/// <summary>
/// 计算某一段路径首次信号重叠时间长度(第3段、第6段)
/// 假设重叠区基站数目为2且编号始终不变
/// </summary>
/// <param name="movePath">移动路径</param>
/// <param name="pathIndex">路径索引（2、5）</param>
/// <param name="tree">基站地图</param>
/// <param name="deltaPos">计算精度（米）</param>
/// <returns>信号重叠区站点和重叠区时长</returns>
OverlapInfo CalcOverlap(const MovePath& movePath, const unsigned& pathIndex, const QuadTree& tree, double deltaPos) {
	OverlapInfo ret;
	//该段距离长度（米）
	DeltaPath path = movePath.paths[pathIndex];
	double totalPos = path.from.Dis(path.to);

	//开始进入重叠区的时间
	Time totalTime = path.startTime;
	ret.totalTime = totalTime;
	//是否开始进入重叠区
	bool enterOverlap = false;
	for (double t = 0; t < totalPos; t += deltaPos) {
		Point2 norm = (path.to - path.from).Normalise();
		//根据当前位置寻找信号最好的站点
		Point2 newPos = path.from + norm * t;

		vector<Station> stations(FindStations(tree, newPos));
		//开始进入重叠区
		if (!enterOverlap && stations.size() == 2) {
			enterOverlap = true;
			ret.stations.push_back(stations[0]);
			ret.stations.push_back(stations[1]);
		}

		if (enterOverlap) {
			if (stations.size() == 0) {
				break;
			}
			ret.totalTime.AddMinute(deltaPos * 0.06 / path.velocity);
		}

		if (!enterOverlap) {
			totalTime.AddMinute(deltaPos * 0.06 / path.velocity);
			ret.totalTime = totalTime;
		}
	}

	ret.totalTime = ret.totalTime - totalTime;
	return ret;
}
