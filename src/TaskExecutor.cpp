#include "TaskExecutor.h"
#include <QDebug>
#include <QMetaObject>
#include <QThread>

TaskExecutor::TaskExecutor(QObject *parent)
    : QObject(parent)
{}

TaskExecutor::~TaskExecutor() {}

void TaskExecutor::addTask(std::function<QVariant()> lambda,
                           std::function<void(QVariant)> resultCallback)
{
    Q_ASSERT(lambda);
    addTask(lambda, nullptr, resultCallback);
}

void TaskExecutor::addTask(std::function<QVariant()> lambda,
                           QObject *callbackObject,
                           std::function<void(QVariant)> resultCallback)
{
    Q_ASSERT(lambda);
    QMutexLocker locker(&m_mutex);
    m_taskQueue.enqueue({std::move(lambda), std::move(resultCallback), callbackObject});
    m_taskAvailable.wakeOne();
}

void TaskExecutor::prependTask(std::function<QVariant()> lambda,
                               QObject *callbackObject,
                               std::function<void(QVariant)> resultCallback)
{
    Q_ASSERT(lambda);
    QMutexLocker locker(&m_mutex);
    m_taskQueue.prepend({std::move(lambda), std::move(resultCallback), callbackObject});
    m_taskAvailable.wakeOne();
}

QFuture<QVariant> TaskExecutor::addTask(std::function<QVariant()> lambda)
{
    Q_ASSERT(lambda);
    QMutexLocker locker(&m_mutex);

    QFutureInterface<QVariant> futureInterface;
    futureInterface.reportStarted();

    m_taskQueue.enqueue({std::move(lambda), {}, nullptr, futureInterface});
    m_taskAvailable.wakeOne();
    return futureInterface.future();
}

void TaskExecutor::stop()
{
    QMutexLocker locker(&m_mutex);
    m_stop = true;
    m_taskAvailable.wakeOne();
}

void TaskExecutor::run()
{
    qDebug() << "TaskExecutor: started, thread id: " << QThread::currentThreadId();
    while (true) {
        Task task;
        {
            QMutexLocker locker(&m_mutex);
            if (m_taskQueue.isEmpty()) {
                m_taskAvailable.wait(&m_mutex);
            }

            if (m_stop) {
                qDebug() << "TaskExecutor: stopping";
                emit finished();
                return;
            }

            qDebug() << "TaskExecutor: task queue size: " << m_taskQueue.size();
            if (!m_taskQueue.isEmpty()) {
                task = m_taskQueue.dequeue();
            }
        }

        if (task.lambda) {
            QVariant result = task.lambda();
            if (task.resultCallback) {
                if (task.callbackObject) {
                    QMetaObject::invokeMethod(task.callbackObject,
                                              [task, result]() { task.resultCallback(result); });
                } else {
                    task.resultCallback(result);
                }
            } else if (!task.future.isCanceled()) {
                task.future.reportResult(result);
                task.future.reportFinished();
            }
        } else {
            qDebug() << "TaskExecutor: no lambda to execute";
        }
    }
}
