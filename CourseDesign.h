#pragma once
#include "Core.h"    
#include <QtWidgets/QMainWindow>
#include "ui_CourseDesign.h"
#include <QPushButton>
class CourseDesign : public QMainWindow
{
    Q_OBJECT
public:
    CourseDesign(QWidget *parent = nullptr);
    ~CourseDesign();
    void OpenMainFile();
    void OpenMoveFile();
    void OpenPseudo();
private:
    //�Ƿ��Ѿ����ļ�
    bool openMainFile = false;
    //�Ƿ��·���ļ�
    bool openPathFile = false;
    MovePath path;
    MovePath pseudoPath;
    bool openPseudo = false;
    QuadTree tree;
    Ui::CourseDesignClass ui;

    /// <summary>
    /// ��ʾ��վ���ݵĸ�������
    /// </summary>
    /// <param name="index">0,1,2,3�ֱ��ʾ���������������ϣ�����</param>
    /// <param name="btn">�󶨵İ�ť</param>
    void Task1(unsigned index, const QPushButton* btn); 

    /// <summary>
    /// �������ܵĸ�������
    /// </summary>
    /// <param name="index">�ڼ����ƶ�·��</param>
    void Task3(unsigned index, const QPushButton* btn);
};
