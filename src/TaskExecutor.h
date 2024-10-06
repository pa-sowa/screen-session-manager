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
    enum class Priority { High, Normal, Low };

    TaskExecutor(QObject *parent = nullptr);

    /// Add a task to the task queue with a result callback to be called on the same (task) thread.
    void addTask(std::function<QVariant()> lambda,
                 std::function<void(QVariant)> resultCallback,
                 Priority priority = Priority::Normal);

    /// Add a task to the task queue with a result callback to be called on the specified object's thread.
    /// The task will be canceled if the object is destroyed before the task is executed.
    void addTask(std::function<QVariant()> lambda,
                 QObject *callbackObject,
                 std::function<void(QVariant)> resultCallback,
                 Priority priority = Priority::Normal);

    QFuture<QVariant> addTask(std::function<QVariant()> lambda,
                              Priority priority = Priority::Normal);

    void stop();

    void run();

signals:
    void finished();

private slots:
    void onCallbackObjectDestroyed();

private:
    struct Task
    {
        Priority priority = Priority::Normal;
        std::function<QVariant()> lambda;
        std::function<void(QVariant)> resultCallback;
        QObject *callbackObject = nullptr;
        QFutureInterface<QVariant> future;
    };

    int indexForInsertion(Priority priority) const;

    QQueue<Task> m_taskQueue;
    QMutex m_mutex;
    QWaitCondition m_taskAvailable;
    bool m_stop = false;
};
