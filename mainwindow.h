#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <string>


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    std::string getDeviceInfoString(std::string iModel="", std::string iVendor="", std::string iSerial="");

public slots:
    void show();
};

#endif // MAINWINDOW_H
