#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <QObject>
#include <QMap>
#include <QString>

class Equipment : public QObject
{
    Q_OBJECT
public:
    explicit Equipment(QObject *parent = nullptr);
    ~Equipment();
    int getSumEquipmentCounts();
    bool setSubSystemName(QString strname);
    bool setEquipmentName(QString strname);
    QString getSubSystemName();
    QString getEquipmentName();
    bool setSubSystemSN(QString strname);
    bool setEquipmentSN(QString strname);
    QString getSubSystemSN();
    QString getEquipmentSN();

    bool intsertMapInstallationPosition(QString strname,unsigned int icounts);
    QMap<QString, unsigned int> getMapInstallationPosition();

signals:

public slots:
private:
    int             m_iSumEquipmentCounts;
    QString         m_strSubSystemSN;
    QString         m_strEquipmentSN;
    QString         m_strSubSystemName;
    QString         m_strEquipmentName;
    QMap<QString, unsigned int> m_mapInstallationPosition;
};

#endif // EQUIPMENT_H


