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

private:
    //�Ƿ��Ѿ����ļ�
    bool openFile = false;
    QuadTree tree;
    Ui::CourseDesignClass ui;

    /// <summary>
    /// ��ʾ��վ���ݵĸ�������
    /// </summary>
    /// <param name="index">0,1,2,3�ֱ��ʾ���������������ϣ�����</param>
    /// <param name="btn">�󶨵İ�ť</param>
    void Task1(unsigned index, const QPushButton* btn); 
};
