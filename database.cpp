#include "database.h"

#define bug qDebug() <<
#define sqlBug if (!query.isActive()) {bug query.lastError().text();return false;}

iVConfDB::iVConfDB(const QString &dbDir)
{
    lastUsedTable.clear();

    if (!CreateConnection(dbDir))
    {
        return;
    }
    tableModel = new QSqlTableModel;

    QSqlQuery query;
    QString order = QString("CREATE TABLE IF NOT EXISTS `SonoDataset` ("
                            "`id` INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "`imagename` VARCHAR(45) NOT NULL,"
                            "`imagetype` VARCHAR(45),"
                            "`P1x` INT,"
                            "`P1y` INT,"
                            "`P2x` INT,"
                            "`P2y` INT,"
                            "`P3x` INT,"
                            "`P3y` INT,"
                            "`P4x` INT,"
                            "`P4y` INT);");
    query.exec(order);

    order = QString("CREATE TABLE IF NOT EXISTS `configfilename` ("
                            "`id` INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "`configration` VARCHAR(45) NOT NULL,"
                            "`description` VARCHAR(100));");
    query.exec(order);

    order = QString("CREATE TABLE IF NOT EXISTS `configtablename` ("
                    "`id` INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "`configtable` VARCHAR(45) NOT NULL);");
    query.exec(order);

    order = QString("CREATE TABLE IF NOT EXISTS `configargumentname` ("
                    "`id` INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "`argument` VARCHAR(45) NOT NULL,"
                    "`location` VARCHAR(45) NOT NULL);");
    query.exec(order);

    tableModel->setTable("SonoDataset");
    tableModel->select();
//    for (int i = 0; i < tableModel->rowCount(); i++)
//    {
//        configTable << tableModel->record(i).value(1).toString();
//    }

//    tableModel->setTable("configfilename");
//    tableModel->select();
//    for (int i = 0; i < tableModel->rowCount(); i++)
//    {
//        configFile << tableModel->record(i).value(1).toString();
//        configFileDescription << tableModel->record(i).value(2).toString();
//    }
}

iVConfDB::~iVConfDB()
{
//    QSqlQuery query;
//    QString order = QString("CREATE TABLE IF NOT EXISTS `configlog` ("
//                            "`id` INTEGER PRIMARY KEY AUTOINCREMENT,"
//                            "`lasttable` VARCHAR(45) NOT NULL,"
//                            "`modificationtime` VARCHAR(50) NOT NULL);");
//    query.exec(order);

//    if (!lastUsedTable.isEmpty())
//    {
//        QSqlRecord record;
//        record.append(QSqlField("lasttable"));
//        record.append(QSqlField("modificationtime"));
//        record.setValue(0, lastUsedTable);
//        QString time = QDate::currentDate().toString("yy.MM.dd") + QString("  ")
//                       + QTime::currentTime().toString("h:m:s");
//        record.setValue(1, time);

//        tableModel->setTable("configlog");
//        tableModel->insertRecord(-1, record);
//        bug QString("Check in new using log. Table : %1").arg(lastUsedTable);
//    }

    delete tableModel;
}

bool iVConfDB::CreateConnection(const QString &dbDir)
{
    QFileInfo info(dbDir);
    if (info.exists())
    {
        bug QString("Database file %1 exists. Loading...").arg(dbDir);
    }
    else
    {
        bug QString("Database file %1 does NOT exist. Creating...").arg(dbDir);
    }
    databaseName = dbDir;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbDir);

    if (!db.open())
    {
        bug 53;
        bug db.lastError().text();
        return false;
    }
    else
    {
        bug QString("Open database %1 success").arg(db.databaseName());
    }

    return true;
}

bool iVConfDB::GetLastUsedTable(QString &tbName, QStringList &arg, QStringList &value) const
{
    tableModel->setTable("configlog");
    if (!tableModel->select())
    {
        bug tableModel->lastError().text();
        bug "First use, last table does not exist.";
        return false;
    }
    tbName = tableModel->record(tableModel->rowCount() - 1).value(1).toString();

    GetConfigTable(tbName, arg, value);

    return true;
}

bool iVConfDB::GetLastUsedTable(QString &tbName)
{
    tableModel->setTable("configlog");
    if (!tableModel->select())
    {
        bug tableModel->lastError().text();
        bug "First use, last table does not exist.";
        return false;
    }
    tbName = tableModel->record(tableModel->rowCount() - 1).value(1).toString();

    return true;
}

