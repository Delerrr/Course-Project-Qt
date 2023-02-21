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
    //是否已经打开文件
    bool openMainFile = false;
    //是否打开路径文件
    bool openPathFile = false;
    MovePath path;
    MovePath pseudoPath;
    bool openPseudo = false;
    QuadTree tree;
    Ui::CourseDesignClass ui;

    /// <summary>
    /// 显示基站数据的辅助函数
    /// </summary>
    /// <param name="index">0,1,2,3分别表示西北，东北，西南，东南</param>
    /// <param name="btn">绑定的按钮</param>
    void Task1(unsigned index, const QPushButton* btn); 

    /// <summary>
    /// 升级功能的辅助函数
    /// </summary>
    /// <param name="index">第几段移动路径</param>
    void Task3(unsigned index, const QPushButton* btn);
};
