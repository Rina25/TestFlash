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

void CTest::run()
{

    for(int i=0; i<100; i++)
    {
        Sleep(10);
        emit blockIsReady(3, i, i-1, i);
    }
    emit testEnded();
}

HANDLE CTest::openDevice()
{
    return 0;
}
