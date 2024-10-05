#pragma once
#include "TaskExecutor.h"

class QThread;

class SingleThreadTaskExecutor : public TaskExecutor
{
    Q_OBJECT
public:
    SingleThreadTaskExecutor(QObject *parent = nullptr);

    bool wait(QDeadlineTimer deadline = QDeadlineTimer(QDeadlineTimer::Forever));
    bool wait(unsigned long time);

public slots:
    void start();

private:
    void run();

    QThread *m_executorThread = nullptr;
};
