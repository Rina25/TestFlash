#include "ctestwindow.h"

CTestWindow::CTestWindow(int iMode, int iCountBlocks, QWidget *iParent):QWidget(iParent,Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint)
{
    aMode = iMode;
    aCountBlocks = iCountBlocks;
    aReadTable = 0;
    aWriteTable = 0;
    aColorMap[-1] = QColor("#000000");
    aColorMap[5] = QColor("#CCFFCC");
    aColorMap[10] = QColor("#99CCCC");
    aColorMap[20] = QColor("#FFFF99");
    aColorMap[50] = QColor("#FFCC99");
    aColorMap[200] = QColor("#6699CC");
    aColorMap[500] = QColor("#996699");
    aColorMap[1000] = QColor("#CC0000");
}

void CTestWindow::show()
{

    QGridLayout* mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    QPushButton* pauseButton = new QPushButton(QString::fromLocal8Bit("Пауза"),this);
    pauseButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    mainLayout->addWidget(pauseButton, 0, 0);
    QPushButton* runButton = new QPushButton(QString::fromLocal8Bit("Возобновить"),this);
    runButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    runButton->setEnabled(false);
    mainLayout->addWidget(runButton, 0, 1);
    QPushButton* stopButton = new QPushButton(QString::fromLocal8Bit("Прервать"),this);
    stopButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    mainLayout->addWidget(stopButton, 0, 2);
    mainLayout->setRowMinimumHeight(0,50);


    aProgressBar = new QProgressBar(this);
    mainLayout->addWidget(aProgressBar,1, 0, 1, 3);

    //Вкладки
    QTabWidget* tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget,2,0,1,3);
    if(aMode == 1 ||aMode == 3)
    {
        QWidget* readMap = new QWidget(this);
        tabWidget->addTab(readMap,QString::fromLocal8Bit("Чтение"));
        QGridLayout* readLayout = new QGridLayout(readMap);
        readMap->setLayout(readLayout);
        aReadTable = createMap(this);
        readLayout->addWidget(aReadTable,0, 0, 8, 1);
    }
    if(aMode == 2 ||aMode == 3)
    {
        QWidget* writeMap = new QWidget(this);
        tabWidget->addTab(writeMap,QString::fromLocal8Bit("Запись"));
        QGridLayout* writeLayout = new QGridLayout(writeMap);
        writeMap->setLayout(writeLayout);
        aWriteTable = createMap(this);
        writeLayout->addWidget(aWriteTable,0, 0, 8, 1);
    }

    adjustSize();
    QWidget::show();
}

void CTestWindow::addBlock(int iMode, int iBlockNumber, int iReadSpeed, int iWriteSpeed)
{
    int row;
    int column;
    std::map<int, QColor>::iterator it;
    if((iMode == 1 || iMode == 3) && (aReadTable))
    {
        row = iBlockNumber / aReadTable->columnCount();
        column = iBlockNumber - row*aReadTable->columnCount();
        QTableWidgetItem* tableItem = new QTableWidgetItem("");
        it = aColorMap.begin();
        if(iReadSpeed == it->first)
            tableItem->setBackground(it->second);
        else
            for(it++; it!=aColorMap.end();it++)
            {
               tableItem->setBackground(it->second);
                if(iReadSpeed < it->first)
                    break;
            }

        aReadTable->setItem(row, column, tableItem);
    }
    if((iMode == 2 || iMode == 3) && (aWriteTable))
    {
        row = iBlockNumber / aWriteTable->columnCount();
        column = iBlockNumber - row*aWriteTable->columnCount();
        QTableWidgetItem* tableItem = new QTableWidgetItem("");
        it = aColorMap.begin();
        if(iWriteSpeed == it->first)
            tableItem->setBackground(it->second);
        else
            for(it++; it!=aColorMap.end();it++)
            {
               tableItem->setBackground(it->second);
                if(iWriteSpeed < it->first)
                    break;
            }
        aWriteTable->setItem(row, column, tableItem);
    }

}

void CTestWindow::closeEvent(QCloseEvent *iCloseEvent)
{
    emit windowClosed();
}

QTableWidget* CTestWindow::createMap(QWidget *iParent)
{
    int rowCount = 20;
    int columnCount = 25;
    if(aCountBlocks > (columnCount*rowCount))
            rowCount = (!aCountBlocks%columnCount)? aCountBlocks/columnCount : aCountBlocks/columnCount+1;
    QTableWidget* returnedTable = new QTableWidget(rowCount, columnCount, iParent);
    for(int i=0; i<rowCount; i++)
        returnedTable->setRowHeight(i, 15);
    for(int i=0; i<columnCount; i++)
        returnedTable->setColumnWidth(i, 15);
    returnedTable->horizontalHeader()->setVisible(false);
    returnedTable->verticalHeader()->setVisible(false);
    returnedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    returnedTable->setSelectionMode(QAbstractItemView::SingleSelection);
    returnedTable->setSelectionMode(QAbstractItemView::NoSelection);
    returnedTable->setFixedSize(400,200);
    return returnedTable;
}
