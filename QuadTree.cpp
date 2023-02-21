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
void QuadTree::BuildTree(string fileName1, string fileName2) {
	DestructHelper(rootNode);
	ifstream ifs(fileName1);
	if (!ifs.is_open()) {
		throw "打开文件" + fileName1 + "失败";
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

	ifs.open(fileName2);
	if (!ifs.is_open()) {
		throw "打开文件" + fileName2 + "失败";
	}
	//读掉开头的文件名
	getline(ifs, buf);
	if (buf.find("JZ") == string::npos) {
		throw "不是站点文件";
	}

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
		node.subNodes[i]->parent = &node;
		node.subNodes[i]->pos = i;
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
 

/// <summary>
/// 得到西南/东南/东北/西北角的树叶
/// </summary>
/// <param name="pos">0,1,2,3分别表示西南/东南/东北/西北</param>
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
/// 查找西南/东南/东北/西北角的树叶的基站
/// </summary>
/// <param name="pos">0,1,2,3分别表示西南/东南/东北/西北</param>
vector<Station> QuadTree::FindSationsByOneDirection(unsigned pos) {
	return GetLeaf(pos).stations;
}

/// <summary>
/// 查找某个树叶某方向的邻居的基站
/// </summary>
/// <param name="node">树叶节点</param>
/// <param name="pos">0,1,2,3,4,5,6,7分别表示西南，南，东南，东，东北，北，西北，西</param>
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
	//筛掉不符合的
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
			throw "pos 为 0-7";
		}
	});
	return retFinal;
}
