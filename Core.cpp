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
/// �����ź�ǿ��
/// </summary>
/// <param name="radius">��Ч�뾶</param>
/// <param name="distance">ʵ�ʾ���</param>
/// <returns>������</returns>
double CalcSignal(double radius, double distance) {
	if (abs(distance - 0) < 0.001) distance = 0.001;
	return radius * radius / distance / distance;
}

//��׼ǿ��Ϊ1ʱ�ĸ������վ����Ч�뾶
double BaseRadius[4] = { 300.0, 1000.0, 5000.0, 40.0 };
string stationType[4] = { "����", "����", "����", "α��վ" };

/// <summary>
/// ���ݵ�ǰ����Ѱ�ҿ����ӵ��ź���õ�վ��(������α��վ)
/// </summary>
/// <param name="tree">�洢����վ����Ĳ����ṹ</param>
/// <param name="position">�õ��λ������</param>
/// <returns>��վ��, ���û�п����ӵ�վ�㣬�򷵻�վ�������ֵΪ-1</returns>
Station FindBestStation(const QuadTree& tree,const Point2& position) {
#ifdef DEBUG
	int totalnum = 0;
	vector<int> indexs;
#endif // 

	Station ret;
	//�Ƿ��ҵ������ӵ�վ��
	bool find = false;
	//��������������վ�������źű���
	double maxSignal = 0;
	//�����뾶
	double maxRadius = *max_element(BaseRadius, BaseRadius + 4) * sqrt(tree.maxBaseSignalStrength);

	//��������
	Point2 leftBottom(position.x - maxRadius, position.y - maxRadius);
	Point2 rightTop(position.x + maxRadius, position.y + maxRadius);

	vector<Station> stations(tree.Find(leftBottom, rightTop));

	for (Station& station : stations) {
		for (int i = 0; i < 3; ++i) {
			//����Чͨ�ŷ�Χ�ڣ����ź�Ϊ��ǿ
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
	//α��վ�켣
	bool pseudo = false;
	ifstream ifs(fileName);
	if (!ifs.is_open()) {
		throw "���ļ�" + fileName + "ʧ��";
	}

	string buf;
	//������ͷ���ļ���
	getline(ifs, buf);
	if (buf.find("WZ") != string::npos) {
		pseudo = true;
	} else if (buf.find("YD") == string::npos) {
		throw "���ǹ켣�ļ�";
	}

	DeltaPath path;
	//������
	char comma;
	//α��վ
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
		//����α��վ
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
	//α��վ�켣
	bool pseudo = false;
	ifstream ifs(fileName);
	if (!ifs.is_open()) {
		throw "���ļ�" + fileName + "ʧ��";
	}

	string buf;
	//������ͷ���ļ���
	getline(ifs, buf);
	if (buf.find("WZ") != string::npos) {
		pseudo = true;
	} else if (buf.find("YD") == string::npos) {
		throw "���ǹ켣�ļ�";
	}

	DeltaPath path;
	//������
	char comma;
	//α��վ
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
		//����α��վ
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
/// �����ƶ��ն��ƶ��켣������վ�����������(������α��վ)
/// </summary>
/// <param name="movePath">�ƶ��ն˵��ƶ��켣</param>
/// <param name="tree">��վ��ͼ</param>
/// <param name="deltaPos">������뾫�ȣ��ף�</param>
/// <returns>����վ����Ϣ����</returns>
vector<ConnectInfo> CalcConnection(const MovePath& movePath,const QuadTree& tree, double deltaPos) {
	vector<ConnectInfo> ret;
	if (movePath.paths.size() == 0) return ret;
	ConnectInfo info;
	info.connectStation.index = -2;
	for (DeltaPath deltaPath : movePath.paths) {
		//�öξ��볤�ȣ��ף�
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
			//���ݵ�ǰλ��Ѱ���ź���õ�վ��
			Point2 newPos = deltaPath.from + norm * t;
			Station res = FindBestStation(tree, newPos);
			//û�ҵ������ӵ�վ����л������µ�վ��
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
/// ��������������1���ƶ��켣�����������׸���վ�ź���Ч��Χ��ʱ�䣬�����뿪�׸���վ�ź���Ч��Χ��ʱ��
/// </summary>
/// <param name="movePath">�ƶ��ն˵��ƶ��켣</param>
/// <param name="tree">��վ��ͼ</param>
/// <param name="deltaTime">������뾫�ȣ��ף�</param>
/// <returns>ʱ������ӵ�վ��</reurns>
ConnectFirst CalcFirstConnection(const MovePath& movePath,const QuadTree& tree) {
	//��ʼ����/��
	double deltaPos = 10;
#ifdef DEBUG
	auto t1 = std::chrono::high_resolution_clock::now();
#endif // DEBUG
	ConnectFirst ret;
	DeltaPath deltaPath = movePath.paths[0];
	//A1, A2�Ǵ��Ա����õ�����������������źŷ�Χ�����źŷ�Χ��
	Point2 A1, A2;
	//�öξ��볤�ȣ��ף�
	double totalPos = deltaPath.from.Dis(deltaPath.to);
	double t = 0;
	Point2 norm = (deltaPath.to - deltaPath.from).Normalise();
	for (; t < totalPos; t += deltaPos) {
		//���ݵ�ǰλ��Ѱ���ź���õ�վ��
		Point2 newPos = deltaPath.from + norm * t;
		Station res = FindBestStation(tree, newPos);
		//�����źŷ�Χ
		if (res.index != -1) {
			A1 = newPos;
			ret.station = res;
			break;
		}
	}

	for (; t < totalPos; t += deltaPos) {
		//���ݵ�ǰλ��Ѱ���ź���õ�վ��
		A2 = deltaPath.from + norm * t;
		Station res = FindBestStation(tree, A2);
		//�뿪�źŷ�Χ
		if (res.index == -1) {
			break;
		}
	}

	//��������
	//վ������
	Point2 stationPos = ret.station.coordinate;
	//������Ч�뾶
	double radius = 0;
	for (int i = 0; i < 3; ++i) {
		if (ret.station.stationType == stationType[i]) {
			radius = BaseRadius[i] * sqrt(ret.station.signalStrength);
			break;
		}
	}

	//��������A1
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
	//��������A2
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
/// ���㵱ǰλ�á���ǰʱ���Ƿ������α��վ
/// </summary>
/// <param name="position">��ǰλ��</param>
/// <param name="time">��ǰʱ��</param>
/// <param name="pseudoPath">α��վ�ƶ�����</param>
/// <returns>���ӵ�α��վvector�����û����vector��sizeΪ0</returns>
vector<Station> FindPseudo(const Point2& position, const Time& time,const MovePath& pseudoPaths) {
	vector<Station> ret;
	Station station;
	station.index = -1;
	for (DeltaPath pseudoPath : pseudoPaths.paths) {
		if (pseudoPath.startTime <= time) {
			double dis = pseudoPath.from.Dis(pseudoPath.to);
			Time endTime = pseudoPath.startTime;
			endTime.AddMinute(dis / pseudoPath.velocity / (50.0 / 3));
			//time����α��վ���ƶ�ʱ�䷶Χ��
			if (endTime >= time) {
				//�ƶ�����
				Point2 norm = (pseudoPath.to - pseudoPath.from).Normalise();
				//����α��վ�ƶ�ʱ��
				double pseudoMoveTime = (time - pseudoPath.startTime).ToMinutes();
				//α��վ�˿̵�λ��
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
/// �����ƶ��ն��ƶ��켣������α��վ���������(����ͬʱ���ֻ��һ��α��վ�����ϣ�
/// </summary>
/// <param name="movePath">�ƶ��ն˵��ƶ��켣</param>
/// <param name="pseudoPath">α��վ���ƶ��켣</param>
/// <param name="deltaTime">������뾫�ȣ��ף�</param>
/// <param name="pathIndex">������һ�Σ�11��9��</param>
/// <param name="deltaPos">������뾫�ȣ��ף�</param>
/// <returns>�������</returns>
vector<ConnectInfo> PseudoConnection(const MovePath& movePath, const MovePath& pseudoPath, const unsigned pathIndex, double deltaPos) {
	vector<ConnectInfo> ret;
	if (movePath.paths.size() == 0) return ret;
	ConnectInfo info;
	info.connectStation.index = -1;
	//�öξ��볤�ȣ��ף�
	DeltaPath path = movePath.paths[pathIndex];
	double totalPos = path.from.Dis(path.to);
	info.connectTime = path.startTime;
	for (double t = 0; t < totalPos; t += deltaPos) {
		//if (ret.size() == 2) break;
		Point2 norm = (path.to - path.from).Normalise();
		//���ݵ�ǰλ��Ѱ���ź���õ�վ��
		Point2 newPos = path.from + norm * t;
		vector<Station> res = FindPseudo(newPos, info.connectTime, pseudoPath);
		//�ҵ�α��վ
		if (res.size() > 1) {
			//�ٶ�ͬʱֻ���ҵ�һ��α��վ
			throw "α��վ����Ŀ����1����������";
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
/// ���ݵ�ǰ����Ѱ�ҿ����ӵ�վ��(������α��վ)
/// </summary>
/// <param name="tree">�洢����վ����Ĳ����ṹ</param>
/// <param name="position">�õ��λ������</param>
/// <returns>վ��</returns>
vector<Station> FindStations(const QuadTree& tree, const Point2& position) {
	vector<Station> ret;
	//�����뾶
	double maxRadius = *max_element(BaseRadius, BaseRadius + 4) * sqrt(tree.maxBaseSignalStrength);

	//��������
	Point2 leftBottom(position.x - maxRadius, position.y - maxRadius);
	Point2 rightTop(position.x + maxRadius, position.y + maxRadius);

	vector<Station> stations(tree.Find(leftBottom, rightTop));

	for (Station& station : stations) {
		for (int i = 0; i < 3; ++i) {
			//����Чͨ�ŷ�Χ��
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
/// ����ĳһ��·���״��ź��ص�ʱ�䳤��(��3�Ρ���6��)
/// �����ص�����վ��ĿΪ2�ұ��ʼ�ղ���
/// </summary>
/// <param name="movePath">�ƶ�·��</param>
/// <param name="pathIndex">·��������2��5��</param>
/// <param name="tree">��վ��ͼ</param>
/// <param name="deltaPos">���㾫�ȣ��ף�</param>
/// <returns>�ź��ص���վ����ص���ʱ��</returns>
OverlapInfo CalcOverlap(const MovePath& movePath, const unsigned& pathIndex, const QuadTree& tree, double deltaPos) {
	OverlapInfo ret;
	//�öξ��볤�ȣ��ף�
	DeltaPath path = movePath.paths[pathIndex];
	double totalPos = path.from.Dis(path.to);

	//��ʼ�����ص�����ʱ��
	Time totalTime = path.startTime;
	ret.totalTime = totalTime;
	//�Ƿ�ʼ�����ص���
	bool enterOverlap = false;
	for (double t = 0; t < totalPos; t += deltaPos) {
		Point2 norm = (path.to - path.from).Normalise();
		//���ݵ�ǰλ��Ѱ���ź���õ�վ��
		Point2 newPos = path.from + norm * t;

		vector<Station> stations(FindStations(tree, newPos));
		//��ʼ�����ص���
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
