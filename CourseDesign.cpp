#include "CourseDesign.h"
#include <QPushButton>
#include <QFileDialog>
#include <sstream>
#include <QMessageBox>
/// <summary>
/// 升级功能的辅助函数
/// </summary>
/// <param name="index">第几段移动路径</param>
void CourseDesign::Task3(unsigned index, const QPushButton* btn) {
    connect(btn, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开基站数据文件"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开路径文件"));
            return;
        }
        if (openPseudo == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开伪基站路径文件"));
            return;
        }
        ui.textBrowser->setText(QString::fromLocal8Bit("计算中"));
        vector<ConnectInfo> res(PseudoConnection(path, pseudoPath, index - 1));
        stringstream ss;
        if (res.size() == 0) {
            ss << "未连接伪基站\n";
        } else {
            double sec1 = res[0].connectTime.minute - (int)res[0].connectTime.minute;
            double sec2 = res[1].connectTime.minute - (int)res[1].connectTime.minute;
            ss << "基站编号: " << res[0].connectStation.index
                << "\n开始时间: " << res[0].connectTime.hour << " : " << (int)res[0].connectTime.minute << " : " << sec1
                << "\n结束时间: " << res[1].connectTime.hour << " : " << (int)res[1].connectTime.minute << " : " << sec2
                << endl;
        }
        ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        });
}
/// <summary>
/// 显示基站数据的辅助函数
/// </summary>
/// <param name="index">方向，为0,1,2,3, 分别表示左下角（西南）、右下角（东南）、右上角（东北）、左上角（西北）</param>
/// <param name="btn">绑定的按钮</param>
void CourseDesign::Task1(unsigned index, const QPushButton* btn) {
    connect(btn, &QPushButton::clicked, [=]() {
			ui.textBrowser->setText(QString::fromLocal8Bit("计算中"));
       vector<Station> res(tree.FindSationsByOneDirection(index));
       //QString ret;
       stringstream ss;
       for (Station station : res) {
           ss << "编号: " << station.index
               << "\t坐标: (" << station.coordinate.x
               << ", " << station.coordinate.y << ")"
               << "\t类型: " << station.stationType
               << "\t信号强度: " << station.signalStrength << "\n\n";
       }
       if (res.size() == 0) {
           ss << "无";
       }
       //ret.append(QString::fromLocal8Bit(ss.str().c_str()));
       //ui.textBrowser->setText(ret);
       ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
    });
}
CourseDesign::CourseDesign(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.tabWidget->setEnabled(false);
    //选择数据文件
    OpenMainFile();
    //选择路径文件
    OpenMoveFile();
    OpenPseudo();
    
    //主要功能一：显示基站数据
	Task1(3, ui.main_1_button0);
	Task1(2, ui.main_1_button1);
	Task1(0, ui.main_1_button2);
	Task1(1, ui.main_1_button3);

    //主要功能1.2：计算树叶的邻居
    connect(ui.main_12_btn, &QPushButton::clicked, [=]() {
       vector<Station> res(tree.FindLeafNeighborsByOneDirenction(ui.comboBox->currentIndex(), ui.comboBox_2->currentIndex()));
       stringstream ss;
       for (Station station : res) {
           ss << "编号: " << station.index
               << "\t坐标: (" << station.coordinate.x
               << ", " << station.coordinate.y << ")"
               << "\t类型: " << station.stationType
               << "\t信号强度: " << station.signalStrength << "\n\n";
       }
       if (res.size() == 0) {
           ss << "无";
       }
       ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
    });

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
               << "\t基准信号强度: " << station.signalStrength << endl;
       } else {
           ss << "没有可连接的基站";
       }
       ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
       });
    //主要功能三：计算基站的切换序列
    connect(ui.main_3_button, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开基站数据文件"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开路径文件"));
            return;
        }
		ui.textBrowser->setText(QString::fromLocal8Bit("计算中"));
        if (openPathFile) {
            vector<ConnectInfo> res(CalcConnection(path, tree, 10));
			stringstream ss;
			for (ConnectInfo info : res) {
				//double sec = (info.connectTime.minute - int(info.connectTime.minute)) * 60;
				ss << "时间: " << info.connectTime.hour << " : " << int(info.connectTime.minute) //<< " : " << int(sec)
					<< "\t 坐标: (" << info.position.x << " , " << info.position.y << ")";
				//断开连接
				if (info.connectStation.index == -1) {
					ss << "\t断开连接\n\n";
				} else {
					ss << "\t基站编号: " << info.connectStation.index 
                        << "\t" << info.connectStation.stationType
                        << "\n\n";
				}
			}
       //ret.append(QString::fromLocal8Bit(ss.str().c_str()));
       //ui.textBrowser->setText(ret);
			ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        }
    });

    //扩展功能一
    connect(ui.main_2_btn, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开基站数据文件"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开路径文件"));
            return;
        }
		ui.textBrowser->setText(QString::fromLocal8Bit("计算中"));
        ConnectFirst res = CalcFirstConnection(path, tree);
			stringstream ss;
				double sec1 = (res.T1.minute - int(res.T1.minute)) * 60;
				double sec2 = (res.T2.minute - int(res.T2.minute)) * 60;
            ss << "基站编号: " << res.station.index
                << "\n开始时间: " << res.T1.hour << " : " << (int)res.T1.minute << " : " << sec1
                << "\n结束时间: " << res.T2.hour << " : " << (int)res.T2.minute << " : " << sec2
                << endl;
			ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        //ConnectFirst res = calc
    });
    //扩展功能二
    connect(ui.main_2_btn2, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开基站数据文件"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开路径文件"));
            return;
        }
		ui.textBrowser->setText(QString::fromLocal8Bit("计算中"));
        OverlapInfo info = CalcOverlap(path, 2, tree);
			stringstream ss;
            if (info.stations.size() > 0) {
				double sec = info.totalTime.minute - (int)info.totalTime.minute;
				ss << "基站编号: " << info.stations[0].index << ", " << info.stations[1].index
					<< "\n持续时间: ";
				if (info.totalTime.hour != 0)
				{
					ss << info.totalTime.hour << " 小时 ";
				}
				ss << (int)info.totalTime.minute << " 分钟 "
					<< sec << " 秒" << endl;
            } else {
                ss << "没有重叠区\n";
            }
			ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        //ConnectFirst res = calc
    });
    connect(ui.main_2_btn3, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开基站数据文件"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开路径文件"));
            return;
        }
		ui.textBrowser->setText(QString::fromLocal8Bit("计算中"));
        OverlapInfo info = CalcOverlap(path, 5, tree);
			stringstream ss;
            if (info.stations.size() > 0) {
				double sec = info.totalTime.minute - (int)info.totalTime.minute;
				ss << "基站编号: " << info.stations[0].index << ", " << info.stations[1].index
					<< "\n持续时间: ";
				if (info.totalTime.hour != 0)
				{
					ss << info.totalTime.hour << " 小时 ";
				}
				ss << (int)info.totalTime.minute << " 分钟 "
					<< sec << " 秒" << endl;
            } else {
                ss << "没有重叠区\n";
            }
			ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        //ConnectFirst res = calc
    });
    //升级功能
    Task3(9, ui.main_3_btn);
    Task3(12, ui.main_3_btn2);
    connect(ui.main_3_btn3, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开基站数据文件"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开路径文件"));
            return;
        }
        if (openPseudo == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未打开伪基站路径文件"));
            return;
        }
        ui.textBrowser->setText(QString::fromLocal8Bit("计算中"));
        vector<ConnectInfo> res(PseudoConnection(path, pseudoPath, ui.spinBox_3->value() - 1));
        stringstream ss;
        if (res.size() == 0) {
            ss << "未连接伪基站\n";
        } else {
            double sec1 = res[0].connectTime.minute - (int)res[0].connectTime.minute;
            double sec2 = res[1].connectTime.minute - (int)res[1].connectTime.minute;
            ss << "基站编号: " << res[0].connectStation.index
                << "\n开始时间: " << res[0].connectTime.hour << " : " << (int)res[0].connectTime.minute << " : " << sec1
                << "\n结束时间: " << res[1].connectTime.hour << " : " << (int)res[1].connectTime.minute << " : " << sec2
                << endl;
        }
        ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
    });
}

