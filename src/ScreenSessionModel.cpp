#include "ScreenSessionModel.h"
#include <QTimer>
#include <QtConcurrent/QtConcurrent>

QString ScreenSessionModel::s_dateFormat = "yyyy-MM-dd hh:mm:ss";

ScreenSessionModel::ScreenSessionModel(ScreenManager *screen, QObject *parent)
    : QAbstractTableModel(parent)
    , m_screen(screen)
{
    assert(m_screen);
}

ScreenManager *ScreenSessionModel::screenManager() const
{
    return m_screen;
}

void ScreenSessionModel::refresh()
{
    beginResetModel();
    m_rows.clear();
    endResetModel();

    auto future = QtConcurrent::run([this]() {
        auto sessions = m_screen->listSessions();
        QMetaObject::invokeMethod(this,
                                  "setSessions",
                                  Qt::QueuedConnection,
                                  Q_ARG(QList<ScreenSession>, sessions));

        for (const auto &session : sessions) {
            if (auto process = m_screen->lastProcess(session.id)) {
                QMetaObject::invokeMethod(this,
                                          "setLastProcess",
                                          Qt::QueuedConnection,
                                          Q_ARG(QString, session.id),
                                          Q_ARG(QString, process->name));

                QString dir = m_screen->workingDirectory(process->pid);
                if (!dir.isEmpty()) {
                    QMetaObject::invokeMethod(this,
                                              "setDirectory",
                                              Qt::QueuedConnection,
                                              Q_ARG(QString, session.id),
                                              Q_ARG(QString, dir));
                }
            }
        }
    });
}

void ScreenSessionModel::setSessions(QList<ScreenSession> sessions)
{
    beginResetModel();
    m_rows.clear();
    m_rows.reserve(sessions.size());

    for (const auto &session : sessions) {
        Row row;
        row.session = session;
        m_rows.append(row);
    }
    endResetModel();
}

void ScreenSessionModel::setLastProcess(const QString &sessionId, const QString &lastProcess)
{
    int r = rowIndex(sessionId);
    if (r > -1) {
        m_rows[r].lastProcess.name = lastProcess;
        auto idx = index(r, LastProcessColumn);
        emit dataChanged(idx, idx, QList<int>{Qt::DisplayRole});
    }
}

void ScreenSessionModel::setDirectory(const QString &sessionId, const QString &directory)
{
    int r = rowIndex(sessionId);
    if (r > -1) {
        m_rows[r].directory = directory;
        auto idx = index(r, DirectoryColumn);
        emit dataChanged(idx, idx, QList<int>{Qt::DisplayRole});
    }
}

ScreenSessionModel::Row *ScreenSessionModel::row(const QString &sessionId)
{
    for (int i = 0; i < m_rows.size(); ++i) {
        if (m_rows[i].session.id == sessionId) {
            return &m_rows[i];
        }
    }
    return nullptr;
}

int ScreenSessionModel::rowIndex(const QString &sessionId)
{
    for (int i = 0; i < m_rows.size(); ++i) {
        if (m_rows[i].session.id == sessionId) {
            return i;
        }
    }
    return -1;
}

QVariant ScreenSessionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case SessionIdColumn:
            return tr("Session ID");
        case StartedColumn:
            return tr("Started");
        case AttachedColumn:
            return tr("Attached");
        case LastProcessColumn:
            return tr("Last Process");
        case DirectoryColumn:
            return tr("Working Directory");
        }
    }
    return {};
}

int ScreenSessionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_rows.size();
}

int ScreenSessionModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return ColumnCount;
}

QVariant ScreenSessionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_rows.size() || role != Qt::DisplayRole)
        return QVariant();

    const auto &row = m_rows.at(index.row());
    switch (index.column()) {
    case SessionIdColumn:
        return row.session.pid();
    case StartedColumn:
        return row.session.started.toString(s_dateFormat);
    case AttachedColumn:
        return row.session.attached;
    case LastProcessColumn:
        return row.lastProcess.toString();
    case DirectoryColumn:
        return row.directory;
    }

    return QVariant();
}

const ScreenSession *ScreenSessionModel::session(const QModelIndex &index) const
{
    return session(index.row());
}

const ScreenSession *ScreenSessionModel::session(int row) const
{
    if (-1 < row && row < m_rows.size()) {
        return &m_rows.at(row).session;
    } else {
        return nullptr;
    }
}

void ScreenSessionModel::remove(const QString &sessionId)
{
    int r = rowIndex(sessionId);
    if (r > -1) {
        beginRemoveRows(QModelIndex(), r, r);
        m_rows.removeAt(r);
        endRemoveRows();
    }
}

QString ScreenSessionModel::dateFormat()
{
    return s_dateFormat;
}
