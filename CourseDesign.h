#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CourseDesign.h"

class CourseDesign : public QMainWindow
{
    Q_OBJECT

public:
    CourseDesign(QWidget *parent = nullptr);
    ~CourseDesign();

private:
    Ui::CourseDesignClass ui;
};
