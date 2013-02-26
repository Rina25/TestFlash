#include "ctest.h"

CTest::CTest(CDevice *iDevice, int iMode, int iMethod, int iBlockSize, long long iStartLBA, long long iEndLBA)
{
    aDevice = iDevice;
    aMode = iMode;
    aMethod = iMethod;
    aBlockSize = iBlockSize;
    aStartLBA = iStartLBA;
    aEndLBA = iEndLBA;
}

CTest::~CTest()
{

}

void CTest::run()
{
    if(!getFrequency())
    {
        emit error("Не поддерживается счетчик высокого разрешения");
        emit testEnded();
        return;
    }
    HANDLE deviceHandle = openDevice();
    if (!deviceHandle)
    {
        emit error("Не удается получить доступ к устройству "+QString::fromStdString(aDevice->getDeviceName()));
        emit testEnded();
        return;
    }
    CloseHandle(deviceHandle);
    emit testEnded();
}

HANDLE CTest::openDevice()
{
    std::string devName = "\\\\.\\"+aDevice->getDiskName();
    HANDLE devHandle = CreateFileA(devName.c_str(),
                                  GENERIC_WRITE|GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL,
                                  OPEN_EXISTING,
                                  0,
                                  NULL);
    if(devHandle == INVALID_HANDLE_VALUE)
        return 0;
    DWORD returnedValue;
    bool status = DeviceIoControl( devHandle,
                     FSCTL_ALLOW_EXTENDED_DASD_IO,
                     NULL,
                     0,
                     NULL,
                     0,
                     &returnedValue,
                     NULL);
    if(!status)
    {
        CloseHandle(devHandle);
        return 0;
    }
    return devHandle;
}

int CTest::getFrequency()
{
    LARGE_INTEGER freq;
    if( !QueryPerformanceFrequency( &freq ) )
        return 0;
    aFrequency = freq.QuadPart/1000;
    if(!aFrequency)
        return 0;
    return 1;
}

long long CTest::getTime()
{
    LARGE_INTEGER counter;
    if(QueryPerformanceCounter(&counter))
        return counter.QuadPart;
    return 0;
}
