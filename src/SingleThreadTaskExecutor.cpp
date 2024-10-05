#include "SingleThreadTaskExecutor.h"

SingleThreadTaskExecutor::SingleThreadTaskExecutor(QObject *parent)
    : TaskExecutor(parent)
{
    m_executorThread = new QThread(this);
    moveToThread(m_executorThread);
    connect(m_executorThread, &QThread::started, this, &TaskExecutor::run);
    connect(this, &TaskExecutor::finished, m_executorThread, &QThread::quit);
}

bool SingleThreadTaskExecutor::wait(QDeadlineTimer deadline)
{
    return m_executorThread->wait(deadline);
}

bool SingleThreadTaskExecutor::wait(unsigned long time)
{
    return m_executorThread->wait(time);
}

void SingleThreadTaskExecutor::start()
{
    m_executorThread->start();
}
