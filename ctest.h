#ifndef CTEST_H
#define CTEST_H
#include <QObject>
#include <Windows.h>
#include "cdevice.h"

class CTest : public QObject
{
    Q_OBJECT
public:
    CTest(CDevice* iDevice, int iMode, int iMethod, int iBlockSize, long long iStartLBA, long long iEndLBA);
    ~CTest();
public slots:
    void run();
signals:
    void testEnded();
    void blockIsReady(int iMode, int iBlockNumber, int iReadSpeed, int iWriteSpeed);
    void error(QString iMessage);
private:
    HANDLE openDevice();
    //Определение частоты счетчика(в мс)
    int getFrequency();
    //Tекущее значение счетчика высокого разрешения
    inline long long getTime();
private:
    CDevice* aDevice;
    int aMode;
    int aMethod;
    int aBlockSize;
    long long aStartLBA;
    long long aEndLBA;
    long long aFrequency;

};

#endif // CTEST_H
