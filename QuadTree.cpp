#include "QuadTree.h"
#include <iostream>
#include <fstream>
#include <algorithm>

/// <summary>
///�ж������Ƿ����
/// </summary>
bool EndInput(const Point2& inputPoint) {
	const double EPSILON = 0.0001;
	return fabs(inputPoint.x + 1) < EPSILON && 
			fabs(inputPoint.y + 1) < EPSILON;
}

/// <summary>
//�ȽϺ�����
/// </summary>
bool XComp(const Station& arg1,const Station& arg2) {
	return arg1.coordinate.x < arg2.coordinate.x;
}

/// <summary>
//�Ƚ�������
/// </summary>
bool YComp(const Station& arg1,const Station& arg2) {
	return arg1.coordinate.y < arg2.coordinate.y;
}

/// <summary>
/// �Ƚ��ź�ǿ��
/// </summary>
bool SignalComp(const Station& arg1,const Station& arg2) {
	return arg1.signalStrength < arg2.signalStrength;
}
QuadTree::QuadTree(string fileName) {
	ifstream ifs(fileName);
	if (!ifs.is_open()) {
		throw "���ļ�" + fileName + "ʧ��";
	}

	string buf;
	//������ͷ���ļ���
	getline(ifs, buf);
	if (buf.find("JZ") == string::npos) {
		throw "����վ���ļ�";
	}
	vector<Station> stations;
	Station inputStation;
	//������
	char comma;
	
	ifs >> inputStation.coordinate.x
		>> comma
		>> inputStation.coordinate.y;

	while (!EndInput(inputStation.coordinate))
	{
		//������
		ifs >> comma
			>> inputStation.stationType
			>> inputStation.signalStrength
			>> comma
			>> inputStation.index;

		stations.push_back(inputStation);

		ifs >> inputStation.coordinate.x
			>> comma
			>> inputStation.coordinate.y;
	}
	ifs.close();


	rootNode.leftBottomBorder.x = (*min_element(stations.begin(), stations.end(), XComp)).coordinate.x;
	rootNode.leftBottomBorder.y = (*min_element(stations.begin(), stations.end(), YComp)).coordinate.y;
	rootNode.rightTopBorder.x = (*max_element(stations.begin(), stations.end(), XComp)).coordinate.x;
	rootNode.rightTopBorder.y = (*max_element(stations.begin(), stations.end(), YComp)).coordinate.y;

	maxBaseSignalStrength = (*max_element(stations.begin(), stations.end(), SignalComp)).signalStrength;

	ConstructHelper(stations, rootNode, rootNode.depth);
	leftBottomBorder = rootNode.leftBottomBorder;
	rightTopBorder = rootNode.rightTopBorder;
}


