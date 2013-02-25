#ifndef CDEVICEMANAGER_H
#define CDEVICEMANAGER_H

#include <vector>
#include <QThread>
#include "cdevice.h"
#include "ctestwindow.h"
#include "ctest.h"

typedef std::vector<CDevice*>* pDevList;
typedef std::vector<std::string>* pNameList;

class CDeviceManager : public QObject
{
    Q_OBJECT

public:
    static CDeviceManager* getInstance();
    ~CDeviceManager();
    pNameList getDeviceNameList();
    int updateDeviceInfo(int iIndex);

    std::string getDeviceModel(int iIndex);
    std::string getDeviceVendor(int iIndex);
    std::string getDeviceSerial(int iIndex);
    std::string getDeviceFileSystem(int iIndex);
    long long getDeviceSize(int iIndex);
    long long getDeviceFreeSpace(int iIndex);
    long long getAvailableSpace(int iIndex);
    int getDeviceSectorSize(int iIndex);
    long long getDeviceCountLBA(int iIndex);

    //iMode: 1 - чтение, 2 - запись, 3 - чтение и запись
    //iMethod: 1 - linear, 2 - butterfly, 3 - random
    int runTest(int iIndex, int iMode, int iMethod, int iBlockSize, long long iStartLBA, long long iEndLBA);
signals:
    void testFinished();
private:
    CDeviceManager();
    pDevList getDeviceList();
private slots:
    void testWindowClosed();
    void testEnded();
private:
    static CDeviceManager* aInstance;
    pDevList aDeviceList;
    CTestWindow* aTestWindow;
    QThread* aTestThread;
    CTest* aTest;
};

#endif // CDEVICEMANAGER_H
