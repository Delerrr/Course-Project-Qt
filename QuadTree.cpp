#include "QuadTree.h"
#include <iostream>
#include <fstream>
#include <algorithm>

/// <summary>
///判断输入是否结束
/// </summary>
bool EndInput(const Point2& inputPoint) {
	const double EPSILON = 0.0001;
	return fabs(inputPoint.x + 1) < EPSILON && 
			fabs(inputPoint.y + 1) < EPSILON;
}

/// <summary>
//比较横坐标
/// </summary>
bool XComp(const Station& arg1,const Station& arg2) {
	return arg1.coordinate.x < arg2.coordinate.x;
}

/// <summary>
//比较纵坐标
/// </summary>
bool YComp(const Station& arg1,const Station& arg2) {
	return arg1.coordinate.y < arg2.coordinate.y;
}

/// <summary>
/// 比较信号强度
/// </summary>
bool SignalComp(const Station& arg1,const Station& arg2) {
	return arg1.signalStrength < arg2.signalStrength;
}
QuadTree::QuadTree(string fileName) {
	ifstream ifs(fileName);
	if (!ifs.is_open()) {
		throw "打开文件" + fileName + "失败";
	}

	string buf;
	//读掉开头的文件名
	getline(ifs, buf);
	if (buf.find("JZ") == string::npos) {
		throw "不是站点文件";
	}
	vector<Station> stations;
	Station inputStation;
	//处理逗号
	char comma;
	
	ifs >> inputStation.coordinate.x
		>> comma
		>> inputStation.coordinate.y;

	while (!EndInput(inputStation.coordinate))
	{
		//处理逗号
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
/// 根据文件名构建树
/// </summary>
/// <param name="fileName"></param>
void QuadTree::BuildTree(string fileName) {
	DestructHelper(rootNode);
	ifstream ifs(fileName);
	if (!ifs.is_open()) {
		throw "打开文件" + fileName + "失败";
	}
	string buf;
	//读掉开头的文件名
	getline(ifs, buf);
	if (buf.find("JZ") == string::npos) {
		throw "不是站点文件";
	}

	rootNode.isLeaf = false;
	vector<Station> stations;
	Station inputStation;
	//处理逗号
	char comma;
	
	ifs >> inputStation.coordinate.x
		>> comma
		>> inputStation.coordinate.y;

	while (!EndInput(inputStation.coordinate))
	{
		//处理逗号
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
		ConstructHelper(substations[i], *(node.subNodes[i]), depth + 1);
	}		
}

/// <summary>
/// 遍历某个结点
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
/// 遍历树的某个方向
/// </summary>
/// <param name="index">为0,1,2,3, 分别表示左下角（西南）、右下角（东南）、右上角（东北）、左上角（西北）</param>
/// <returns></returns>
vector<Station> QuadTree::TraverseTreeByOneDirection(int index) {
	return TraverseNodeByOneDirection(rootNode, index);
}

/// <summary>
/// 遍历某结点的某个方向
/// </summary>
/// <param name="node">要遍历的基准结点</param>
/// <param name="index">方向，为0,1,2,3, 分别表示左下角（西南）、右下角（东南）、右上角（东北）、左上角（西北）</param>
/// <returns></returns>
vector<Station> QuadTree::TraverseNodeByOneDirection(TreeNode node, int index) {
	if (index > 3 || index < 0) throw "index 不合理";
	return TraverseNode(*(node.subNodes[index]));
}

/// <summary>
/// 析构函数的辅助函数
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
/// 查找函数的辅助函数
/// </summary>
vector<Station> QuadTree::FindHelper(const Point2& leftBottom, const Point2& rightTop, TreeNode node) const {
	vector<Station> ret;
	if (node.isLeaf) {
		ret.assign(node.stations.begin(), node.stations.end());
	} else {
		for (TreeNode* subNode : node.subNodes) {
			//如果子节点区域和被查找区域有重叠，则查找子节点，否则不查找
			if (IsOverlapped(subNode->leftBottomBorder, subNode->rightTopBorder, leftBottom, rightTop)) {
				vector<Station> temp(FindHelper(leftBottom, rightTop, *subNode));
				ret.insert(ret.end(), temp.begin(), temp.end());
			}
		}
	}

	return ret;
}

/// <summary>
/// 给定矩形区域，查找该矩形区域内的站点
/// </summary>
vector<Station> QuadTree::Find(const Point2& leftBottom, const Point2& rightTop) const {
 	return FindHelper(leftBottom, rightTop, rootNode);
}
 



