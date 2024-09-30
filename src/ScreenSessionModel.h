#pragma once
#include "ScreenManager.h"
#include <QAbstractTableModel>

class ScreenSessionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        SessionIdColumn,
        StartedColumn,
        AttachedColumn,
        LastProcessColumn,
        DirectoryColumn,
        ColumnCount,
    };

    explicit ScreenSessionModel(ScreenManager *screen, QObject *parent = nullptr);

    ScreenManager *screenManager() const;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    const ScreenSession *session(int row) const;
    const ScreenSession *session(const QModelIndex &index) const;
    void remove(const QString &sessionId);

    static QString dateFormat();

public slots:
    void refresh();

private slots:
    void setSessions(QList<ScreenSession> sessions);
    void setLastProcess(const QString &sessionId, const QString &lastProcess);
    void setDirectory(const QString &sessionId, const QString &directory);

private:
    struct Row
    {
        ScreenSession session;
        ScreenManager::Process lastProcess;
        QString directory;
    };

    Row *row(const QString &sessionId);
    int rowIndex(const QString &sessionId);

    ScreenManager *m_screen = nullptr;
    QList<ScreenSession> m_sessions;
    QList<Row> m_rows;
    static QString s_dateFormat;
};
