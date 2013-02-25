#ifndef CTESTWINDOW_H
#define CTESTWINDOW_H
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QTabWidget>
#include <QHeaderView>
#include <QTableWidget>

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
private:
    QTableWidget* createMap(QWidget* iParent = 0);
private:
    int aMode;
    int aCountBlocks;
    QProgressBar* aProgressBar;

};

#endif // CTESTWINDOW_H
