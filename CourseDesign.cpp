#include "CourseDesign.h"
#include <QPushButton>
#include <QFileDialog>
#include <sstream>
#include <QMessageBox>
/// <summary>
/// ��ʾ��վ���ݵĸ�������
/// </summary>
/// <param name="index">����Ϊ0,1,2,3, �ֱ��ʾ���½ǣ����ϣ������½ǣ����ϣ������Ͻǣ������������Ͻǣ�������</param>
/// <param name="btn">�󶨵İ�ť</param>
void CourseDesign::Task1(unsigned index, const QPushButton* btn) {
    connect(btn, &QPushButton::clicked, [=]() {
       vector<Station> res(tree.TraverseTreeByOneDirection(index));
       QString ret;
       stringstream ss;
       for (Station station : res) {
           ss << "���: " << station.index
               << "\t����: (" << station.coordinate.x
               << ", " << station.coordinate.y << ")"
               << "\t����: " << station.stationType
               << "\t�ź�ǿ��: " << station.signalStrength << endl;
       }
       ret.append(QString::fromLocal8Bit(ss.str().c_str()));
       ui.textBrowser->setText(ret);
    });
}
CourseDesign::CourseDesign(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //ѡ�������ļ�
    connect(ui.file_button, &QPushButton::clicked, [=]() {
        QString filePath = QFileDialog::getOpenFileName(this, "���ļ�", "", "*.txt");
        openFile = true;
        try {
            tree.BuildTree(string(filePath.toLocal8Bit()));
        }
        catch (const char* e) {
            if (openFile) openFile = false;
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(e));
        }
        if (openFile) {
            ui.fileName->setText(QFileInfo(filePath).fileName());
            ui.doubleSpinBox_x->setMinimum(tree.leftBottomBorder.x);
            ui.doubleSpinBox_x->setMaximum(tree.rightTopBorder.x);
            ui.doubleSpinBox_y->setMinimum(tree.leftBottomBorder.y);
            ui.doubleSpinBox_y->setMaximum(tree.rightTopBorder.y);
        }
    });

    //��Ҫ����һ����ʾ��վ����
	Task1(3, ui.main_1_button0);
	Task1(2, ui.main_1_button1);
	Task1(0, ui.main_1_button2);
	Task1(1, ui.main_1_button3);

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
               << "\t�ź�ǿ��: " << station.signalStrength << endl;
       } else {
           ss << "û�п����ӵĻ�վ";
       }
       ui.textBrowser->setText(QString::fromLocal8Bit(ss.str().c_str()));
       });
}

CourseDesign::~CourseDesign()
{}
