#ifndef CTEST_H
#define CTEST_H
#include <QObject>

class CTest : public QObject
{
    Q_OBJECT
public:
    CTest();
public slots:
    void run();
signals:
    void testEnded();
};

#endif // CTEST_H