bool iVConfDB::ConfigTables(QStringList &tbNames) const
{
    tbNames = configTable;
    return true;
}

bool iVConfDB::ConfigFiles(QStringList& configFileName, QStringList& description) const
{
    configFileName = configFile;
    description = configFileDescription;
    return true;
}

bool iVConfDB::GetConfigFileDescription(const QString &fileName, QString &description)
{
    int index = configFile.indexOf(fileName);
    if (index == -1)
    {
        return false;
    }
    description = configFileDescription[index];

    return true;
}

bool iVConfDB::CreateConfigTable(const QString &tbName)
{
    QSqlQuery query;
    bool exist = false;
    if (IsConfigTableExist(tbName))
    {
        exist = true;
        bug QString("table '%1' exists, replacing...").arg(tbName);
        query.exec(QString("DROP TABLE %1;").arg(tbName));
        sqlBug;
    }
    QString order = QString("CREATE TABLE %1 ("
                            "id       INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "Argument VARCHAR(40) NOT NULL,"
                            "Value    VARCHAR(40),"
                            "Location VARCHAR(40) NOT NULL);").arg(tbName);
    query.exec(order);
    sqlBug;

    tableModel->setTable("configargumentname");
    if (tableModel->select())
    {
        QStringList arg;
        QStringList location;
        for (int i = 0; i < tableModel->rowCount(); i++)
        {
            arg << tableModel->record(i).value(1).toString();
            location << tableModel->record(i).value(2).toString();
        }

        tableModel->setTable(tbName);
        tableModel->select();
        QSqlRecord record;
        record.append(QSqlField("argument"));
        record.append(QSqlField("location"));

        for (int i = 0; i < arg.count(); i++)
        {
            record.setValue(0, arg[i]);
            record.setValue(1, location[i]);
            tableModel->insertRecord(-1, record);
        }
    }

    if (!exist)
    {
        configTable << tbName;
        return AddTableNameRecord(tbName);
    }

    return true;
}

bool iVConfDB::DeleteConfigTable(const QString &tbName)
{
    QString order = QString("DROP TABLE %1").arg(tbName);
    QSqlQuery query;
    query.exec(order);
    sqlBug;

    configTable.removeAll(tbName);
    return DeleteTableNameRecord(tbName);
}

bool iVConfDB::AddConfigFile(const QString &fileName, const QString &description)
{
    tableModel->setTable("configfilename");
    tableModel->setFilter(QString("Configration = '%1'").arg(fileName));
    tableModel->select();
    bool exist = false;

    if (tableModel->rowCount() != 0)
    {
        exist = true;
        bug QString("file '%1' exists, replacing...").arg(fileName);
        tableModel->removeRow(0);
        //tableModel->submitAll();
    }

    QSqlRecord record;
    record.append(QSqlField("configration", QVariant::String));
    record.append(QSqlField("description", QVariant::String));
    record.setValue(0, fileName);
    record.setValue(1, description);

    tableModel->setTable("configfilename");
    tableModel->select();
    tableModel->insertRecord(-1, record);

    if (!exist)
    {
        configFile << fileName;
        configFileDescription << description;
    }
    else
    {
        int index = configFile.indexOf(QString("%1").arg(fileName));
        configFileDescription[index] = description;
    }

    return AddConfigFileRecord(fileName, description);
}
/*
bool iVConfDB::DeleteConfigFile(const QString &configname)
{
    tableModel->setTable("configfilename");
    tableModel->setFilter(QString("configration = '%1'").arg(configname));
    tableModel->select();
    if (tableModel->rowCount() == 0)
    {
        bug QString("ERROR : configration %1 does not exist.").arg(configname);
        return false;
    }
    tableModel->removeRow(0);
    //tableModel->submitAll();

    int index = configFile.indexOf(configname);
    configFile.removeOne(configname);
    configFileDescription.removeAt(index);

    return true;
}
*/
bool iVConfDB::SetArgument(const QString &tbName, const QString &argName, const QVariant &value, const QString& location)
{
    if (!IsConfigTableExist(tbName))
    {
        bug QString("ERROR : table %1 does not exist.").arg(tbName);
        return false;
    }
    if (!IsLocationExist(location))
    {
        bug QString("ERROR : location %1 does not exist.").arg(location);
        return false;
    }

    QSqlQuery query;
    if (IsArgumentExist(argName))
    {
        query.exec(QString("UPDATE `%1` SET `Value` = '%2' WHERE `Argument` = '%3';")
                   .arg(tbName).arg(value.toString()).arg(argName));
        sqlBug;

        lastUsedTable = tbName;
        return true;
    }

    SubmitNewArgument(argName, location);
    for (int i = 0; i < configTable.count(); i++)
    {
        query.prepare(QString("INSERT INTO %1 (Argument, Location)"
                              "VALUES (?, ?)").arg(configTable[i]));
        query.addBindValue(argName);
        //query.addBindValue(value.toString());
        query.addBindValue(location);
        query.exec();
        sqlBug;
    }

    query.exec(QString("UPDATE `%1` SET `Value` = '%2' WHERE `Argument` = '%3';")
               .arg(tbName).arg(value.toString()).arg(argName));
    sqlBug;

    lastUsedTable = tbName;
    return true;
}

