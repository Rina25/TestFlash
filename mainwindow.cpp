#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint)
{
    aWindowLocked = false;
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::show()
{
    QWidget::show();
    aWindow = new QWidget(this);
    QGridLayout* lGLayout = new QGridLayout(aWindow);
    aWindow->setLayout(lGLayout);
    setCentralWidget(aWindow);

    //Информация об устройстве
    QGroupBox* lDeviceGroup = new QGroupBox(QString::fromLocal8Bit("Устройство"),aWindow);
    lGLayout->addWidget(lDeviceGroup,0,0);
    aDeviceComboBox = new QComboBox(lDeviceGroup);
    QObject::connect(aDeviceComboBox, SIGNAL(activated(int)), this, SLOT(updateDeviceProperty(int)));
    QVBoxLayout* lDeviceLayout = new QVBoxLayout(lDeviceGroup);
    lDeviceGroup->setLayout(lDeviceLayout);
    lDeviceLayout->addWidget(aDeviceComboBox);
    aDeviceLabel = new QLabel("",lDeviceGroup);
    lDeviceLayout->addWidget(aDeviceLabel);

    //Информация о памяти
    QGroupBox* lMemoryGroup = new QGroupBox(QString::fromLocal8Bit("Характеристики"),aWindow);
    lGLayout->addWidget(lMemoryGroup,0,1);
    QVBoxLayout* lMemoryLayout = new QVBoxLayout(lMemoryGroup);
    lMemoryGroup->setLayout(lMemoryLayout);
    aMemoryLabel = new QLabel(QString::fromLocal8Bit(getMemoryInfoString().c_str()));
    lMemoryLayout->addWidget(aMemoryLabel);

    //Параметры тестирования
    QGroupBox* lTestParamGroup = new QGroupBox(QString::fromLocal8Bit("Параметры тестирования"),aWindow);
    lGLayout->addWidget(lTestParamGroup,1,0,1,2);
    QGridLayout* lTestParamLayout = new QGridLayout(lTestParamGroup);
    lTestParamGroup->setLayout(lTestParamLayout);
    aReadCheck = new QCheckBox(QString::fromLocal8Bit("Чтение"),lTestParamGroup);
    aReadCheck->setChecked(true);
    lTestParamLayout->addWidget(aReadCheck, 0, 0);
    aWriteCheck = new QCheckBox(QString::fromLocal8Bit("Запись"),lTestParamGroup);
    lTestParamLayout->addWidget(aWriteCheck, 1, 0);
    aLinearRadio = new QRadioButton(QString::fromLocal8Bit("Линейная адресация"),lTestParamGroup);
    aLinearRadio->setChecked(true);
    lTestParamLayout->addWidget(aLinearRadio, 0, 1);
    aButterflyRadio = new QRadioButton(QString::fromLocal8Bit("Адресация методом 'Butterfly'"),lTestParamGroup);
    lTestParamLayout->addWidget(aButterflyRadio, 1, 1);
    aRandomRadio = new QRadioButton(QString::fromLocal8Bit("Адресация методом 'Random'"),lTestParamGroup);
    lTestParamLayout->addWidget(aRandomRadio, 2, 1);
    QLabel* blockSizeLabel = new QLabel(QString::fromLocal8Bit("Размер блока:"),lTestParamGroup);
    lTestParamLayout->addWidget(blockSizeLabel, 0, 3);
    aBlockSizeEdit = new QSpinBox(lTestParamGroup);
    aBlockSizeEdit->setMaximum(4096);
    aBlockSizeEdit->setMinimum(1);
    aBlockSizeEdit->setValue(256);
    lTestParamLayout->addWidget(aBlockSizeEdit,0,4);
    QLabel* startLabel = new QLabel(QString::fromLocal8Bit("Начальный адрес LBA:"),lTestParamGroup);
    lTestParamLayout->addWidget(startLabel, 1, 3);
    aStartEdit = new QSpinBox(lTestParamGroup);
    aStartEdit->setMinimum(0);
    lTestParamLayout->addWidget(aStartEdit,1,4);
    QLabel* endLabel = new QLabel(QString::fromLocal8Bit("Конечный адрес LBA:"),lTestParamGroup);
    lTestParamLayout->addWidget(endLabel, 2, 3);
    aEndEdit = new QSpinBox(lTestParamGroup);
    aEndEdit->setMinimum(0);
    lTestParamLayout->addWidget(aEndEdit,2,4);
    lTestParamLayout->setColumnMinimumWidth(2,20);
    QPushButton* testButton = new QPushButton(QString::fromLocal8Bit("Начать тестирование"),lTestParamGroup);
    lTestParamLayout->addWidget(testButton, 3, 1, 1, 3);
    QObject::connect(testButton, SIGNAL(clicked()), this, SLOT(testButtonClick()));

    CDeviceManager* lDevManager = CDeviceManager::getInstance();
    QObject::connect(lDevManager,SIGNAL(testFinished()),this,SLOT(unlockWindow()));
    pNameList nameList = lDevManager->getDeviceNameList();
    for(std::vector<std::string>::iterator it=nameList->begin();it<nameList->end();it++)
        aDeviceComboBox->addItem(QString::fromStdString(*it));

    updateDeviceProperty(0);
    aWindow->adjustSize();

}

void MainWindow::unlockWindow()
{
    aWindow->setEnabled(true);
    aWindowLocked = false;
}

void MainWindow::closeEvent(QCloseEvent *iCloseEvent)
{
    if(aWindowLocked)
        iCloseEvent->ignore();
    else
        iCloseEvent->accept();
}

std::string MainWindow::getDeviceInfoString(std::string iModel, std::string iVendor, std::string iSerial, std::string iFileSystem)
{
    return "\nМодель:                      "+iModel
           +"\nПроизводитель:           "+iVendor
           +"\nСерийный номер:       "+iSerial
            +"\nФайловая система:    "+iFileSystem;
}

std::string MainWindow::getMemoryInfoString(long long iDevSize, long long iAvailableSpace, long long iFreeSpace, long long iCountLBA, int iSectorSize)
{
    std::stringstream lMemoryInfoString;
    lMemoryInfoString<<"\nРазмер устройства:                ";
    if(iDevSize)
        lMemoryInfoString<<iDevSize<<" "<<convertBytes(iDevSize);
    lMemoryInfoString<<"\nДоступный объем памяти:    ";
    if(iAvailableSpace)
        lMemoryInfoString<<iAvailableSpace<<" "<<convertBytes(iAvailableSpace);
    lMemoryInfoString<<"\nОбъем свободной памяти:     ";
    if(iFreeSpace)
        lMemoryInfoString<<iFreeSpace<<" "<<convertBytes(iFreeSpace);
    lMemoryInfoString<<"\n\nКоличество секторов LBA:    ";
    if(iCountLBA)
        lMemoryInfoString<<iCountLBA;
    lMemoryInfoString<<"\nРазмер сектора:                      ";
    if(iSectorSize)
        lMemoryInfoString<<iSectorSize;
    return lMemoryInfoString.str();
}

std::string MainWindow::convertBytes(long long iBytes)
{
    std::stringstream convertStream;
    if( (iBytes/1024) < 1)
        return "";
    if((iBytes/1024/1024) < 1)
        convertStream<<"("<<((long)(iBytes/1024.0*100))/100.0<<" Кб)";
    else
        if((iBytes/1024/1024/1024) < 1)
             convertStream<<"("<<((long)(iBytes/1024.0/1024*100))/100.0<<" Мб)";
        else
            if((iBytes/1024/1024/1024/1024) < 1)
                convertStream<<"("<<((long)(iBytes/1024.0/1024/1024*100))/100.0<<" Гб)";
    return convertStream.str();

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
       if(!devManager->updateDeviceInfo(iIndex))
           throw "device not found";
        aDeviceLabel->setText(QString::fromLocal8Bit(getDeviceInfoString(devManager->getDeviceModel(iIndex),
                                                                         devManager->getDeviceVendor(iIndex),
                                                                         devManager->getDeviceSerial(iIndex),
                                                                         devManager->getDeviceFileSystem(iIndex)).c_str()));
        aMemoryLabel->setText(QString::fromLocal8Bit(getMemoryInfoString(devManager->getDeviceSize(iIndex),
                                                                         devManager->getDeviceFreeSpace(iIndex),
                                                                         devManager->getAvailableSpace(iIndex),
                                                                         devManager->getDeviceCountLBA(iIndex),
                                                                         devManager->getDeviceSectorSize(iIndex)).c_str()));
        aStartEdit->setMaximum(devManager->getDeviceCountLBA(iIndex)-1);
        aEndEdit->setMaximum(devManager->getDeviceCountLBA(iIndex));
        aEndEdit->setValue(devManager->getDeviceCountLBA(iIndex));
    }
    catch(...)
    {
        aDeviceLabel->setText(QString::fromLocal8Bit(getDeviceInfoString().c_str()));
        aMemoryLabel->setText(QString::fromLocal8Bit(getMemoryInfoString().c_str()));
    }
}

void MainWindow::testButtonClick()
{
    aWindow->setEnabled(false);
    aWindowLocked = true;
    int iMode = 0;
    if (aReadCheck->isChecked())
        iMode+=1;
    if (aWriteCheck->isChecked())
        iMode+=2;
    int iMethod = 0;
    if(aLinearRadio->isChecked())
        iMethod =1;
    if(aButterflyRadio->isChecked())
        iMethod =2;
    if(aRandomRadio->isChecked())
        iMethod =3;
    (CDeviceManager::getInstance())->runTest(aDeviceComboBox->currentIndex(),
                            iMode,
                            iMethod,
                            aBlockSizeEdit->value(),
                            aStartEdit->value(),
                            aEndEdit->value());

}
