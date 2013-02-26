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
#include <map>

class CTestWindow: public QWidget
{
    Q_OBJECT
public:
    CTestWindow(int iMode, int iCountBlocks, QWidget *iParent = 0);
public slots:
    void show();

signals:
    void windowClosed();
protected:
    void closeEvent(QCloseEvent *iCloseEvent);
private slots:
    void addBlock(int iMode, int iBlockNumber, int iReadSpeed, int iWriteSpeed);
    void viewError(QString iMessage);
private:
    QTableWidget* createMap(QWidget* iParent = 0);
private:
    int aMode;
    int aCountBlocks;
    std::map<int, QColor> aColorMap;
    QProgressBar* aProgressBar;
    QTableWidget* aReadTable;
    QTableWidget* aWriteTable;
};

#endif // CTESTWINDOW_H