/// <summary>
/// �����ļ���������
/// </summary>
/// <param name="fileName"></param>
void QuadTree::BuildTree(string fileName1, string fileName2) {
	DestructHelper(rootNode);
	ifstream ifs(fileName1);
	if (!ifs.is_open()) {
		throw "���ļ�" + fileName1 + "ʧ��";
	}
	string buf;
	//������ͷ���ļ���
	getline(ifs, buf);
	if (buf.find("JZ") == string::npos) {
		throw "����վ���ļ�";
	}

	rootNode.isLeaf = false;
	vector<Station> stations;
	Station inputStation;
	//������
	char comma;
	
	ifs >> inputStation.coordinate.x
		>> comma
		>> inputStation.coordinate.y;

	while (!EndInput(inputStation.coordinate))
	{
		//������
		ifs >> comma
			>> inputStation.stationType
			>> inputStation.signalStrength
			>> comma
			>> inputStation.index;

		stations.push_back(inputStation);

		ifs >> inputStation.coordinate.x
			>> comma
			>> inputStation.coordinate.y;
	}
	ifs.close();

	ifs.open(fileName2);
	if (!ifs.is_open()) {
		throw "���ļ�" + fileName2 + "ʧ��";
	}
	//������ͷ���ļ���
	getline(ifs, buf);
	if (buf.find("JZ") == string::npos) {
		throw "����վ���ļ�";
	}

	ifs >> inputStation.coordinate.x
		>> comma
		>> inputStation.coordinate.y;

	while (!EndInput(inputStation.coordinate))
	{
		//������
		ifs >> comma
			>> inputStation.stationType
			>> inputStation.signalStrength
			>> comma
			>> inputStation.index;

		stations.push_back(inputStation);

		ifs >> inputStation.coordinate.x
			>> comma
			>> inputStation.coordinate.y;
	}
	ifs.close();

	rootNode.leftBottomBorder.x = (*min_element(stations.begin(), stations.end(), XComp)).coordinate.x;
	rootNode.leftBottomBorder.y = (*min_element(stations.begin(), stations.end(), YComp)).coordinate.y;
	rootNode.rightTopBorder.x = (*max_element(stations.begin(), stations.end(), XComp)).coordinate.x;
	rootNode.rightTopBorder.y = (*max_element(stations.begin(), stations.end(), YComp)).coordinate.y;

	maxBaseSignalStrength = (*max_element(stations.begin(), stations.end(), SignalComp)).signalStrength;

	ConstructHelper(stations, rootNode, rootNode.depth);
	leftBottomBorder = rootNode.leftBottomBorder;
	rightTopBorder = rootNode.rightTopBorder;
}
void QuadTree::ConstructHelper(vector<Station>& stations, TreeNode& node, unsigned depth) {
	Point2 leftBottomBorder = node.leftBottomBorder;
	Point2 rightTopBorder = node.rightTopBorder;
	if (stations.size() <= maxStationsOfLeaf) {
		node.isLeaf = true;
		node.stations.assign(stations.begin(), stations.end());
		return;
	}
	node.isLeaf = false;
	vector<Station> substations[4];

	const int length = sizeof(node.subNodes) / sizeof(rootNode.subNodes[0]);
	for (int subIndex = 0; subIndex < length; ++subIndex) {
		node.subNodes[subIndex] = new TreeNode(depth + 1);
	}
	node.subNodes[0]->leftBottomBorder = leftBottomBorder;
	node.subNodes[0]->rightTopBorder.x = (leftBottomBorder.x + rightTopBorder.x) / 2;
	node.subNodes[0]->rightTopBorder.y = (leftBottomBorder.y + rightTopBorder.y) / 2;
	node.subNodes[1]->leftBottomBorder.x = node.subNodes[0]->rightTopBorder.x;
	node.subNodes[1]->leftBottomBorder.y = leftBottomBorder.y;
	node.subNodes[1]->rightTopBorder.x = rightTopBorder.x;
	node.subNodes[1]->rightTopBorder.y = node.subNodes[0]->rightTopBorder.y;
	node.subNodes[2]->leftBottomBorder = node.subNodes[0]->rightTopBorder;
	node.subNodes[2]->rightTopBorder = rightTopBorder;
	node.subNodes[3]->leftBottomBorder.x = leftBottomBorder.x;
	node.subNodes[3]->leftBottomBorder.y = node.subNodes[0]->rightTopBorder.y;
	node.subNodes[3]->rightTopBorder.x = node.subNodes[0]->rightTopBorder.x;
	node.subNodes[3]->rightTopBorder.y = rightTopBorder.y;

	for (Station i : stations) {
		for (int j = 0; j < 4; ++j) {
			if (IsInRegion(i.coordinate, node.subNodes[j]->leftBottomBorder, node.subNodes[j]->rightTopBorder, j)) {
				substations[j].push_back(i);
				continue;
			}
		}
	}
	for (int i = 0; i < 4; ++i) {
		node.subNodes[i]->parent = &node;
		node.subNodes[i]->pos = i;
		ConstructHelper(substations[i], *(node.subNodes[i]), depth + 1);
	}		
}

/// <summary>
/// ����ĳ�����
/// </summary>
vector<Station> QuadTree::TraverseNode(TreeNode node) {
	if (node.isLeaf) {
		return node.stations;
	}

	vector<Station> ret;
	for (TreeNode* i : node.subNodes) {
		vector<Station> temp = TraverseNode(*i);
		ret.insert(ret.end(), temp.begin(), temp.end());
	}

	return ret;
}


/// <summary>
/// ��������ĳ������
/// </summary>
/// <param name="index">Ϊ0,1,2,3, �ֱ��ʾ���½ǣ����ϣ������½ǣ����ϣ������Ͻǣ������������Ͻǣ�������</param>
/// <returns></returns>
vector<Station> QuadTree::TraverseTreeByOneDirection(int index) {
	return TraverseNodeByOneDirection(rootNode, index);
}

/// <summary>
/// ����ĳ����ĳ������
/// </summary>
/// <param name="node">Ҫ�����Ļ�׼���</param>
/// <param name="index">����Ϊ0,1,2,3, �ֱ��ʾ���½ǣ����ϣ������½ǣ����ϣ������Ͻǣ������������Ͻǣ�������</param>
/// <returns></returns>
vector<Station> QuadTree::TraverseNodeByOneDirection(TreeNode node, int index) {
	if (index > 3 || index < 0) throw "index ������";
	return TraverseNode(*(node.subNodes[index]));
}

