#include "ctestwindow.h"

CTestWindow::CTestWindow(int iMode, int iCountBlocks, QWidget *iParent):QWidget(iParent,Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint)
{
    aMode = iMode;
    aCountBlocks = iCountBlocks;
}

void CTestWindow::show()
{
    QWidget::show();
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
        QTableWidget* readTable = createMap(this);
        readLayout->addWidget(readTable,0, 0, 8, 1);
    }
    if(aMode == 2 ||aMode == 3)
    {
        QWidget* writeMap = new QWidget(this);
        tabWidget->addTab(writeMap,QString::fromLocal8Bit("Запись"));
        QGridLayout* writeLayout = new QGridLayout(writeMap);
        writeMap->setLayout(writeLayout);
        QTableWidget* writeTable = createMap(this);
        writeLayout->addWidget(writeTable,0, 0, 8, 1);
    }

    adjustSize();
}

void CTestWindow::closeEvent(QCloseEvent *iCloseEvent)
{
    emit windowClosed();
}

QTableWidget* CTestWindow::createMap(QWidget *iParent)
{
    int rowCount = 50;
    int columnCount = 30;
    QTableWidget* returnedTable = new QTableWidget(iParent);
    returnedTable->horizontalHeader()->setVisible(false);
    returnedTable->verticalHeader()->setVisible(false);
    returnedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    returnedTable->setSelectionMode(QAbstractItemView::SingleSelection);
    returnedTable->setSelectionMode(QAbstractItemView::NoSelection);
    returnedTable->setFixedSize(400,300);
    return returnedTable;
}
