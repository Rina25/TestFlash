#include "cdevicemanager.h"

CDeviceManager* CDeviceManager::aInstance=0;

CDeviceManager* CDeviceManager::getInstance()
{
    if(!aInstance)
        aInstance=new CDeviceManager();
    return aInstance;
}

CDeviceManager::~CDeviceManager()
{
    aInstance=0;
    for(std::vector<CDevice*>::iterator it=aDeviceList->begin();it<aDeviceList->end();it++)
        delete *it;
    delete aDeviceList;
    aDeviceList=0;
}

pNameList CDeviceManager::getDeviceNameList()
{
    getDeviceList();
    pNameList nameList = new std::vector<std::string>();
    for(std::vector<CDevice*>::iterator it=aDeviceList->begin();it<aDeviceList->end();it++)
        nameList->push_back((*it)->getDiskName());
    return nameList;
}

int CDeviceManager::updateDeviceInfo(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return 0;
    if(!aDeviceList->at(iIndex)->updateDeviceInfo())
        return 0;
    return 1;
}

std::string CDeviceManager::getDeviceModel(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return "";
    return aDeviceList->at(iIndex)->getModel();
}

std::string CDeviceManager::getDeviceVendor(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return "";
    return aDeviceList->at(iIndex)->getVendor();
}

std::string CDeviceManager::getDeviceSerial(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return "";
    return aDeviceList->at(iIndex)->getSerial();
}

std::string CDeviceManager::getDeviceFileSystem(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return "";
    return aDeviceList->at(iIndex)->getFileSystem();
}

long long CDeviceManager::getDeviceSize(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return 0;
    return aDeviceList->at(iIndex)->getDeviceSize();
}

long long CDeviceManager::getDeviceFreeSpace(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return 0;
    return aDeviceList->at(iIndex)->getFreeSpace();
}

long long CDeviceManager::getAvailableSpace(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return 0;
    return aDeviceList->at(iIndex)->getAvailableSpace();
}

int CDeviceManager::getDeviceSectorSize(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return 0;
    return aDeviceList->at(iIndex)->getSectorSize();
}

long long CDeviceManager::getDeviceCountLBA(int iIndex)
{
    if(iIndex<0 || iIndex>=aDeviceList->size())
        return 0;
    return aDeviceList->at(iIndex)->getCountLBA();
}

CDeviceManager::CDeviceManager()
{
    aDeviceList=0;
}

pDevList CDeviceManager::getDeviceList()
{
    if(aDeviceList)
    {
        for(std::vector<CDevice*>::iterator it=aDeviceList->begin();it<aDeviceList->end();it++)
            delete *it;
        delete aDeviceList;
        aDeviceList=0;
    }

    aDeviceList = new std::vector<CDevice*>();
    //Маска доступных устройств
    unsigned int lDisks = GetLogicalDrives();
    char lDivName[] = "?:";
    for(int i = 31; i >= 0; i--)
    {
        //Устройство существует?
        if(lDisks & (1 << i))
        {
            lDivName[0] = static_cast<char>('A') + i;
            //Tип устройства
            DWORD lType = GetDriveTypeA(lDivName);
            //Устройство съемное?
            if(lType == DRIVE_REMOVABLE)
            {
                char lBuf[MAX_PATH];
                    if(QueryDosDeviceA(lDivName, lBuf, MAX_PATH))
                        if(std::string(lBuf).find("\\Floppy") == std::string::npos) //Если в имени нет подстроки "\\Floppy",
                        {
                            aDeviceList->push_back(new CDevice(&lDivName[0]));
                            aDeviceList->back()->updateDeviceInfo();
                        }
             }
          }
    }
    return aDeviceList;
}

void CDeviceManager::testWindowClosed()
{
    delete aTestWindow;
    emit testFinished();
}

void CDeviceManager::testEnded()
{

}

int CDeviceManager::runTest(int iIndex, int iMode, int iMethod, int iBlockSize, long long iStartLBA, long long iEndLBA)
{
    //Проверка параметров
    bool status = true;
    getDeviceList();
    if(iIndex<0 || iIndex>=aDeviceList->size())
        status = false;
    else
        if(!updateDeviceInfo(iIndex))
            status = false;
    if(iStartLBA <0 || iStartLBA >iEndLBA || iStartLBA > getDeviceCountLBA(iIndex))
        status = false;
    if(iEndLBA <0 || iEndLBA > getDeviceCountLBA(iIndex))
        status = false;
    if(iBlockSize<1 || iBlockSize>4096)
        status = false;
    if(iMode < 1 || iMode > 3 || iMethod < 1 || iMethod > 3)
        status = false;

    if(!status)
    {
        //Ошибка, выход
        QMessageBox* testBox = new QMessageBox(QMessageBox::Critical,"Error",QString::fromLocal8Bit("Невозможно получить доступ к устройству"));
        testBox->exec();
        emit testFinished();
        return 0;
    }
    //Окно тестирования
    aTestWindow = new CTestWindow(iMode,(iEndLBA-iStartLBA+1)/iBlockSize+1);
    QObject::connect(aTestWindow, SIGNAL(windowClosed()), this, SLOT(testWindowClosed()));
    aTestWindow->show();

    //Поток для тестирования
    aTestThread = new QThread();
    aTest = new CTest(aDeviceList->at(iIndex), iMode, iMethod, iBlockSize, iStartLBA, iEndLBA);
    aTest->moveToThread(aTestThread);
    connect(aTestThread, SIGNAL(started()), aTest, SLOT(run()));
    connect(aTest, SIGNAL(testEnded()), aTestThread, SLOT(quit()));
    connect(aTest, SIGNAL(testEnded()), aTest, SLOT(deleteLater()));
    connect(aTestThread, SIGNAL(finished()), aTestThread, SLOT(deleteLater()));
    connect(aTest, SIGNAL(blockIsReady(int,int,int,int)), aTestWindow, SLOT(addBlock(int,int,int,int)));


    aTestThread->start();
}