bool iVConfDB::GetArgument(QVariant& receiver, const QString &tbName, const QString &argName) const
{
    if (!IsConfigTableExist(tbName))
    {
        bug QString("Config table %1 does not exist.").arg(tbName);
        return false;
    }
    tableModel->setTable(tbName);
    tableModel->setFilter(QString("Argument = '%1'").arg(argName));
    if (!tableModel->select())
    {
        bug QString("Argument %1 does not exist.").arg(argName);
    }
    if (tableModel->rowCount() == 0)
    {
        bug QString("Argument %1 has not been populated.").arg(argName);
        return false;
    }
    receiver = tableModel->record(0).value(2);

    return true;
}

bool iVConfDB::GetSingleConfArgument(QStringList& arg, QStringList& value, const QString &tbName, const QString &fileName) const
{
    if (!IsConfigTableExist(tbName))
    {
        bug QString("ERROR : config table %1 does not exist.").arg(tbName);
        return false;
    }
    if (!IsConfigFileExist(fileName))
    {
        bug QString("ERROR : config file %1 does not exist.").arg(fileName);
        return false;
    }
    tableModel->setTable(tbName);
    tableModel->setFilter(QString("location = '%1'").arg(fileName));
    tableModel->select();

    for (int i = 0; i < tableModel->rowCount(); i++)
    {
        arg << tableModel->record(i).value(1).toString();
        value << tableModel->record(i).value(2).toString();
    }

    return true;
}

bool iVConfDB::GetArgument(QString& receiver, const QString &tbName, const QString &argName) const
{
    if (!IsConfigTableExist(tbName))
    {
        bug QString("Config table %1 does not exist.").arg(tbName);
        return false;
    }
    tableModel->setTable(tbName);
    tableModel->setFilter(QString("Argument = '%1'").arg(argName));
    if (!tableModel->select())
    {
        bug QString("Argument %1 does not exist.").arg(argName);
    }
    if (tableModel->rowCount() == 0)
    {
        bug QString("Argument %1 has not been populated.").arg(argName);
        return false;
    }
    receiver = tableModel->record(0).value(2).toString();

    return true;
}

bool iVConfDB::GetConfigTable(const QString& tbName, QStringList &arg, QStringList &value) const
{
    if (!IsConfigTableExist(tbName))
    {
        bug QString("ERROR : configtable %1 does not exist.").arg(tbName);
        return false;
    }

    tableModel->setTable(tbName);
    tableModel->select();

    for (int i = 0; i < tableModel->rowCount(); i++)
    {
        arg << tableModel->record(i).value(1).toString();
        value << tableModel->record(i).value(2).toString();
    }

    return true;
}

bool iVConfDB::IsFirstUse() const
{
    tableModel->setTable("configlog");
    return !tableModel->select();
}

bool iVConfDB::IsConfigTableExist(const QString &tbName) const
{
    tableModel->setTable("configtablename");
    tableModel->setFilter(QString("configtable = '%1'").arg(tbName));
    tableModel->select();
    if (tableModel->rowCount() == 0)
    {
        return false;
    }

    return true;
}

bool iVConfDB::IsArgumentExist(const QString &ArgumentName) const
{
    tableModel->setTable("configargumentname");
    tableModel->setFilter(QString("argument = '%1'").arg(ArgumentName));
    tableModel->select();

    return tableModel->rowCount();
}

bool iVConfDB::IsLocationExist(const QString &location) const
{
    return IsConfigFileExist(location);
    /*
    tableModel->setTable("configfilename");
    tableModel->setFilter(QString("configration = '%1'").arg(location));
    tableModel->select();

    if (tableModel->rowCount() == 0)
    {
        return false;
    }

    return true;
    */
}

