/****************************************************************************
**
** 版本：1.0
**                            数据库文件说明
** 概述：
**
** 数据库文件由一张存储参数文件名的数据表"configfilename"、一张参数调用记录表"configlog"、
** 一张存储参数表名称的数据表"configtablename"、一张记录现有参数的记录表"configargumentname"
** 和多张参数表组成。每张参数表对应“一款产品”的参数文件。
**
** "configfilename"表中有三列，分别是id、Configration、Description。Configration
** 是配置文件的名称，Description是对应的注释。每个配置文件名称最多45个字符，注释最多100个
** 字符。可在iVConfDB构造函数中修改。
**
** "configargumentname"表有三列，分别是id、argument、location。argument是参数的名称，
** location是该参数所属的配置文件名。
**
** 每张参数表有四列，分别是id、Argument、Value、Location。Location表示该参数所属的配置
** 文件名，即configtablename表中的某个参数名。Argument是参数名称，Value是对应的值。
**
** "configlog"表在类构造时读取，析构时被写入。保存上一次最后调用SetArgument()函数时的数据表
** 参数并在本次程序运行时调入。
**
** 注意：
**
** 由于一款产品的所有参数都存储在一张列中，所以无法区分数据类型，统一用字符串存储。
**
****************************************************************************/

#ifndef IVCONFDB_H
#define IVCONFDB_H

#include <QSqlTableModel>
#include <QVariant>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>

#include <QFileInfo>
#include <QTime>
#include "assert.h"




class iVConfDB
{
public:
    iVConfDB(const QString &dbDir);
    virtual ~iVConfDB();

    // 返回上次运行的表名及所有内容
    bool GetLastUsedTable(QString& tbName, QStringList& arg, QStringList& value) const;
    // 仅返回上次运行的表名
    bool GetLastUsedTable(QString& tbName);

    // 返回所有的配置表名
    bool ConfigTables(QStringList& tbNames) const;
    // 为某个产品创建一张配置表
    bool CreateConfigTable(const QString& tableName);
    bool DeleteConfigTable(const QString& tbName);

    // 返回所有的配置文件名及注释
    bool ConfigFiles(QStringList &configFileName, QStringList &description) const;
    // 返回指定配置文件的注释
    bool GetConfigFileDescription(const QString& fileName, QString& description);
    // 在configfilename表中添加记录（添加配置文件）
    bool AddConfigFile(const QString& fileName, const QString& description);
    //bool DeleteConfigFile(const QString& configname);

    // 在每个产品的配置表中添加/修改一个新的参数
    bool SetArgument(const QString& tableName, const QString& argName, const QVariant& value, const QString &location);
    // 获取一个参数表中的某项参数
    bool GetArgument(QString& receiver, const QString &tbName, const QString &argName) const;
    bool GetArgument(QVariant &receiver, const QString& tbName, const QString& argName) const;
    // 获取一个参数表中属于指定配置文件的全部参数
    bool GetSingleConfArgument(QStringList &arg, QStringList &value, const QString &tbName, const QString &fileName) const;
    // 获取一个参数表的全部参数
    bool GetConfigTable(const QString &tbName, QStringList& arg, QStringList& value) const;

    bool IsFirstUse() const;
    bool IsConfigTableExist(const QString& tableName) const;
    bool IsArgumentExist(const QString& ArgumentName) const;
    bool IsLocationExist(const QString& location) const;
    // 与上一个函数相同
    bool IsConfigFileExist(const QString& confFile) const;

    bool IsImageLabeled(const QString& imageName);
    bool AddLabel(const QString& imageName, const QVector<int>& coordiantes);
    bool GetLabel(const QString& imageName, QSqlRecord& receiver);

private:
    // 创建到数据库的连接，如果指定的数据库文件不存在则自动创建
    bool CreateConnection(const QString &dbDir);

    // 创建/删除参数表时，向configtablename表中更改记录
    bool AddTableNameRecord(const QString& tbName);
    bool DeleteTableNameRecord(const QString& tbName);
    bool AddConfigFileRecord(const QString& fileName, const QString& description);
    bool DeleteConfigFileRecord(const QString& fileName);

    // 设置了新参数，提交至configargumentname表
    bool SubmitNewArgument(const QString& argName, const QString &location);


    QSqlTableModel* tableModel;
    QString databaseName;

    QStringList configFile;
    QStringList configFileDescription;
    QStringList configTable;

    QString lastUsedTable;
};


#endif // IVCONFDB_H
