#include "SshHostConfig.h"
#include <QJsonObject>

bool SshHostConfig::isValid() const
{
    return !host.isEmpty() && !user.isEmpty();
}

QJsonObject SshHostConfig::toJson() const
{
    QJsonObject obj;
    obj["host"] = host;
    obj["user"] = user;
    obj["identityFile"] = identityFile;
    return obj;
}

SshHostConfig SshHostConfig::fromJson(const QJsonObject &json)
{
    SshHostConfig config;
    config.host = json["host"].toString();
    config.user = json["user"].toString();
    config.identityFile = json["identityFile"].toString();
    return config;
}
