#include "cdevice.h"

CDevice::CDevice(std::string iDiskName)
{
    aDeviceSize = 0;
    aAvailableSpace = 0;
    aFreeSpace = 0;
    aDiskName=iDiskName;
    char bufLabel[BUFFER_SIZE];
    char bufFS[BUFFER_SIZE];
    GetVolumeInformationA((aDiskName+"\\").c_str(),
                          &bufLabel[0],
                          BUFFER_SIZE,
                          NULL,
                          0,
                          0,
                          &bufFS[0],
                          BUFFER_SIZE);
    aLabel = bufLabel;
    aFileSystem = bufFS;
}

std::string CDevice::getDiskName()
{
    return aDiskName;
}

std::string CDevice::getDeviceName()
{
    return aDiskName +"   ("+aLabel+")";
}

int CDevice::updateDeviceInfo()
{
    std::string devName = "\\\\.\\"+aDiskName;
    HANDLE devHandle = CreateFileA(devName.c_str(),
                                  GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL,
                                  OPEN_EXISTING,
                                  0,
                                  NULL);
    if(devHandle == INVALID_HANDLE_VALUE)
        return 0;
    if(!getDeviceProperty(devHandle))
    {
         CloseHandle(devHandle);
        return 0;
    }
    if(!getDeviceSize(devHandle))
    {
        CloseHandle(devHandle);
        return 0;
    }
     CloseHandle(devHandle);
    return 1;
}

std::string CDevice::getModel()
{
    return aModel;
}

std::string CDevice::getVendor()
{
    return aVendor;
}

std::string CDevice::getSerial()
{
    return aSerial;
}

std::string CDevice::getFileSystem()
{
    return aFileSystem;
}

long long CDevice::getDeviceSize()
{
    return aDeviceSize;
}

long long CDevice::getFreeSpace()
{
    return aFreeSpace;
}

long long CDevice::getAvailableSpace()
{
    return aAvailableSpace;
}

int CDevice::getSectorSize()
{
    return aSectorSize;
}

long long CDevice::getCountLBA()
{
    return aCountLBA;
}

int CDevice::getDeviceProperty(HANDLE iHandle)
{
    STORAGE_PROPERTY_QUERY query;
    query.PropertyId = StorageDeviceProperty;
    query.QueryType  = PropertyStandardQuery;
    char buffer[BUFFER_SIZE+1];
    unsigned long returnedLength;
    bool status = DeviceIoControl(iHandle,
                                  IOCTL_STORAGE_QUERY_PROPERTY,
                                  &query,
                                  sizeof( STORAGE_PROPERTY_QUERY ),
                                  &buffer,
                                  BUFFER_SIZE,
                                  &returnedLength,
                                  NULL);
    if(!status)
       return 0;
    PSTORAGE_DEVICE_DESCRIPTOR	deviceDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR) buffer;
    aModel =bufferToString(&buffer[0],deviceDescriptor->ProductIdOffset,returnedLength);
    aVendor =bufferToString(&buffer[0],deviceDescriptor->VendorIdOffset,returnedLength);
    aSerial =bufferToString(&buffer[0],deviceDescriptor->SerialNumberOffset,returnedLength);
    return 1;
}

int CDevice::getDeviceSize(HANDLE iHandle)
{
    //Размер уст-ва
    GET_LENGTH_INFORMATION lengthInfo;
    DWORD returnedLength;
    bool status = DeviceIoControl(iHandle,
                                  IOCTL_DISK_GET_LENGTH_INFO,
                                  NULL,
                                  0,
                                  &lengthInfo,
                                  sizeof(lengthInfo),
                                  &returnedLength,
                                  NULL);
    if(!status)
        return 0;
    aDeviceSize = (long long) lengthInfo.Length.QuadPart;
    //Доступная и свободная память
    ULARGE_INTEGER 	availableSpace;
    ULARGE_INTEGER 	totalSpace;
    ULARGE_INTEGER 	freeSpace;
    status = GetDiskFreeSpaceExA((aDiskName+"\\").c_str(),
                                (PULARGE_INTEGER)&availableSpace,
                                (PULARGE_INTEGER)&totalSpace,
                                (PULARGE_INTEGER)&freeSpace);
    if(!status)
        return 0;
    aAvailableSpace = (long long) availableSpace.QuadPart;
    aFreeSpace = (long long) totalSpace.QuadPart;
    //Размер сектора
    DISK_GEOMETRY diskGeometry;
    returnedLength = 0;
    status = DeviceIoControl(iHandle,
                             IOCTL_DISK_GET_DRIVE_GEOMETRY,
                             NULL,
                             0,
                             &diskGeometry,
                             sizeof(DISK_GEOMETRY),
                             &returnedLength,
                             NULL);
    if(!status)
        return 0;
    aSectorSize = (long long) diskGeometry.BytesPerSector;
    //Количество секторов LBA
    aCountLBA = aDeviceSize / aSectorSize;
    return 1;
}

std::string CDevice::bufferToString(char *iBuffer, int iTextOffset, int iMaxLength)
{
    char* ret = new char[iMaxLength-iTextOffset+1];
    std::string textString;
    if(!iTextOffset)
        return "";
    strncpy(ret,iBuffer+iTextOffset,iMaxLength-iTextOffset);
    ret[iMaxLength-iTextOffset]='\0';
    return ret;

}
