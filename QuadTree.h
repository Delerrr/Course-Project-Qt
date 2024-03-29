#pragma once
#include "Utils.h"
#include <vector>
#include <string>
using namespace std;
/// <summary>
/// 存储基站数据的数据结构：四叉树
/// </summary>
class QuadTree {
private:
	// 叶节点存储站点数的最大值，当大于这个值时就进行分裂
	const unsigned maxStationsOfLeaf = 10;

	struct TreeNode {
		// 左下边界点和右上边界点用来表示该节点表示的矩形区域
		Point2 leftBottomBorder;
		Point2 rightTopBorder;

		// 是否为叶节点
		bool isLeaf = true;
		//父节点
		TreeNode* parent = nullptr;
		//当前节点在父节点的位置(0,1,2,3分别表示左下、右下、右上、左上)
		unsigned pos;
		// 不是叶节点则必有四个子节点
		TreeNode* subNodes[4];
		// 是叶节点则存储站点
		vector<Station> stations;

		// 当前节点的深度，从0开始
		unsigned depth = 0;

		TreeNode(unsigned _depth):depth(_depth){}
		TreeNode() {}
	};

	// 根节点
	TreeNode rootNode;

	/// <summary>
	/// 构造函数的辅助函数
	/// </summary>
	void ConstructHelper(vector<Station>& stations, TreeNode& node, unsigned depth);

	/// <summary>
	/// 遍历某个结点
	/// </summary>
	vector<Station> TraverseNode(TreeNode node);

	/// <summary>
	/// 析构函数的辅助函数
	/// </summary>
	void DestructHelper(TreeNode& node);

	vector<Station> FindHelper(const Point2& leftBottom, const Point2& rightTop, TreeNode node) const;

	const TreeNode& GetLeafHelper(const TreeNode& node, unsigned pos) const;
	/// <summary>
	/// 得到西南/东南/东北/西北角的树叶
	/// </summary>
	/// <param name="pos">0,1,2,3分别表示西南/东南/东北/西北</param>
	TreeNode GetLeaf(unsigned pos) const;

	/// <summary>
	/// 查找某个树叶某方向的邻居的基站
	/// </summary>
	/// <param name="node">树叶节点</param>
	/// <param name="pos">0,1,2,3,4,5,6,7分别表示西南，南，东南，东，东北，北，西北，西</param>
	vector<Station> FindNeighborsByOneDirenction(const TreeNode& node, unsigned pos);
public: 
	double maxBaseSignalStrength = 1.0;
	Point2 leftBottomBorder;
	Point2 rightTopBorder;
	// 根据文件来构建四叉树
	QuadTree(string fileName);

	void BuildTree(string fileName1, string fileName2);
	~QuadTree() { 
		DestructHelper(rootNode);
	};
	
	/// <summary>
	/// 查找西南/东南/东北/西北角的树叶的基站
	/// </summary>
	/// <param name="pos">0,1,2,3分别表示西南/东南/东北/西北</param>
	vector<Station> FindSationsByOneDirection(unsigned pos);
		
	/// <summary>
	/// 查找某树叶某方向的邻居
	/// </summary>
	/// <param name="leafPos">0,1,2,3分别表示西南/东南/东北/西北</param>
	/// <param name="pos">0,1,2,3,4,5,6,7分别表示西南，南，东南，东，东北，北，西北，西</param>
	/// <returns></returns>
	vector<Station> FindLeafNeighborsByOneDirenction(unsigned leafPos, unsigned pos) {
		return FindNeighborsByOneDirenction(GetLeaf(leafPos), pos);
	}

	/// <summary>
	/// 遍历树的某个方向
	/// </summary>
	/// <param name="index">为0,1,2,3, 分别表示左下角（西南）、右下角（东南）、右上角（东北）、左上角（西北）</param>
	/// <returns></returns>
	vector<Station> TraverseTreeByOneDirection(int index);

	/// <summary>
	/// 遍历某结点的某个方向
	/// </summary>
	/// <param name="node">要遍历的基准结点</param>
	/// <param name="index">方向，为0,1,2,3, 分别表示左下角（西南）、右下角（东南）、右上角（东北）、左上角（西北）</param>
	/// <returns></returns>
	vector<Station> TraverseNodeByOneDirection(TreeNode node, int index);

	QuadTree() {
		rootNode.isLeaf = true;
	};

	//根据矩形区域寻找站点
	vector<Station> Find(const Point2& leftBottom, const Point2& rightTop) const;
};
/// <summary>
///判断输入是否结束
/// </summary>
bool EndInput(const Point2& inputPoint);
