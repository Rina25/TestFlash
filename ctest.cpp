#include "ctest.h"

CTest::CTest(CDevice *iDevice, int iMode, int iMethod, int iBlockSize, long long iStartLBA, long long iEndLBA, QMutex *iPauseMutex, QMutex *iStopMutex)
{
    aDevice = iDevice;
    aMode = iMode;
    aMethod = iMethod;
    aBlockSize = iBlockSize;
    aStartLBA = iStartLBA;
    aEndLBA = iEndLBA;
    aPauseMutex = iPauseMutex;
    aStopMutex = iStopMutex;
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
    if(aMode == 1 && aMethod == 1)
        linearRead(deviceHandle);
    if(aMode == 1 && aMethod == 2)
        butterflyRead(deviceHandle);
    if(aMode == 2)
        linearWrite(deviceHandle);
    if(aMode == 3)
        correctnessWrite(deviceHandle);
    CloseHandle(deviceHandle);
    emit testEnded();
}

HANDLE CTest::openDevice()
{
    std::string devName = "\\\\.\\"+aDevice->getDiskName();
    HANDLE devHandle = CreateFileA(devName.c_str(),
                                  GENERIC_WRITE|GENERIC_READ,
                                  FILE_SHARE_WRITE|FILE_SHARE_READ,
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
    status = DeviceIoControl(devHandle,
                         FSCTL_LOCK_VOLUME,
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
    aFrequency = freq.QuadPart;
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

int CTest::readBlock(HANDLE iDeviceHandle, unsigned char *oData, long long iDataSize, long long& oTime, long long iOffset, DWORD iStartPosition)
{
    //Установить указатель
    LARGE_INTEGER	move;
    move.QuadPart = iOffset;
    long long beginTime;
    long long endTime;
    beginTime = getTime();
    bool status = SetFilePointerEx(iDeviceHandle,
                                   move,
                                   NULL,
                                   iStartPosition);
    endTime = getTime();
    if(!status)
        return 0;
    oTime =endTime - beginTime ;
    //Чтение блока

    DWORD read;
    beginTime = getTime();
    status = ReadFile(iDeviceHandle,
                      oData,
                      iDataSize,
                      (LPDWORD)&read,
                      NULL);
    endTime = getTime();
    oTime = oTime + (endTime - beginTime);
    oTime = ( (aFrequency/oTime)*iDataSize )/ 1024;
    if(!status || read!=iDataSize)
    {
        oTime = -1;
        return -1;
    }

    return 1;
}

int CTest::writeBlock(HANDLE iDeviceHandle, unsigned char *oData, long long iDataSize, long long &oTime, long long iOffset, DWORD iStartPosition)
{
    //Установить указатель
    LARGE_INTEGER	move;
    move.QuadPart = iOffset;
    long long beginTime;
    long long endTime;
    bool status = SetFilePointerEx(iDeviceHandle,
                                   move,
                                   NULL,
                                   iStartPosition);
    if(!status)
        return 0;

    //Запись блока
    DWORD write;
    beginTime = getTime();
    status = WriteFile(iDeviceHandle,
                      oData,
                      iDataSize,
                      (LPDWORD)&write,
                      NULL);
    endTime = getTime();
    oTime = endTime - beginTime;
    oTime = ( (aFrequency/oTime)*iDataSize )/ 1024;
    if(!status || write!=iDataSize)
    {
        oTime = -1;
        return -1;
    }
    return 1;
}

int CTest::writeSectors(HANDLE iDeviceHandle, int iCountSectors, long long iOffset, unsigned char* iWrite1, unsigned char* iWrite2, DWORD iStartPosition)
{
    LARGE_INTEGER	move;
    move.QuadPart = iOffset;
    bool status = SetFilePointerEx(iDeviceHandle,
                                   move,
                                   NULL,
                                   iStartPosition);
    if(!status)
        return 0;
     DWORD write;
    for(int i = 0; i<(iCountSectors-1); i+=2)
    {
        WriteFile(iDeviceHandle,
                  iWrite1,
                  aDevice->getSectorSize(),
                  (LPDWORD)&write,
                  NULL);
        WriteFile(iDeviceHandle,
                  iWrite2,
                  aDevice->getSectorSize(),
                  (LPDWORD)&write,
                  NULL);
    }
    return 1;
}

int CTest::readSectors(HANDLE iDeviceHandle, int iCountSectors, long long iOffset, unsigned char* iWrite1, unsigned char* iWrite2, DWORD iStartPosition)
{
    LARGE_INTEGER	move;
    move.QuadPart = iOffset;
    unsigned char* block1 = new unsigned char[aDevice->getSectorSize()+1];
    unsigned char* block2 = new unsigned char[aDevice->getSectorSize()+1];
    bool status = SetFilePointerEx(iDeviceHandle,
                                   move,
                                   NULL,
                                   iStartPosition);
    if(!status)
        return 0;
     DWORD read;
     status = true;
    for(int i = 0; i<(iCountSectors-1); i+=2)
    {
        ReadFile(iDeviceHandle,
                  block1,
                  aDevice->getSectorSize(),
                  (LPDWORD)&read,
                  NULL);
        if(!std::equal(block1, block1+aDevice->getSectorSize(), iWrite1))
            status = false;
        ReadFile(iDeviceHandle,
                  block2,
                  aDevice->getSectorSize(),
                  (LPDWORD)&read,
                  NULL);
        if(!std::equal(block2, block2+aDevice->getSectorSize(), iWrite2))
            status = false;
        if(!status)
        {
            delete[] block1;
            delete[] block2;
            return -1;
        }
    }
    delete[] block1;
    delete[] block2;
    return 1;
}

long long CTest::getLastBlockSize(long long& oBeginLastBlock)
{
    long long size = ( (aEndLBA-aStartLBA) % aBlockSize) + 1;
    oBeginLastBlock = aEndLBA - (size-1);
    return size;
}

int CTest::linearRead(HANDLE iDeviceHandle)
{
    //Количество байт в блоке
    long long size = aDevice->getSectorSize()*aBlockSize;
    unsigned char* data = new unsigned char[size+1];
    long long speed;
    //Количество блоков (без последнего)
    long long count = (aEndLBA - aStartLBA) / aBlockSize;
    long long i;
    for (i = 0; i < count; i++)
    {
        aPauseMutex->lock();
        readBlock(iDeviceHandle, data, size, speed, 0, FILE_CURRENT);
        aPauseMutex->unlock();
        emit blockIsReady(1, i, speed, 0);
        if(aStopMutex->tryLock())
            aStopMutex->unlock();
        else
        {
            delete[] data;
            return 0;
        }
    }
    long long beginLastBlock;
    long long lastBlockSize = getLastBlockSize(beginLastBlock);
    readBlock(iDeviceHandle, data, lastBlockSize*aDevice->getSectorSize(), speed, 0, FILE_CURRENT);
    emit blockIsReady(1, i, speed, 0);
    delete[] data;
    return 1;
}

int CTest::butterflyRead(HANDLE iDeviceHandle)
{
    //Количество байт в блоке
    long long size = aDevice->getSectorSize()*aBlockSize;
    unsigned char* data = new unsigned char[size+1];
    long long speed;
    //Количество блоков (без последнего)
    long long count = (aEndLBA - aStartLBA) / aBlockSize;
    //Проверяется последний блок
    long long beginLastBlock;
    long long lastBlockSize = getLastBlockSize(beginLastBlock);
    readBlock(iDeviceHandle, data, lastBlockSize*aDevice->getSectorSize(), speed, count*size, FILE_BEGIN);
    emit blockIsReady(1, count, speed, 0);

    long long ir;
    long long il;
    for(il = 0, ir = count-1; il <= ir; il++, ir--)
    {
        aPauseMutex->lock();
        readBlock(iDeviceHandle, data, size, speed, il*size, FILE_BEGIN);
        emit blockIsReady(1, il, speed, 0);
        readBlock(iDeviceHandle, data, size, speed, ir*size, FILE_BEGIN);
        emit blockIsReady(1, ir, speed, 0);
        aPauseMutex->unlock();
        if(aStopMutex->tryLock())
            aStopMutex->unlock();
        else
        {
            delete[] data;
            return 0;
        }
    }
    delete[] data;
    return 1;
}

int CTest::linearWrite(HANDLE iDeviceHandle)
{
    //Количество байт в блоке
    long long size = aDevice->getSectorSize()*aBlockSize;
    unsigned char* data = new unsigned char[size+1];
    long long speed;
    //Количество блоков (без последнего)
    long long count = (aEndLBA - aStartLBA) / aBlockSize;
    long long i;
    for (i = 0; i < count; i++)
    {
        aPauseMutex->lock();
        if(readBlock(iDeviceHandle, data, size, speed, i*size, FILE_BEGIN) == 1)
        {
            writeBlock(iDeviceHandle, data, size, speed, i*size, FILE_BEGIN);
            emit blockIsReady(2, i, 0, speed);
        }
        aPauseMutex->unlock();

        if(aStopMutex->tryLock())
            aStopMutex->unlock();
        else
        {
            delete[] data;
            return 0;
        }
    }
    //Проверяется последний блок
    long long beginLastBlock;
    long long lastBlockSize = getLastBlockSize(beginLastBlock);
    if(readBlock(iDeviceHandle, data, lastBlockSize*aDevice->getSectorSize(), speed, count*size, FILE_BEGIN) == 1);
    {
        writeBlock(iDeviceHandle, data, lastBlockSize*aDevice->getSectorSize(), speed, count*size, FILE_BEGIN);
        emit blockIsReady(2, count, 0, speed);
    }
    delete[] data;
    return 1;
}

int CTest::correctnessWrite(HANDLE iDeviceHandle)
{
    //Количество байт в блоке
    long long size = aDevice->getSectorSize()*aBlockSize;
    unsigned char* data = new unsigned char[size+1];
    long long speedRead;
    long long speedWrite;
    //Количество блоков (без последнего)
    long long count = (aEndLBA - aStartLBA) / aBlockSize;
    long long i;
    unsigned char* blockFF = new unsigned char[aDevice->getSectorSize()+1];
    std::fill(blockFF, blockFF+aDevice->getSectorSize(),0xFF);
    unsigned char* block00 = new unsigned char[aDevice->getSectorSize()+1];
    std::fill(block00, block00+aDevice->getSectorSize(),0x00);
    for (i = 0; i < count; i++)
    {
        aPauseMutex->lock();
        aPauseMutex->unlock();
        if(readBlock(iDeviceHandle, data, size, speedRead, i*size, FILE_BEGIN) == 1)
        {
            writeSectors(iDeviceHandle, aBlockSize, i*size, blockFF, block00);
            if (readSectors(iDeviceHandle, aBlockSize, i*size, blockFF, block00)== -1)
            {
                writeBlock(iDeviceHandle, data, size, speedWrite, i*size, FILE_BEGIN);
                emit blockIsReady(3, i, speedRead, speedWrite, false);
            }
            else
            {
                writeBlock(iDeviceHandle, data, size, speedWrite, i*size, FILE_BEGIN);
                emit blockIsReady(3, i, speedRead, speedWrite, true);
            }
        }
        if(aStopMutex->tryLock())
            aStopMutex->unlock();
        else
        {
            delete[] blockFF;
            delete[] block00;
            delete[] data;
            return 0;
        }
    }
    //Проверяется последний блок
    long long beginLastBlock;
    long long lastBlockSize = getLastBlockSize(beginLastBlock);
    if(readBlock(iDeviceHandle, data, lastBlockSize*aDevice->getSectorSize(), speedRead, count*size, FILE_BEGIN) == 1);
    {
        writeSectors(iDeviceHandle, lastBlockSize, count*size, blockFF, block00);
        if (readSectors(iDeviceHandle, lastBlockSize, count*size, blockFF, block00)== -1)
        {
            writeBlock(iDeviceHandle, data, lastBlockSize*aDevice->getSectorSize(), speedWrite, count*size, FILE_BEGIN);
            emit blockIsReady(3, i, speedRead, speedWrite, false);
        }
        else
        {
            writeBlock(iDeviceHandle, data, lastBlockSize*aDevice->getSectorSize(), speedWrite, count*size, FILE_BEGIN);
            emit blockIsReady(3, i, speedRead, speedWrite, true);
        }
    }
    delete[] blockFF;
    delete[] block00;
    delete[] data;
    return 1;
}