void CourseDesign::OpenMainFile() {
    QString fileName1 = "jz001.txt";
    QString fileName2 = "jz002.txt";
    openMainFile = true;
        try {
            tree.BuildTree(fileName1.toStdString(), fileName2.toStdString());
        }
        catch (const char* e) {
            if (openMainFile) openMainFile = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(e));
            ui.fileName->setText(QString::fromLocal8Bit("无"));
        }
        catch (string e) {
            if (openMainFile) openMainFile = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(e.c_str()));
            ui.fileName->setText(QString::fromLocal8Bit("无"));
        }
        if (openMainFile) {
            ui.tabWidget->setEnabled(true);
            ui.fileName->setText(fileName1 + " " + fileName2);
            ui.doubleSpinBox_x->setMinimum(tree.leftBottomBorder.x);
            ui.doubleSpinBox_x->setMaximum(tree.rightTopBorder.x);
            ui.doubleSpinBox_y->setMinimum(tree.leftBottomBorder.y);
            ui.doubleSpinBox_y->setMaximum(tree.rightTopBorder.y);
        }
}

void CourseDesign::OpenMoveFile() {
    QString fileName = "yd001.txt";
        openPathFile = true;
        try {
            path.Init(fileName.toStdString());
        }
        catch (const char* e) {
            if (openPathFile) openPathFile = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(e));
            ui.main_3_label->setText(QString::fromLocal8Bit("无"));
        }
        catch (string e) {
            if (openPathFile) openPathFile = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(e.c_str()));
            ui.main_3_label->setText(QString::fromLocal8Bit("无"));
        }
        if (openPathFile) {
            ui.main_3_label->setText(fileName);
        }
}

void CourseDesign::OpenPseudo() {
    QString fileName = "wz001.txt";
        openPseudo = true;
        try {
            pseudoPath.Init(fileName.toStdString());
        }
        catch (const char* e) {
            if (openPseudo) openPseudo = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(e));
            ui.pseudo_label->setText(QString::fromLocal8Bit("无"));
        }
        catch (string e) {
            if (openPseudo) openPseudo = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(e.c_str()));
            ui.pseudo_label->setText(QString::fromLocal8Bit("无"));
        }
        if ((openPseudo)) {
            ui.pseudo_label->setText(fileName);
            ui.spinBox_3->setMinimum(1);
            ui.spinBox_3->setMaximum(path.paths.size());
            ui.doubleSpinBox_x->setSingleStep(1);
        }
}

CourseDesign::~CourseDesign()
{}

