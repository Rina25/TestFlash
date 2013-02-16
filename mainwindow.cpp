#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::show()
{
    QWidget::show();
    QWidget *lWindow=new QWidget(this);
    QGridLayout* lGLayout=new QGridLayout(this);
    lWindow->setLayout(lGLayout);
    setCentralWidget(lWindow);

    QGroupBox* lDeviceGroup= new QGroupBox(QString::fromLocal8Bit("Устройство"),lWindow);
    lGLayout->addWidget(lDeviceGroup,0,0);

    QGroupBox* lMemoryGroup= new QGroupBox(QString::fromLocal8Bit("Характеристики"),lWindow);
    lGLayout->addWidget(lMemoryGroup,0,1);

    QGroupBox* lTestParamGroup= new QGroupBox(QString::fromLocal8Bit("Параметры тестирования"),lWindow);
    lGLayout->addWidget(lTestParamGroup,1,0,1,2);
}
