#pragma once
#include "Utils.h"
#include <vector>
#include <string>
using namespace std;
/// <summary>
/// �洢��վ���ݵ����ݽṹ���Ĳ���
/// </summary>
class QuadTree {
private:
	// Ҷ�ڵ�洢վ���������ֵ�����������ֵʱ�ͽ��з���
	const unsigned maxStationsOfLeaf = 10;

	struct TreeNode {
		// ���±߽������ϱ߽��������ʾ�ýڵ��ʾ�ľ�������
		Point2 leftBottomBorder;
		Point2 rightTopBorder;

		// �Ƿ�ΪҶ�ڵ�
		bool isLeaf = true;
		//���ڵ�
		TreeNode* parent = nullptr;
		//��ǰ�ڵ��ڸ��ڵ��λ��(0,1,2,3�ֱ��ʾ���¡����¡����ϡ�����)
		unsigned pos;
		// ����Ҷ�ڵ�������ĸ��ӽڵ�
		TreeNode* subNodes[4];
		// ��Ҷ�ڵ���洢վ��
		vector<Station> stations;

		// ��ǰ�ڵ����ȣ���0��ʼ
		unsigned depth = 0;

		TreeNode(unsigned _depth):depth(_depth){}
		TreeNode() {}
	};

	// ���ڵ�
	TreeNode rootNode;

	/// <summary>
	/// ���캯���ĸ�������
	/// </summary>
	void ConstructHelper(vector<Station>& stations, TreeNode& node, unsigned depth);

	/// <summary>
	/// ����ĳ�����
	/// </summary>
	vector<Station> TraverseNode(TreeNode node);

	/// <summary>
	/// ���������ĸ�������
	/// </summary>
	void DestructHelper(TreeNode& node);

	vector<Station> FindHelper(const Point2& leftBottom, const Point2& rightTop, TreeNode node) const;

	const TreeNode& GetLeafHelper(const TreeNode& node, unsigned pos) const;
	/// <summary>
	/// �õ�����/����/����/�����ǵ���Ҷ
	/// </summary>
	/// <param name="pos">0,1,2,3�ֱ��ʾ����/����/����/����</param>
	TreeNode GetLeaf(unsigned pos) const;

	/// <summary>
	/// ����ĳ����Ҷĳ������ھӵĻ�վ
	/// </summary>
	/// <param name="node">��Ҷ�ڵ�</param>
	/// <param name="pos">0,1,2,3,4,5,6,7�ֱ��ʾ���ϣ��ϣ����ϣ�����������������������</param>
	vector<Station> FindNeighborsByOneDirenction(const TreeNode& node, unsigned pos);
public: 
	double maxBaseSignalStrength = 1.0;
	Point2 leftBottomBorder;
	Point2 rightTopBorder;
	// �����ļ��������Ĳ���
	QuadTree(string fileName);

	void BuildTree(string fileName1, string fileName2);
	~QuadTree() { 
		DestructHelper(rootNode);
	};
	
	/// <summary>
	/// ��������/����/����/�����ǵ���Ҷ�Ļ�վ
	/// </summary>
	/// <param name="pos">0,1,2,3�ֱ��ʾ����/����/����/����</param>
	vector<Station> FindSationsByOneDirection(unsigned pos);
		
	/// <summary>
	/// ����ĳ��Ҷĳ������ھ�
	/// </summary>
	/// <param name="leafPos">0,1,2,3�ֱ��ʾ����/����/����/����</param>
	/// <param name="pos">0,1,2,3,4,5,6,7�ֱ��ʾ���ϣ��ϣ����ϣ�����������������������</param>
	/// <returns></returns>
	vector<Station> FindLeafNeighborsByOneDirenction(unsigned leafPos, unsigned pos) {
		return FindNeighborsByOneDirenction(GetLeaf(leafPos), pos);
	}

	/// <summary>
	/// ��������ĳ������
	/// </summary>
	/// <param name="index">Ϊ0,1,2,3, �ֱ��ʾ���½ǣ����ϣ������½ǣ����ϣ������Ͻǣ������������Ͻǣ�������</param>
	/// <returns></returns>
	vector<Station> TraverseTreeByOneDirection(int index);

	/// <summary>
	/// ����ĳ����ĳ������
	/// </summary>
	/// <param name="node">Ҫ�����Ļ�׼���</param>
	/// <param name="index">����Ϊ0,1,2,3, �ֱ��ʾ���½ǣ����ϣ������½ǣ����ϣ������Ͻǣ������������Ͻǣ�������</param>
	/// <returns></returns>
	vector<Station> TraverseNodeByOneDirection(TreeNode node, int index);

	QuadTree() {
		rootNode.isLeaf = true;
	};

	//���ݾ�������Ѱ��վ��
	vector<Station> Find(const Point2& leftBottom, const Point2& rightTop) const;
};
/// <summary>
///�ж������Ƿ����
/// </summary>
bool EndInput(const Point2& inputPoint);
