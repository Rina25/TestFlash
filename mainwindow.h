#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <string>
#include <sstream>
#include <QMessageBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QPushButton>
#include "cdevicemanager.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    std::string getDeviceInfoString(std::string iModel="", std::string iVendor="", std::string iSerial="", std::string iFileSystem="");
    std::string getMemoryInfoString(long long iDevSize=0, long long iAvailableSpace=0, long long iFreeSpace=0, long long iCountLBA=0, int iSectorSize=0);
    std::string convertBytes(long long iBytes);
public slots:
    void show();
private slots:
    void updateDiskNameList();
    void updateDeviceProperty(int iIndex);
private:
    QLabel* aDeviceLabel;
    QLabel* aMemoryLabel;
    QCheckBox* aReadCheck;
    QCheckBox* aWriteCheck;
    QRadioButton* aLinearRadio;
    QRadioButton* aButterflyRadio;
    QRadioButton* aRandomRadio;
    QSpinBox* aBlockSizeEdit;
    QSpinBox* aStartEdit;
    QSpinBox* aEndEdit;
};

#endif // MAINWINDOW_H
