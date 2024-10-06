#include "AsyncScreenManager.h"
#include "SingleThreadTaskExecutor.h"
#include <functional>
#include <QFutureWatcher>
#include <QPromise>
#include <QString>
#include <QVariant>

namespace {

template<typename T>
QFuture<T> convertFuture(QFuture<QVariant> innerFuture, std::function<T(const QVariant &)> converter)
{
    QPromise<T> promise;
    QFuture<T> future = promise.future();
    auto *watcher = new QFutureWatcher<QVariant>();

    QObject::connect(watcher,
                     &QFutureWatcher<QVariant>::finished,
                     [=, promise = std::move(promise)]() mutable {
                         if (innerFuture.isCanceled()) {
                             promise.future().cancel();
                         } else if (innerFuture.isFinished()) {
                             try {
                                 QVariant variant = innerFuture.result();
                                 T result = converter(variant);

                                 promise.addResult(result);
                                 promise.finish();
                             } catch (...) {
                                 promise.setException(std::current_exception());
                             }
                         }
                         watcher->deleteLater();
                     });

    watcher->setFuture(innerFuture);
    return future;
}

QString toString(const QVariant &variant)
{
    return variant.toString();
}

} // anonymous namespace

AsyncScreenManager::AsyncScreenManager(ScreenManager *screen, QObject *parent)
    : QObject(parent)
    , m_screen(screen)
{
    m_executor = new SingleThreadTaskExecutor();
    m_executor->start();
}

AsyncScreenManager::~AsyncScreenManager()
{
    m_executor->stop();
    m_executor->wait();
    delete m_executor;
    qDebug() << "AsyncScreenManager::~AsyncScreenManager() thread stopped";
}

QFuture<QList<ScreenSession>> AsyncScreenManager::listSessions() const
{
    auto task = [this]() {
        QList<ScreenSession> sessions = m_screen->listSessions();
        return QVariant::fromValue(sessions);
    };
    QFuture<QVariant> innerFuture = m_executor->addTask(task);
    return convertFuture<QList<ScreenSession>>(innerFuture, [](const QVariant &variant) {
        return variant.value<QList<ScreenSession>>();
        ;
    });
}

QFuture<QString> AsyncScreenManager::retrieveSessionOutput(const QString &sessionId) const
{
    auto task = [this, sessionId]() { return m_screen->retrieveSessionOutput(sessionId); };
    QFuture<QVariant> innerFuture = m_executor->addTask(task);
    return convertFuture<QString>(innerFuture, toString);
}

QFuture<std::optional<ScreenManager::Process>> AsyncScreenManager::lastProcess(
    const QString &sessionId) const
{
    auto task = [this, sessionId]() -> QVariant {
        std::optional<ScreenManager::Process> p = m_screen->lastProcess(sessionId);
        if (p) {
            return QVariantList() << p->pid << p->name;
        } else {
            return QVariant();
        }
    };
    QFuture<QVariant> innerFuture = m_executor->addTask(task);
    return convertFuture<std::optional<ScreenManager::Process>>(
        innerFuture, [](const QVariant &variant) {
            if (variant.isNull()) {
                return std::optional<ScreenManager::Process>();
            } else {
                auto list = variant.toList();
                ScreenManager::Process p;
                p.pid = list[0].toInt();
                p.name = list[1].toString();
                return std::optional<ScreenManager::Process>(p);
            }
        });
}

QFuture<QString> AsyncScreenManager::workingDirectory(const QString &sessionId) const
{
    auto task = [this, sessionId]() { return m_screen->workingDirectory(sessionId); };
    QFuture<QVariant> innerFuture = m_executor->addTask(task);
    return convertFuture<QString>(innerFuture, toString);
}

QFuture<QString> AsyncScreenManager::workingDirectory(quint32 processPid) const
{
    auto task = [this, processPid]() { return m_screen->workingDirectory(processPid); };
    QFuture<QVariant> innerFuture = m_executor->addTask(task);
    return convertFuture<QString>(innerFuture, toString);
}

TaskExecutor *AsyncScreenManager::taskExecutor() const
{
    return m_executor;
}
