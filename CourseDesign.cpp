#include "CourseDesign.h"
#include <QPushButton>
#include <QFileDialog>
#include <sstream>
#include <QMessageBox>
/// <summary>
/// 显示基站数据的辅助函数
/// </summary>
/// <param name="index">方向，为0,1,2,3, 分别表示左下角（西南）、右下角（东南）、右上角（东北）、左上角（西北）</param>
/// <param name="btn">绑定的按钮</param>
void CourseDesign::Task1(unsigned index, const QPushButton* btn) {
    connect(btn, &QPushButton::clicked, [=]() {
       vector<Station> res(tree.TraverseTreeByOneDirection(index));
       QString ret;
       stringstream ss;
       for (Station station : res) {
           ss << "编号: " << station.index
               << "\t坐标: (" << station.coordinate.x
               << ", " << station.coordinate.y << ")"
               << "\t类型: " << station.stationType
               << "\t信号强度: " << station.signalStrength << endl;
       }
       ret.append(QString::fromLocal8Bit(ss.str().c_str()));
       ui.textBrowser->setText(ret);
    });
}
CourseDesign::CourseDesign(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //选择数据文件
    connect(ui.file_button, &QPushButton::clicked, [=]() {
        QString filePath = QFileDialog::getOpenFileName(this, "打开文件", "", "*.txt");
        openFile = true;
        try {
            tree.BuildTree(string(filePath.toLocal8Bit()));
        }
        catch (const char* e) {
            if (openFile) openFile = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(e));
        }
        if (openFile) {
            ui.fileName->setText(QFileInfo(filePath).fileName());
            ui.doubleSpinBox_x->setMinimum(tree.leftBottomBorder.x);
            ui.doubleSpinBox_x->setMaximum(tree.rightTopBorder.x);
            ui.doubleSpinBox_y->setMinimum(tree.leftBottomBorder.y);
            ui.doubleSpinBox_y->setMaximum(tree.rightTopBorder.y);
        }
    });

    //主要功能一：显示基站数据
	Task1(3, ui.main_1_button0);
	Task1(2, ui.main_1_button1);
	Task1(0, ui.main_1_button2);
	Task1(1, ui.main_1_button3);

    //主要功能二：计算信号最强的基站
    connect(ui.main_2_button, &QPushButton::clicked, [=]() {
        Point2 pos(ui.doubleSpinBox_x->value(), ui.doubleSpinBox_y->value());

       QString ret;
       stringstream ss;
       Station station = FindBestStation(tree, pos);
       if (station.index != -1) {
           ss << "基站编号: " << station.index
               << "\t基站坐标: (" << station.coordinate.x
               << ", " << station.coordinate.y << ")"
               << "\t类型: " << station.stationType
               << "\t信号强度: " << station.signalStrength << endl;
       } else {
           ss << "没有可连接的基站";
       }
       ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
       });
}

CourseDesign::~CourseDesign()
{}
