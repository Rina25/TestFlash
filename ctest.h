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
public slots:
    void run();
signals:
    void testEnded();
    void blockIsReady(int iMode, int iBlockNumber, int iReadSpeed, int iWriteSpeed);
private:
    HANDLE openDevice();
private:
    CDevice* aDevice;
    int aMode;
    int aMethod;
    int aBlockSize;
    long long aStartLBA;
    long long aEndLBA;
};

#endif // CTEST_H
