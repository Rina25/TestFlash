#ifndef CTEST_H
#define CTEST_H
#include <QObject>
#include <QMutex>
#include <Windows.h>
#include <algorithm>
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
    void blockIsReady(int iMode, int iBlockNumber, int iReadSpeed, int iWriteSpeed, bool iCorrectnessWrite = true);
    void error(QString iMessage);
private:
    HANDLE openDevice();
    //����������� ������� ��������(� ��)
    int getFrequency();
    //T������ �������� �������� �������� ����������
    inline long long getTime();
    //����������: -1 - ������ ������ �����; 0 - ���������� �������� ������ � ����������; 1 - ��� ������
    int readBlock(HANDLE iDeviceHandle, unsigned char *oData, long long iDataSize, long long &oTime, long long iOffset, DWORD iStartPosition = FILE_BEGIN);
    int writeBlock(HANDLE iDeviceHandle, unsigned char *oData, long long iDataSize, long long &oTime, long long iOffset, DWORD iStartPosition = FILE_BEGIN);
    int writeSectors(HANDLE iDeviceHandle, int iCountSectors, long long iOffset, unsigned char *iWrite1, unsigned char *iWrite2, DWORD iStartPosition = FILE_BEGIN);
    int readSectors(HANDLE iDeviceHandle, int iCountSectors, long long iOffset, unsigned char *iWrite1, unsigned char *iWrite2, DWORD iStartPosition = FILE_BEGIN);
    //���������� ������ (� ��������) ���������� ������������ �����; oBeginLastBlock - ����� ������� ������� ���������� �����
    long long getLastBlockSize(long long &oBeginLastBlock);
    int linearRead(HANDLE iDeviceHandle);
    int butterflyRead(HANDLE iDeviceHandle);
    int linearWrite(HANDLE iDeviceHandle);
    int correctnessWrite(HANDLE iDeviceHandle);
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
