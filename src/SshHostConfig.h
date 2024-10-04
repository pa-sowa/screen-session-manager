#pragma once
#include <QString>

class QJsonObject;

struct SshHostConfig
{
    QString host; // required
    QString user;
    QString identityFile;

    bool isValid() const;
    QJsonObject toJson() const;
    static SshHostConfig fromJson(const QJsonObject &json);
};
