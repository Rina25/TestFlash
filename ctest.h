#ifndef CTEST_H
#define CTEST_H
#include <QObject>
#include <QMutex>
#include <Windows.h>
#include "cdevice.h"

class CTest : public QObject
{
    Q_OBJECT
public:
    CTest(CDevice* iDevice, int iMode, int iMethod, int iBlockSize, long long iStartLBA, long long iEndLBA, QMutex* iPauseMutex, QMutex* iStopMutex);
    ~CTest();
public slots:
    void run();
signals:
    void testEnded();
    void blockIsReady(int iMode, int iBlockNumber, int iReadSpeed, int iWriteSpeed);
    void error(QString iMessage);
private:
    HANDLE openDevice();
    //ќпределение частоты счетчика(в мс)
    int getFrequency();
    //Tекущее значение счетчика высокого разрешени€
    inline long long getTime();
    //¬озвращает: -1 - ошибка чтени€ блока; 0 - невозможно получить доступ к устройству; 1 - без ошибок
    int readBlock(HANDLE iDeviceHandle, unsigned char *oData, long long iDataSize, long long &oTime, long long iOffset, DWORD iStartPosition = FILE_BEGIN);
    //¬озвращает размер (в секторах) последнего провер€емого блока; oBeginLastBlock - номер первого сектора последнего блока
    long long getLastBlockSize(long long &oBeginLastBlock);
    int linearRead(HANDLE iDeviceHandle);
    int butterflyRead(HANDLE iDeviceHandle);
private:
    CDevice* aDevice;
    int aMode;
    int aMethod;
    int aBlockSize;
    long long aStartLBA;
    long long aEndLBA;
    long long aFrequency;
    QMutex* aPauseMutex;
    QMutex* aStopMutex;
};

#endif // CTEST_H
