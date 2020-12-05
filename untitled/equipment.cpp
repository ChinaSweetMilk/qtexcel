#include "equipment.h"
#include <objbase.h>

Equipment::Equipment(QObject *parent) : QObject(parent)
{
    m_iSumEquipmentCounts = 0;
}

Equipment::~Equipment()
{
    m_strSubSystemSN.clear();
    m_strEquipmentSN.clear();
    m_strSubSystemName.clear();
    m_strEquipmentName.clear();
    m_mapInstallationPosition.clear();
}

int Equipment::getSumEquipmentCounts()
{
    return m_iSumEquipmentCounts;
}

bool Equipment::setSubSystemName(QString strname)
{
   if(strname.isEmpty())
   {
       return false;
   }
   m_strSubSystemName.clear();
   m_strSubSystemName = strname;

   return true;
}

bool Equipment::setEquipmentName(QString strname)
{
   if(strname.isEmpty())
   {
       return false;
   }
   m_strEquipmentName.clear();
   m_strEquipmentName = strname;

   return true;
}

bool Equipment::setSubSystemSN(QString strname)
{
    if(strname.isEmpty())
    {
        return false;
    }
    m_strSubSystemSN.clear();
    m_strSubSystemSN = strname;

    return true;
}

bool Equipment::setEquipmentSN(QString strname)
{
    if(strname.isEmpty())
    {
        return false;
    }
    m_strEquipmentSN.clear();
    m_strEquipmentSN = strname;

    return true;
}

QString Equipment::getSubSystemSN()
{
    return m_strSubSystemSN;
}

QString Equipment::getEquipmentSN()
{
    return m_strEquipmentSN;
}

QString Equipment::getSubSystemName()
{
   return m_strSubSystemName;
}

QString Equipment::getEquipmentName()
{
   return m_strEquipmentName;
}

bool Equipment::intsertMapInstallationPosition(QString strname,unsigned int icounts)
{
   if(strname.isEmpty() || icounts == 0)
   {
       return false;
   }

   m_iSumEquipmentCounts += icounts;
   m_mapInstallationPosition.insert(strname, icounts);

   return true;
}

QMap<QString, unsigned int>  Equipment::getMapInstallationPosition()
{
   return m_mapInstallationPosition;
}