/// <summary>
/// ���������ĸ�������
/// </summary>
/// <param name="node"></param>
void QuadTree::DestructHelper(TreeNode& node) {
	if (node.isLeaf) {
		return;
	}

	for (TreeNode* subNodePtr : node.subNodes) {
		DestructHelper(*subNodePtr);
		delete subNodePtr;
	}
	node.isLeaf = true;
}

/// <summary>
/// ���Һ����ĸ�������
/// </summary>
vector<Station> QuadTree::FindHelper(const Point2& leftBottom, const Point2& rightTop, TreeNode node) const {
	vector<Station> ret;
	if (node.isLeaf) {
		ret.assign(node.stations.begin(), node.stations.end());
	} else {
		for (TreeNode* subNode : node.subNodes) {
			//����ӽڵ�����ͱ������������ص���������ӽڵ㣬���򲻲���
			if (IsOverlapped(subNode->leftBottomBorder, subNode->rightTopBorder, leftBottom, rightTop)) {
				vector<Station> temp(FindHelper(leftBottom, rightTop, *subNode));
				ret.insert(ret.end(), temp.begin(), temp.end());
			}
		}
	}

	return ret;
}

/// <summary>
/// �����������򣬲��Ҹþ��������ڵ�վ��
/// </summary>
vector<Station> QuadTree::Find(const Point2& leftBottom, const Point2& rightTop) const {
 	return FindHelper(leftBottom, rightTop, rootNode);
}
 

/// <summary>
/// �õ�����/����/����/�����ǵ���Ҷ
/// </summary>
/// <param name="pos">0,1,2,3�ֱ��ʾ����/����/����/����</param>
QuadTree::TreeNode QuadTree::GetLeaf(unsigned pos) const {
	return GetLeafHelper(rootNode, pos);
}

const QuadTree::TreeNode& QuadTree::GetLeafHelper(const TreeNode& node, unsigned pos) const {
	if (node.isLeaf) {
		return node;
	}

	return GetLeafHelper(*(node.subNodes[pos]), pos);
}

/// <summary>
/// ��������/����/����/�����ǵ���Ҷ�Ļ�վ
/// </summary>
/// <param name="pos">0,1,2,3�ֱ��ʾ����/����/����/����</param>
vector<Station> QuadTree::FindSationsByOneDirection(unsigned pos) {
	return GetLeaf(pos).stations;
}

/// <summary>
/// ����ĳ����Ҷĳ������ھӵĻ�վ
/// </summary>
/// <param name="node">��Ҷ�ڵ�</param>
/// <param name="pos">0,1,2,3,4,5,6,7�ֱ��ʾ���ϣ��ϣ����ϣ�����������������������</param>
vector<Station> QuadTree::FindNeighborsByOneDirenction(const TreeNode& node, unsigned pos) {
	struct Range {
		Point2 lb;
		Point2 rt;
	};
	Point2 l = node.leftBottomBorder;
	Point2 r = node.rightTopBorder;
	Range ranges[8] = {
		{Point2(l.x - 1, l.y - 1), l },
		{Point2(l.x, l.y - 1), Point2(r.x, l.y)},
		{Point2(r.x, l.y - 1), Point2(r.x + 1, l.y)},
		{Point2(r.x, l.y), Point2(r.x + 1, r.y)},
		{r, Point2(r.x + 1, r.y + 1)},
		{Point2(l.x, r.y), Point2(r.x, r.y + 1)},
		{Point2(l.x - 1, r.y), Point2(l.x, r.y + 1)},
		{Point2(l.x - 1, l.y), Point2(l.x, r.y)}
	};

	vector<Station> ret(Find(ranges[pos].lb, ranges[pos].rt));
	vector<Station> retFinal;
	//ɸ�������ϵ�
	std::copy_if(ret.begin(), ret.end(), std::back_inserter(retFinal), [=](Station station){
		switch (pos)
		{
		case 0:
			return station.coordinate.x < l.x && station.coordinate.y < l.y;
		case 1:
			return station.coordinate.y < l.y;
		case 2:
			return station.coordinate.y < l.y&& station.coordinate.x > r.x;
		case 3:
			return station.coordinate.x > r.x;
		case 4:
			return station.coordinate.y > r.y&& station.coordinate.x > r.x;
		case 5:
			return station.coordinate.y > r.y;
		case 6:
			return station.coordinate.y > r.y&& station.coordinate.x < l.x;
		case 7:
			return station.coordinate.x < l.x;
		default:
			throw "pos Ϊ 0-7";
		}
	});
	return retFinal;
}
