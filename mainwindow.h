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
#include "cdevicemanager.h"

typedef unsigned long long ull;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    std::string getDeviceInfoString(std::string iModel="", std::string iVendor="", std::string iSerial="");
    std::string getMemoryInfoString(ull iDevSize=0, ull iTotalSpace=0, ull iFreeSpace=0, ull iCountLBA=0, int iSectorSize=0);

public slots:
    void show();
};

#endif // MAINWINDOW_H
