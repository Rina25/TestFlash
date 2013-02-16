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
    QWidget* lWindow = new QWidget(this);
    QGridLayout* lGLayout = new QGridLayout(lWindow);
    lWindow->setLayout(lGLayout);
    setCentralWidget(lWindow);

    QGroupBox* lDeviceGroup = new QGroupBox(QString::fromLocal8Bit("Устройство"),lWindow);
    lGLayout->addWidget(lDeviceGroup,0,0);
    QComboBox* lDeviceComboBox = new QComboBox(lDeviceGroup);
    QVBoxLayout* lDeviceLayout = new QVBoxLayout(lDeviceGroup);
    lDeviceGroup->setLayout(lDeviceLayout);
    lDeviceLayout->addWidget(lDeviceComboBox);
    QLabel* lDeviceLabel = new QLabel(QString::fromLocal8Bit(getDeviceInfoString().c_str()),lDeviceGroup);
    lDeviceLayout->addWidget(lDeviceLabel);

    QGroupBox* lMemoryGroup = new QGroupBox(QString::fromLocal8Bit("Характеристики"),lWindow);
    lGLayout->addWidget(lMemoryGroup,0,1);

    QGroupBox* lTestParamGroup = new QGroupBox(QString::fromLocal8Bit("Параметры тестирования"),lWindow);
    lGLayout->addWidget(lTestParamGroup,1,0,1,2);
}

std::string MainWindow::getDeviceInfoString(std::string iModel, std::string iVendor, std::string iSerial)
{
    return "\nМодель: "+iModel
           +"\nПроизводитель: "+iVendor
           +"\nСерийный номер: "+iSerial;
}
