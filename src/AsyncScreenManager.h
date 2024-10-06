#pragma once
#include "ScreenManager.h"
#include <QFuture>

class ScreenManager;
class SingleThreadTaskExecutor;
class TaskExecutor;

class AsyncScreenManager : public QObject
{
    Q_OBJECT
public:
    AsyncScreenManager(ScreenManager *screen, QObject *parent = nullptr);
    ~AsyncScreenManager();

    QFuture<QList<ScreenSession>> listSessions() const;
    QFuture<QString> retrieveSessionOutput(const QString &sessionId) const;
    QFuture<std::optional<ScreenManager::Process>> lastProcess(const QString &sessionId) const;
    QFuture<QString> workingDirectory(const QString &sessionId) const;
    QFuture<QString> workingDirectory(quint32 processPid) const;

    TaskExecutor *taskExecutor() const;

private:
    ScreenManager *m_screen = nullptr;
    SingleThreadTaskExecutor *m_executor = nullptr;
};
