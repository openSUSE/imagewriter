#ifndef METATYPES_H
#define METATYPES_H

#include <QtCore>
#include <QtDBus>

typedef QMap<QString,QVariantMap> QVariantMapMap;
Q_DECLARE_METATYPE(QVariantMapMap)

typedef QMap<QDBusObjectPath, QVariantMapMap> DBUSManagerStruct;
Q_DECLARE_METATYPE(DBUSManagerStruct)

typedef QList<QByteArray> ByteArrayList;
Q_DECLARE_METATYPE(ByteArrayList)

#endif // METATYPES_H
