#include "CourseDesign.h"
#include <QPushButton>
#include <QFileDialog>
#include <sstream>
#include <QMessageBox>
/// <summary>
/// �������ܵĸ�������
/// </summary>
/// <param name="index">�ڼ����ƶ�·��</param>
void CourseDesign::Task3(unsigned index, const QPushButton* btn) {
    connect(btn, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�򿪻�վ�����ļ�"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ��·���ļ�"));
            return;
        }
        if (openPseudo == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ��α��վ·���ļ�"));
            return;
        }
        ui.textBrowser->setText(QString::fromLocal8Bit("������"));
        vector<ConnectInfo> res(PseudoConnection(path, pseudoPath, index - 1));
        stringstream ss;
        if (res.size() == 0) {
            ss << "δ����α��վ\n";
        } else {
            double sec1 = res[0].connectTime.minute - (int)res[0].connectTime.minute;
            double sec2 = res[1].connectTime.minute - (int)res[1].connectTime.minute;
            ss << "��վ���: " << res[0].connectStation.index
                << "\n��ʼʱ��: " << res[0].connectTime.hour << " : " << (int)res[0].connectTime.minute << " : " << sec1
                << "\n����ʱ��: " << res[1].connectTime.hour << " : " << (int)res[1].connectTime.minute << " : " << sec2
                << endl;
        }
        ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        });
}
/// <summary>
/// ��ʾ��վ���ݵĸ�������
/// </summary>
/// <param name="index">����Ϊ0,1,2,3, �ֱ��ʾ���½ǣ����ϣ������½ǣ����ϣ������Ͻǣ������������Ͻǣ�������</param>
/// <param name="btn">�󶨵İ�ť</param>
void CourseDesign::Task1(unsigned index, const QPushButton* btn) {
    connect(btn, &QPushButton::clicked, [=]() {
			ui.textBrowser->setText(QString::fromLocal8Bit("������"));
       vector<Station> res(tree.FindSationsByOneDirection(index));
       //QString ret;
       stringstream ss;
       for (Station station : res) {
           ss << "���: " << station.index
               << "\t����: (" << station.coordinate.x
               << ", " << station.coordinate.y << ")"
               << "\t����: " << station.stationType
               << "\t�ź�ǿ��: " << station.signalStrength << "\n\n";
       }
       if (res.size() == 0) {
           ss << "��";
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
    //ѡ�������ļ�
    OpenMainFile();
    //ѡ��·���ļ�
    OpenMoveFile();
    OpenPseudo();
    
    //��Ҫ����һ����ʾ��վ����
	Task1(3, ui.main_1_button0);
	Task1(2, ui.main_1_button1);
	Task1(0, ui.main_1_button2);
	Task1(1, ui.main_1_button3);

    //��Ҫ����1.2��������Ҷ���ھ�
    connect(ui.main_12_btn, &QPushButton::clicked, [=]() {
       vector<Station> res(tree.FindLeafNeighborsByOneDirenction(ui.comboBox->currentIndex(), ui.comboBox_2->currentIndex()));
       stringstream ss;
       for (Station station : res) {
           ss << "���: " << station.index
               << "\t����: (" << station.coordinate.x
               << ", " << station.coordinate.y << ")"
               << "\t����: " << station.stationType
               << "\t�ź�ǿ��: " << station.signalStrength << "\n\n";
       }
       if (res.size() == 0) {
           ss << "��";
       }
       ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
    });

    //��Ҫ���ܶ��������ź���ǿ�Ļ�վ
    connect(ui.main_2_button, &QPushButton::clicked, [=]() {
       Point2 pos(ui.doubleSpinBox_x->value(), ui.doubleSpinBox_y->value());
       QString ret;
       stringstream ss;
       Station station = FindBestStation(tree, pos);
       if (station.index != -1) {
           ss << "��վ���: " << station.index
               << "\t��վ����: (" << station.coordinate.x
               << ", " << station.coordinate.y << ")"
               << "\t����: " << station.stationType
               << "\t��׼�ź�ǿ��: " << station.signalStrength << endl;
       } else {
           ss << "û�п����ӵĻ�վ";
       }
       ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
       });
    //��Ҫ�������������վ���л�����
    connect(ui.main_3_button, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�򿪻�վ�����ļ�"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ��·���ļ�"));
            return;
        }
		ui.textBrowser->setText(QString::fromLocal8Bit("������"));
        if (openPathFile) {
            vector<ConnectInfo> res(CalcConnection(path, tree, 10));
			stringstream ss;
			for (ConnectInfo info : res) {
				//double sec = (info.connectTime.minute - int(info.connectTime.minute)) * 60;
				ss << "ʱ��: " << info.connectTime.hour << " : " << int(info.connectTime.minute) //<< " : " << int(sec)
					<< "\t ����: (" << info.position.x << " , " << info.position.y << ")";
				//�Ͽ�����
				if (info.connectStation.index == -1) {
					ss << "\t�Ͽ�����\n\n";
				} else {
					ss << "\t��վ���: " << info.connectStation.index 
                        << "\t" << info.connectStation.stationType
                        << "\n\n";
				}
			}
       //ret.append(QString::fromLocal8Bit(ss.str().c_str()));
       //ui.textBrowser->setText(ret);
			ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        }
    });

    //��չ����һ
    connect(ui.main_2_btn, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�򿪻�վ�����ļ�"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ��·���ļ�"));
            return;
        }
		ui.textBrowser->setText(QString::fromLocal8Bit("������"));
        ConnectFirst res = CalcFirstConnection(path, tree);
			stringstream ss;
				double sec1 = (res.T1.minute - int(res.T1.minute)) * 60;
				double sec2 = (res.T2.minute - int(res.T2.minute)) * 60;
            ss << "��վ���: " << res.station.index
                << "\n��ʼʱ��: " << res.T1.hour << " : " << (int)res.T1.minute << " : " << sec1
                << "\n����ʱ��: " << res.T2.hour << " : " << (int)res.T2.minute << " : " << sec2
                << endl;
			ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        //ConnectFirst res = calc
    });
    //��չ���ܶ�
    connect(ui.main_2_btn2, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�򿪻�վ�����ļ�"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ��·���ļ�"));
            return;
        }
		ui.textBrowser->setText(QString::fromLocal8Bit("������"));
        OverlapInfo info = CalcOverlap(path, 2, tree);
			stringstream ss;
            if (info.stations.size() > 0) {
				double sec = info.totalTime.minute - (int)info.totalTime.minute;
				ss << "��վ���: " << info.stations[0].index << ", " << info.stations[1].index
					<< "\n����ʱ��: ";
				if (info.totalTime.hour != 0)
				{
					ss << info.totalTime.hour << " Сʱ ";
				}
				ss << (int)info.totalTime.minute << " ���� "
					<< sec << " ��" << endl;
            } else {
                ss << "û���ص���\n";
            }
			ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        //ConnectFirst res = calc
    });
    connect(ui.main_2_btn3, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�򿪻�վ�����ļ�"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ��·���ļ�"));
            return;
        }
		ui.textBrowser->setText(QString::fromLocal8Bit("������"));
        OverlapInfo info = CalcOverlap(path, 5, tree);
			stringstream ss;
            if (info.stations.size() > 0) {
				double sec = info.totalTime.minute - (int)info.totalTime.minute;
				ss << "��վ���: " << info.stations[0].index << ", " << info.stations[1].index
					<< "\n����ʱ��: ";
				if (info.totalTime.hour != 0)
				{
					ss << info.totalTime.hour << " Сʱ ";
				}
				ss << (int)info.totalTime.minute << " ���� "
					<< sec << " ��" << endl;
            } else {
                ss << "û���ص���\n";
            }
			ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
        //ConnectFirst res = calc
    });
    //��������
    Task3(9, ui.main_3_btn);
    Task3(12, ui.main_3_btn2);
    connect(ui.main_3_btn3, &QPushButton::clicked, [=]() {
        if (openMainFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�򿪻�վ�����ļ�"));
            return;
        }
        if (openPathFile == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ��·���ļ�"));
            return;
        }
        if (openPseudo == false) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ��α��վ·���ļ�"));
            return;
        }
        ui.textBrowser->setText(QString::fromLocal8Bit("������"));
        vector<ConnectInfo> res(PseudoConnection(path, pseudoPath, ui.spinBox_3->value() - 1));
        stringstream ss;
        if (res.size() == 0) {
            ss << "δ����α��վ\n";
        } else {
            double sec1 = res[0].connectTime.minute - (int)res[0].connectTime.minute;
            double sec2 = res[1].connectTime.minute - (int)res[1].connectTime.minute;
            ss << "��վ���: " << res[0].connectStation.index
                << "\n��ʼʱ��: " << res[0].connectTime.hour << " : " << (int)res[0].connectTime.minute << " : " << sec1
                << "\n����ʱ��: " << res[1].connectTime.hour << " : " << (int)res[1].connectTime.minute << " : " << sec2
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
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(e));
            ui.fileName->setText(QString::fromLocal8Bit("��"));
        }
        catch (string e) {
            if (openMainFile) openMainFile = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(e.c_str()));
            ui.fileName->setText(QString::fromLocal8Bit("��"));
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
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(e));
            ui.main_3_label->setText(QString::fromLocal8Bit("��"));
        }
        catch (string e) {
            if (openPathFile) openPathFile = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(e.c_str()));
            ui.main_3_label->setText(QString::fromLocal8Bit("��"));
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
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(e));
            ui.pseudo_label->setText(QString::fromLocal8Bit("��"));
        }
        catch (string e) {
            if (openPseudo) openPseudo = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(e.c_str()));
            ui.pseudo_label->setText(QString::fromLocal8Bit("��"));
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

