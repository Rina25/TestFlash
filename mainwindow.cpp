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

    QGroupBox* lDeviceGroup = new QGroupBox(QString::fromLocal8Bit("����������"),lWindow);
    lGLayout->addWidget(lDeviceGroup,0,0);
    QComboBox* lDeviceComboBox = new QComboBox(lDeviceGroup);
    QObject::connect(lDeviceComboBox, SIGNAL(activated(int)), this, SLOT(updateDiskNameList()));
    QVBoxLayout* lDeviceLayout = new QVBoxLayout(lDeviceGroup);
    lDeviceGroup->setLayout(lDeviceLayout);
    lDeviceLayout->addWidget(lDeviceComboBox);
    aDeviceLabel = new QLabel("",lDeviceGroup);
    lDeviceLayout->addWidget(aDeviceLabel);

    QGroupBox* lMemoryGroup = new QGroupBox(QString::fromLocal8Bit("��������������"),lWindow);
    lGLayout->addWidget(lMemoryGroup,0,1);
    QVBoxLayout* lMemoryLayout = new QVBoxLayout(lMemoryGroup);
    lMemoryGroup->setLayout(lMemoryLayout);
    QLabel* lMemoryLabel = new QLabel(QString::fromLocal8Bit(getMemoryInfoString().c_str()));
    lMemoryLayout->addWidget(lMemoryLabel);

    QGroupBox* lTestParamGroup = new QGroupBox(QString::fromLocal8Bit("��������� ������������"),lWindow);
    lGLayout->addWidget(lTestParamGroup,1,0,1,2);

    CDeviceManager* lDevManager = CDeviceManager::getInstance();
    pNameList nameList = lDevManager->getDeviceNameList();
    for(std::vector<std::string>::iterator it=nameList->begin();it<nameList->end();it++)
        lDeviceComboBox->addItem(QString::fromStdString(*it));

    updateDeviceProperty(0);
}

std::string MainWindow::getDeviceInfoString(std::string iModel, std::string iVendor, std::string iSerial, std::string iFileSystem)
{
    return "\n������:                      "+iModel
           +"\n�������������:           "+iVendor
           +"\n�������� �����:       "+iSerial
            +"\n�������� �������:    "+iFileSystem;
}

std::string MainWindow::getMemoryInfoString(ull iDevSize, ull iTotalSpace, ull iFreeSpace, ull iCountLBA, int iSectorSize)
{
    std::stringstream lMemoryInfoString;
    lMemoryInfoString<<"\n������ ����������: ";
    if(iDevSize)
        lMemoryInfoString<<iDevSize;
    lMemoryInfoString<<"\n��������� ����� ������: ";
    if(iTotalSpace)
        lMemoryInfoString<<iTotalSpace;
    lMemoryInfoString<<"\n����� ��������� ������: ";
    if(iFreeSpace)
        lMemoryInfoString<<iFreeSpace;
    lMemoryInfoString<<"\n\n���������� �������� LBA: ";
    if(iCountLBA)
        lMemoryInfoString<<iCountLBA;
    lMemoryInfoString<<"\n������ �������: ";
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
