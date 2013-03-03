#ifndef CTESTWINDOW_H
#define CTESTWINDOW_H
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QTabWidget>
#include <QHeaderView>
#include <QTableWidget>
#include <QMessageBox>
#include <QLabel>
#include <QPainter>
#include <map>

class CTestWindow: public QWidget
{
    Q_OBJECT
public:
    CTestWindow(int iMode, long long iStartLBA, long long iEndLBA, int iBlockSize, QWidget *iParent = 0);
public slots:
    void show();

signals:
    void windowClosed();
    void pause();
    void stop();
    void run();
protected:
    void closeEvent(QCloseEvent *iCloseEvent);
private slots:
    void addBlock(int iMode, int iBlockNumber, int iReadSpeed, int iWriteSpeed);
    void viewError(QString iMessage);
    void pauseButtonClick();
    void runButtonClick();
    void stopButtonClick();
    void cellClick(int iRow, int iColumn);
    void testEnded();
private:
    QTableWidget* createMap(QWidget* iParent = 0);
    void setLegend(QGridLayout* iLayout, int iRow, int iColumn, QWidget* iParent = 0);
private:
    int aMode;
    long long aStartLBA;
    long long aEndLBA;
    int aBlockSize;
    int aCountBlocks;
    std::map<int, QColor> aColorMap;
    QProgressBar* aProgressBar;
    QTableWidget* aReadTable;
    QTableWidget* aWriteTable;
    QPushButton* aPauseButton;
    QPushButton* aRunButton;
    QPushButton* aStopButton;
    QLabel* aStartLBALabel;
    long long aCountReadyBlock;
};

#endif // CTESTWINDOW_H
