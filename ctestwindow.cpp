#include "ctestwindow.h"

CTestWindow::CTestWindow(int iMode, long long iStartLBA, long long iEndLBA, int iBlockSize, QWidget *iParent):QWidget(iParent,Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint)
{
    aMode = iMode;
    aCountBlocks = (iEndLBA-iStartLBA+1)/iBlockSize+1;
    aStartLBA = iStartLBA;
    aEndLBA = iEndLBA;
    aBlockSize = iBlockSize;
    aReadTable = 0;
    aWriteTable = 0;
    aCountReadyBlock = 0;
    aColorMap[-1] = QColor("#000000");
    aColorMap[1999] = QColor("#CC0000");
    aColorMap[2000] = QColor("#996699");
    aColorMap[5000] = QColor("#6699CC");
    aColorMap[10000] = QColor("#CCCCFF");
    aColorMap[15000] = QColor("#CCFF99");
    aColorMap[18000] = QColor("#FFFFCC");
    aColorMap[20000] = QColor("#FFFF99");
    aColorMap[22000] = QColor("#FFCC99");
    aColorMap[25000] = QColor("#99CCCC");
    aColorMap[30000] = QColor("#CCFFCC");
}

void CTestWindow::show()
{

    QGridLayout* mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    aPauseButton = new QPushButton(QString::fromLocal8Bit("Пауза"),this);
    aPauseButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    connect(aPauseButton, SIGNAL(clicked()), this, SLOT(pauseButtonClick()));
    mainLayout->addWidget(aPauseButton, 0, 0);
    aRunButton = new QPushButton(QString::fromLocal8Bit("Возобновить"),this);
    aRunButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    aRunButton->setEnabled(false);
    connect(aRunButton, SIGNAL(clicked()), this, SLOT(runButtonClick()));
    mainLayout->addWidget(aRunButton, 0, 1);
    aStopButton = new QPushButton(QString::fromLocal8Bit("Прервать"),this);
    aStopButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    connect(aStopButton, SIGNAL(clicked()), this, SLOT(stopButtonClick()));
    mainLayout->addWidget(aStopButton, 0, 2);
    mainLayout->setRowMinimumHeight(0,50);


    aProgressBar = new QProgressBar(this);
    aProgressBar->setMaximum(100);
    aProgressBar->setMinimum(0);
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
        readLayout->addWidget(aReadTable,0, 0, 11, 1);
        setLegend(readLayout, 0, 1, this);
    }
    if(aMode == 2 ||aMode == 3)
    {
        QWidget* writeMap = new QWidget(this);
        tabWidget->addTab(writeMap,QString::fromLocal8Bit("Запись"));
        QGridLayout* writeLayout = new QGridLayout(writeMap);
        writeMap->setLayout(writeLayout);
        aWriteTable = createMap(this);
        writeLayout->addWidget(aWriteTable,0, 0, 11, 1);
    }
    aStartLBALabel = new QLabel(this);
    connect(aReadTable, SIGNAL(cellClicked(int,int)), this, SLOT(cellClick(int,int)));
    mainLayout->addWidget(aStartLBALabel, 3, 0, 1, 3);

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
        {
            it=aColorMap.end();
            for(it--; it!=aColorMap.begin();it--)
            {
               tableItem->setBackground(it->second);
                if(iReadSpeed > it->first)
                    break;
            }
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
    aCountReadyBlock++;
    aProgressBar->setValue(((double)aCountReadyBlock/aCountBlocks)*100);
}

void CTestWindow::viewError(QString iMessage)
{
    QMessageBox* testBox = new QMessageBox(QMessageBox::Critical,
                                           "Error",
                                           QString::fromLocal8Bit((iMessage.toStdString()).c_str()));
    testBox->exec();
}

void CTestWindow::pauseButtonClick()
{
    aPauseButton->setEnabled(false);
    aStopButton->setEnabled(false);
    aRunButton->setEnabled(true);
    emit pause();
}

void CTestWindow::runButtonClick()
{
    emit run();
    aPauseButton->setEnabled(true);
    aStopButton->setEnabled(true);
    aRunButton->setEnabled(false);
}

void CTestWindow::stopButtonClick()
{
    aPauseButton->setEnabled(false);
    aStopButton->setEnabled(false);
    aRunButton->setEnabled(false);
    emit stop();
}

void CTestWindow::cellClick(int iRow, int iColumn)
{
    long long startLBA = aStartLBA + iRow * (25)* aBlockSize + iColumn*aBlockSize;
    if(startLBA > aEndLBA)
    {
        aStartLBALabel->setText("");
        return;
    }
    long long endLBA = startLBA + aBlockSize - 1;
    if(endLBA > aEndLBA)
        endLBA = aEndLBA;
    aStartLBALabel->setText(QString::fromLocal8Bit("Сектора LBA : ")+QString::number(startLBA)+" - "+QString::number(endLBA));
}

void CTestWindow::testEnded()
{
    aPauseButton->setEnabled(false);
    aStopButton->setEnabled(false);
    aRunButton->setEnabled(false);
}

void CTestWindow::closeEvent(QCloseEvent *iCloseEvent)
{
    emit windowClosed();
}

QTableWidget* CTestWindow::createMap(QWidget *iParent)
{
    int rowCount = 17;
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
    returnedTable->setFixedSize(400,250);
    return returnedTable;
}

void CTestWindow::setLegend(QGridLayout *iLayout, int iRow, int iColumn, QWidget *iParent)
{
    QPixmap pixmap(32,16);
    QPainter painter;
    std::map<int, QColor>::iterator it = aColorMap.end();
    int i;
    for(i = 0,it--; it!=aColorMap.begin();it--, i++)
    {
        painter.begin(&pixmap);
        painter.drawRect(0,0,32,16);
        painter.fillRect(0,0,32,16,QBrush(it->second));
        painter.end();
        QLabel* label = new QLabel(iParent);
        label->setPixmap(pixmap);
        iLayout->addWidget(label, iRow+i, iColumn);
        QLabel* textlabel;
        if(i == (aColorMap.size()-2))
            textlabel = new QLabel("<"+QString::number(it->first+1)+" Kb/s",iParent);
        else
            textlabel = new QLabel(">"+QString::number(it->first)+" Kb/s",iParent);
        iLayout->addWidget(textlabel, iRow+i, iColumn+1);
    }
    it = aColorMap.begin();
    painter.begin(&pixmap);
    painter.drawRect(0,0,32,16);
    painter.fillRect(0,0,32,16,QBrush(it->second));
    painter.end();
    QLabel* label = new QLabel(iParent);
    label->setPixmap(pixmap);
    iLayout->addWidget(label, iRow+i, iColumn);
    QLabel* textlabel = new QLabel("bad",iParent);
    iLayout->addWidget(textlabel, iRow+i, iColumn+1);
}
