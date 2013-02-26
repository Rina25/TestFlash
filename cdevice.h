#ifndef CDEVICE_H
#define CDEVICE_H
#define BUFFER_SIZE 2047

#include <string>
#include <windows.h>
#include <QMessageBox>

class CDevice
{
public:
    CDevice(std::string iDiskName);
    int updateDeviceInfo();
    std::string getDiskName();
    std::string getDeviceName();
    std::string getLabel();
    std::string getModel();
    std::string getVendor();
    std::string getSerial();
    std::string getFileSystem();
    long long getDeviceSize();
    long long getFreeSpace();
    long long getAvailableSpace();
    int getSectorSize();
    long long getCountLBA();
private:
    int getDeviceProperty(HANDLE iHandle);
    int getDeviceSize(HANDLE iHandle);
    std::string bufferToString(char *iBuffer, int iTextOffset, int iMaxLength);
private:
    std::string aDiskName;
    std::string aLabel;
    std::string aModel;
    std::string aVendor;
    std::string aSerial;
    std::string aFileSystem;
    long long aDeviceSize;
    long long aAvailableSpace;
    long long aFreeSpace;
    int aSectorSize;
    long long aCountLBA;
};

#endif // CDEVICE_H
