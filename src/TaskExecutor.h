#pragma once
#include <QFuture>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QVariant>
#include <QWaitCondition>

class TaskExecutor : public QObject
{
    Q_OBJECT
public:
    TaskExecutor(QObject *parent = nullptr);
    ~TaskExecutor();

    void addTask(std::function<QVariant()> lambda, std::function<void(QVariant)> resultCallback);

    void addTask(std::function<QVariant()> lambda,
                 QObject *callbackObject,
                 std::function<void(QVariant)> resultCallback);

    QFuture<QVariant> addTask(std::function<QVariant()> lambda);

    void prependTask(std::function<QVariant()> lambda,
                     QObject *callbackObject,
                     std::function<void(QVariant)> resultCallback);

    void stop();

    void run();

signals:
    void finished();

private:
    struct Task
    {
        std::function<QVariant()> lambda;
        std::function<void(QVariant)> resultCallback;
        QObject *callbackObject = nullptr;
        QFutureInterface<QVariant> future;
    };

    QQueue<Task> m_taskQueue;
    QMutex m_mutex;
    QWaitCondition m_taskAvailable;
    bool m_stop = false;
};