bool iVConfDB::IsConfigFileExist(const QString &confFile) const
{
    QSqlQuery query;
    query.exec(QString("SELECT COUNT(*) FROM configfilename WHERE configration = '%1'").arg(confFile));
    query.next();
    return query.value(0).toBool();
}

bool iVConfDB::IsImageLabeled(const QString &imageName)
{
    tableModel->setTable("SonoDataset");
    tableModel->setFilter(QString("imagename = '%1'").arg(imageName));
    tableModel->select();

    return tableModel->rowCount();
}

bool iVConfDB::GetLabel(const QString& imageName, QSqlRecord& receiver)
{
    tableModel->setTable("SonoDataset");
    tableModel->setFilter(QString("imagename = '%1'").arg(imageName));
    if (!tableModel->select())
    {
        bug QString("Argument %1 does not exist.").arg(imageName);
    }
    if (tableModel->rowCount() == 0)
    {
        bug QString("Argument %1 has not been populated.").arg(imageName);
        //return false;
    }
    receiver = tableModel->record(0);

    return true;

}

bool iVConfDB::AddLabel(const QString& imageName, const QVector<int>& coordiantes)
{
//    if (!IsConfigTableExist(tbName))
//    {
//        bug QString("ERROR : table %1 does not exist.").arg(tbName);
//        return false;
//    }
//    if (!IsLocationExist(location))
//    {
//        bug QString("ERROR : location %1 does not exist.").arg(location);
//        return false;
//    }

    //assert(0);
    //qDebug() << p1x << imageName;
    if (IsImageLabeled(imageName))
    {
        QSqlQuery query;

//        query.exec(QString("UPDATE `SonoDataset` SET `P1x` = '%1' WHERE `imagename` = '%2';")
//                    .arg(p1x).arg(imageName));
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
        qDebug() << query.prepare(QString("UPDATE `SonoDataset` SET P1x=?, P1y=?, P2x=?, P2y=?, P3x=?, P3y=?, P4x=?, P4y=?"
                              "WHERE `imagename` = '%1';").arg(imageName));
        for (int i = 0; i < 8; i++)
        {
            query.addBindValue(coordiantes[i]);
        }
        qDebug() << query.boundValues();
        qDebug() << query.isValid();
        query.exec();
        sqlBug;
    }
    else
    {
        QSqlQuery query;
        query.prepare(QString("INSERT INTO `SonoDataset` (imagename, P1x, P1y, P2x, P2y, P3x, P3y, P4x, P4y)"
                              "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"));
        query.addBindValue(imageName);
        //query.addBindValue(value.toString());
        for (int i = 0; i < 8; i++)
        {
            query.addBindValue(coordiantes[i]);
        }
        query.exec();
        sqlBug;
    }
    //qDebug() << "Show preblems here." << p1x << imageName;

    return true;
}

bool iVConfDB::AddTableNameRecord(const QString& tbName)
{
    tableModel->setTable("configtablename");
    tableModel->select();

    QSqlRecord record;
    record.append(QSqlField("configtable"));
    record.setValue(0, tbName);
    tableModel->insertRecord(-1, record);

    return true;
}

bool iVConfDB::DeleteTableNameRecord(const QString &tbName)
{
    tableModel->setTable("configtablename");
    tableModel->setFilter(QString("configtable = '%1'").arg(tbName));
    tableModel->select();

    if (tableModel->rowCount() == 0)
    {
        return false;
    }
    else
    {
        tableModel->removeRow(0);
        return true;
    }
}

bool iVConfDB::AddConfigFileRecord(const QString &fileName, const QString &description)
{
    tableModel->setTable("configfilename");
    tableModel->setFilter(QString("configration = '%1'").arg(fileName));
    tableModel->select();
    if (tableModel->rowCount() != 0)
    {
        tableModel->removeRow(0);
        //tableModel->submitAll();
    }

    QSqlRecord record;
    record.append(QSqlField("configration"));
    record.append(QSqlField("description"));
    record.setValue(0, fileName);
    record.setValue(1, description);

    tableModel->insertRecord(-1, record);
    return true;
}

bool iVConfDB::SubmitNewArgument(const QString &argName, const QString& location)
{
    tableModel->setTable("configargumentname");
    if (!tableModel->select())
    {
        bug "configargumentname does not exist.";
        return false;
    }
    QSqlRecord record;
    record.append(QSqlField("argument"));
    record.append(QSqlField("location"));
    record.setValue(0, argName);
    record.setValue(1, location);

    return tableModel->insertRecord(-1, record);
}

