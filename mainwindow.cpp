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
    QObject::connect(lDeviceComboBox, SIGNAL(activated(int)), this, SLOT(updateDiskNameList()));
    QVBoxLayout* lDeviceLayout = new QVBoxLayout(lDeviceGroup);
    lDeviceGroup->setLayout(lDeviceLayout);
    lDeviceLayout->addWidget(lDeviceComboBox);
    aDeviceLabel = new QLabel("",lDeviceGroup);
    lDeviceLayout->addWidget(aDeviceLabel);

    QGroupBox* lMemoryGroup = new QGroupBox(QString::fromLocal8Bit("Характеристики"),lWindow);
    lGLayout->addWidget(lMemoryGroup,0,1);
    QVBoxLayout* lMemoryLayout = new QVBoxLayout(lMemoryGroup);
    lMemoryGroup->setLayout(lMemoryLayout);
    QLabel* lMemoryLabel = new QLabel(QString::fromLocal8Bit(getMemoryInfoString().c_str()));
    lMemoryLayout->addWidget(lMemoryLabel);

    QGroupBox* lTestParamGroup = new QGroupBox(QString::fromLocal8Bit("Параметры тестирования"),lWindow);
    lGLayout->addWidget(lTestParamGroup,1,0,1,2);

    CDeviceManager* lDevManager = CDeviceManager::getInstance();
    pNameList nameList = lDevManager->getDeviceNameList();
    for(std::vector<std::string>::iterator it=nameList->begin();it<nameList->end();it++)
        lDeviceComboBox->addItem(QString::fromStdString(*it));

    updateDeviceProperty(0);
}

std::string MainWindow::getDeviceInfoString(std::string iModel, std::string iVendor, std::string iSerial, std::string iFileSystem)
{
    return "\nМодель:                      "+iModel
           +"\nПроизводитель:           "+iVendor
           +"\nСерийный номер:       "+iSerial
            +"\nФайловая система:    "+iFileSystem;
}

std::string MainWindow::getMemoryInfoString(ull iDevSize, ull iTotalSpace, ull iFreeSpace, ull iCountLBA, int iSectorSize)
{
    std::stringstream lMemoryInfoString;
    lMemoryInfoString<<"\nРазмер устройства: ";
    if(iDevSize)
        lMemoryInfoString<<iDevSize;
    lMemoryInfoString<<"\nДоступный объем памяти: ";
    if(iTotalSpace)
        lMemoryInfoString<<iTotalSpace;
    lMemoryInfoString<<"\nОбъем свободной памяти: ";
    if(iFreeSpace)
        lMemoryInfoString<<iFreeSpace;
    lMemoryInfoString<<"\n\nКоличество секторов LBA: ";
    if(iCountLBA)
        lMemoryInfoString<<iCountLBA;
    lMemoryInfoString<<"\nРазмер сектора: ";
    if(iSectorSize)
        lMemoryInfoString<<iSectorSize;
    return lMemoryInfoString.str();
}

void MainWindow::updateDiskNameList()
{
    QMessageBox* testBox = new QMessageBox();
    testBox->exec();
}

void MainWindow::updateDeviceProperty(int iIndex)
{
    CDeviceManager* devManager = CDeviceManager::getInstance();
    try
    {
        aDeviceLabel->setText(QString::fromLocal8Bit(getDeviceInfoString(devManager->getDeviceModel(iIndex),
                                                                         devManager->getDeviceVendor(iIndex),
                                                                         devManager->getDeviceSerial(iIndex),
                                                                         devManager->getDeviceFileSystem(iIndex)).c_str()));
    }
    catch(...)
    {
        aDeviceLabel->setText(QString::fromLocal8Bit(getDeviceInfoString().c_str()));
    }

}
