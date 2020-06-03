
// author: Riccardo.Maria.Bianchi@cern.ch - 2017
// major updates: Aug 2018

#include <GeoModelDBManager/GMDBManager.h>

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QDebug>


#include <stdlib.h> /* exit, EXIT_FAILURE */


static std::string dbversion = "0.4.0"; // removed "parent" info from [Full]PhysVols tables (May 2020)


// FIXME: move this to utility class/file
std::vector<std::string> toStdVectorStrings(QStringList qlist)
{
  std::vector<std::string> vec;
  foreach(QString qstr, qlist) {
    vec.push_back(qstr.toStdString());
  }
  return vec;
}

// FIXME: should go to an utility class
std::string joinVectorStrings(std::vector<std::string> vec, std::string sep="") {
  std::string s;
  unsigned int ii = 0;
  for (const auto &piece : vec) {
    ++ii;
    if (ii == vec.size()) {
      s += (piece);
    } else {
      s += (piece + sep);
    }
  }
  return s;
}



GMDBManager::GMDBManager(const std::string &path) : m_dbpath(path), /*m_db(nullptr),*/ m_dbIsOK(false), m_deepDebug(false)
{
  // TODO: move to a more organic way
	#ifdef GEOREAD_DEEP_DEBUG
	  m_deepDebug = true;
 	#endif
  
  
  


  m_dbIsOK = false;

  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName( QString::fromStdString(path));

  if (!m_db.open())
  {
    std::cout << "Error: connection with database failed!\n";
    showError( m_db.lastError() );
    m_dbIsOK = false;
  }
  else
  {
    m_dbIsOK = true;
  }

  
//  // FIXME: TODO: we should check the existence of the file, otherwise SQLite will create a new file from scratch
//  // Save the connection result
//  int exit = 0;
//  exit = sqlite3_open(path.c_str(), &m_db);
//
//  // Test if there was an error
//  if (exit) {
//    std::cout << "DB Open Error: " << sqlite3_errmsg(m_db) << std::endl;
//    m_dbIsOK = false;
//
//  } else {
//    std::cout << "Opened Database Successfully!" << std::endl;
//    m_dbIsOK = true;
//  }
  


  // check if DB has tables, if not create them
  if (m_dbIsOK) {
    if ( ! (initDB()) ) {
      std::cout << "Error: database initialization failed" << std::endl;
      m_dbIsOK = false;
    }
  }

	// populate DB with fake data, only for debug
  //loadTestData(); // TODO: we should move initDB() here, only for debug and Write
}

GMDBManager::~GMDBManager()
{
  m_db.close();
//  m_db = nullptr;
}

bool GMDBManager::isOpen() const
{
	if (! m_dbIsOK )
		return false;
	return m_db.isOpen();
}


void GMDBManager::printAllMaterials() const
{
	printAllRecords("Materials");
}
void GMDBManager::printAllElements() const
{
	printAllRecords("Elements");
}

void GMDBManager::printAllShapes() const
{
	printAllRecords("Shapes");
}

void GMDBManager::printAllSerialDenominators() const
{
	printAllRecords("SerialDenominators");
}

void GMDBManager::printAllLogVols() const
{
	printAllRecords("LogVols");
}
void GMDBManager::printAllPhysVols() const
{
	printAllRecords("PhysVols");
}
void GMDBManager::printAllFullPhysVols() const
{
	printAllRecords("FullPhysVols");
}
void GMDBManager::printAllSerialTransformers() const
{
	printAllRecords("SerialTransformers");
}
void GMDBManager::printAllFunctions() const
{
	printAllRecords("Functions");
}
void GMDBManager::printAllTransforms() const
{
	printAllRecords("Transforms");
}
void GMDBManager::printAllAlignableTransforms() const
{
	printAllRecords("AlignableTransforms");
}
void GMDBManager::printAllNameTags() const
{
	printAllRecords("NameTags");
}
void GMDBManager::printAllChildrenPositions() const
{
	printAllRecords("ChildrenPositions");
}
void GMDBManager::printAllNodeTypes() const
{
	printAllRecords("GeoNodesTypes");
}
void GMDBManager::printRootVolumeId() const
{
	printAllRecords("RootVolume");
}
void GMDBManager::printDBVersion() const
{
	printAllRecords("dbversion");
}




void GMDBManager::printAllRecords(const std::string &tableName) const
{
	std::cout << tableName << " in db:" << std::endl;

	// QSqlQuery query("SELECT * FROM " + tableName);
  QSqlQuery query = selectAllFromTable(tableName);

	int nCols = (m_tableNames.at(tableName)).size();

	// print table column names
	std::cout << "- " <<  joinVectorStrings(m_tableNames.at(tableName), ", ") << std::endl;
	while (query.next())
	{
		std::cout << "* "; // TODO: move to a osstream: it's cleaner
		for( int ii=0; ii<nCols; ++ii)
		{
			if (! (ii==0) )
				std::cout << ", ";
			std::string valueStr = query.value(ii).toString().toStdString();
			// if (valueStr == "")
			// 	std::cout << "NULL"; // removed because we want to save the NULL to the DB, for consistency
			// else
				std::cout << valueStr;
		}
		std::cout << std::endl;
	}

	// TODO: I want to have a symbol like '---' to mean empty line when query gives 0 results.
	// but I guess query.size() is not the right method...
	//	qDebug() << "query size: " << query.size();
	//	if (query.size() == 0 || query.size() == -1)
	//		std::cout << "---" << std::endl;

}
//QHash<unsigned int, QStringList> GMDBManager::getTableFromNodeType(QString nodeType)
//{
//
//  QString tableName = getTableNameFromNodeType(nodeType);
//
//  QHash<unsigned int, QStringList> records;
//  QStringList nodeParams;
//
//  int nCols = (m_tableNames.at(tableName.toStdString())).size();
//
//  QSqlQuery query = selectAllFromTable(tableName.toStdString());
//  while (query.next()) {
//    nodeParams.clear();
//    unsigned int nodeId = query.value(0).toUInt();
//
//    for( int ii=0; ii<nCols; ++ii) {
//      nodeParams << query.value(ii).toString();
//    }
//    records[nodeId] = nodeParams;
//  }
//return records;
//}

std::vector<std::vector<std::string>> GMDBManager::getTableFromNodeType(std::string nodeType)
{

  std::string tableName = getTableNameFromNodeType(nodeType);

	// QHash<unsigned int, QStringList> records;
	// QStringList nodeParams;
  std::vector<std::vector<std::string>> records;
  std::vector<std::string> nodeParams;

	int nCols = (m_tableNames.at(tableName)).size();

	QSqlQuery query = selectAllFromTable(tableName); // sorted by ID
	while (query.next()) {
		nodeParams.clear();
//    unsigned int nodeId = query.value(0).toUInt();

		for( int ii=0; ii<nCols; ++ii) {
			// nodeParams << query.value(ii).toString();
			nodeParams.push_back( query.value(ii).toString().toStdString() );
		}
		// records[nodeId] = nodeParams;
    records.push_back(nodeParams);
	}
return records;
}


void GMDBManager::showError(const QSqlError &err) const
{
	qWarning() << "Unable to initialize Database" << "Error initializing database: " + err.text();
}

//QVariant GMDBManager::addPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume)
//{
//  if (m_deepDebug) std::cout << "GMDBManager::addPhysVol() - is root?" << isRootVolume << std::endl;
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into PhysVols(logvol, parent) values(?, ?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//
//  q.addBindValue(logVolId);
//  q.addBindValue(parentPhysVolId);
//  q.exec();
//
//  QVariant lastInserted = q.lastInsertId();
//
//  if (isRootVolume) {
//    storeRootVolume(lastInserted.toUInt(), "GeoPhysVol");
//  }
//
//  return lastInserted;
//
//}

//QVariant GMDBManager::addFullPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume)
//{
//  if (m_deepDebug) std::cout << "GMDBManager::addFullPhysVol() - is root?" << isRootVolume << std::endl;
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into FullPhysVols(logvol, parent) values(?, ?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//
//  q.addBindValue(logVolId);
//  q.addBindValue(parentPhysVolId);
//  q.exec();
//
//  QVariant lastInserted = q.lastInsertId();
//
//  if (isRootVolume) {
//    storeRootVolume(lastInserted.toUInt(), "GeoFullPhysVol");
//  }
//
//  return lastInserted;
//
//}



//QVariant GMDBManager::addLogVol(const QString &name, const QVariant &shapeId, const QVariant &materialId)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addLogVol()";
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into LogVols(name, shape, material) values(?, ?, ?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//
//  q.addBindValue(name);
//  q.addBindValue(shapeId);
//  q.addBindValue(materialId);
//  q.exec();
//  return q.lastInsertId();
//}

//QVariant GMDBManager::addMaterial(const QString &name, const QString &density, const QString &elements)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addMaterial()";
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into Materials(name, density, elements) values(?, ?, ?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//  q.addBindValue(name);
//  q.addBindValue(density);
//  q.addBindValue(elements);
//  q.exec();
//  return q.lastInsertId();
//}


//QVariant GMDBManager::addElement(const QString &name, const QString &symbol, const QString &elZ, const QString &elA)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addElement()";
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into Elements(name, symbol, Z, A) values(?, ?, ?, ?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//  q.addBindValue(name);
//  q.addBindValue(symbol);
//  q.addBindValue(elZ);
//  q.addBindValue(elA);
//  q.exec();
//  return q.lastInsertId();
//}

bool GMDBManager::addListOfChildrenPositions(const std::vector<std::vector<std::string>> &records)
{
    // NOTE: Choose the right function for your version of SQLite!!
	return addListOfRecordsToTable("ChildrenPositions", records); // newest SQLite versions
	//return addListOfRecordsToTableOld("ChildrenPositions", records); // old SQLite versions
}

//bool GMDBManager::addListOfRecords(const QString geoType, const std::vector<QStringList> records)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addListOfRecords():" << geoType;
//
//  QString tableName = QString::fromStdString(m_childType_tableName[geoType.toStdString()]);
//
//  if (tableName.isEmpty()) {
//        //qWarning() << "m_childType_tableName:" << m_childType_tableName;
//    qWarning() << "ERROR!! could not retrieve tableName for node type " << geoType << "!! Aborting...";
//    exit(1);
//  }
//
//  if (records.size() > 0 ) {
//        // NOTE: Choose the right function!!
//    return addListOfRecordsToTable(tableName, records); // newest SQLite versions
//    //return addListOfRecordsToTableOld(tableName, records); // old SQLite versions
//  }
//  else
//    qWarning() << "Info: no records to save for geoType '" << geoType << "'. Skipping...";
//  return true;
//}


bool GMDBManager::addListOfRecords(const std::string geoType, const std::vector<std::vector<std::string>> records)
{
//  if (m_deepDebug) qDebug() << "GMDBManager::addListOfRecords():" << geoType;
  
  std::string tableName = m_childType_tableName[geoType];
  
  if (tableName.size() == 0) {
    //qWarning() << "m_childType_tableName:" << m_childType_tableName;
    std::cout << "ERROR!! could not retrieve tableName for node type '" << geoType << "'!! Aborting...";
    exit(EXIT_FAILURE);
  }
  
  if (records.size() > 0 ) {
    // NOTE: Choose the right function!!
    return addListOfRecordsToTable(tableName, records); // better, but needs SQLite >= 3.7.11
    //return addListOfRecordsToTableOld(tableName, records); // old SQLite versions
  }
  else
  std::cout << "Info: no records to save for geoType '" << geoType << "'. Skipping..." << std::endl;
  return true;
}




// ***Note***
// the syntax built here below is more convenient
// but it is supported only in SQLite >= 3.7.11
// ...but SLC6 only has 3.7.9!
// here we build a query like this:
// queryStr = QString("INSERT INTO Materials (id, name) VALUES  (1,'Air'), (2,'Silicon'), (368,'ShieldSteel');");
//
//bool GMDBManager::addListOfRecordsToTable(const QString tableName, const std::vector<QStringList> records)
//{
//  // get table columns and format them for query
//  QString tableColString = "(" + QString::fromStdString(joinVectorStrings(m_tableNames.at(tableName.toStdString()), ", ")) + ")";
//
//    unsigned int nRecords = records.size();
//    qInfo() << "number of " << tableName << "records to insert into the DB:" << nRecords;
//
//  // preparing the SQL query
//  QString queryStr("INSERT INTO %1 %2 VALUES ");
//  queryStr = queryStr.arg(tableName); // insert table name
//  queryStr = queryStr.arg(tableColString); // insert table columns
//
//  unsigned int nMat = nRecords;
//  unsigned int id = 0;
//  foreach(QStringList rec, records) {
//        //qDebug() << "rec:" << rec;
//
//    ++id;
//    QStringList items;
//
//    foreach (QString item, rec) {
//      items << '"' + item + '"';
//    }
//    QString values = items.join(",");
//    queryStr += " (" + QString::number(id) + "," + values + ")";
//    if (id != nMat)
//      queryStr += ",";
//    else
//      queryStr += ";";
//
//        }
//
//  // executing the SQL query
//  QSqlQuery q;
//  if (!q.exec(queryStr)) {
//        qWarning() << "ERROR!!! SQL error:";
//    showError(q.lastError());
//    return false;
//  }
//
//  return true;
//}


// ***Note***
// the syntax built here below is more convenient
// but it is supported only in SQLite >= 3.7.11
// ...but SLC6 only has 3.7.9!
// here we build a query like this:
// queryStr = QString("INSERT INTO Materials (id, name) VALUES  (1,'Air'), (2,'Silicon'), (368,'ShieldSteel');");
//
bool GMDBManager::addListOfRecordsToTable(const std::string tableName, const std::vector<std::vector<std::string>> records)
//bool addListOfRecordsToTable(const std::string tableName, const std::vector<std::vector<std::string>> records)
{
  // get table columns and format them for query
  QString tableColString = "(" + QString::fromStdString(joinVectorStrings(m_tableNames.at(tableName), ", ")) + ")";
  
  qWarning() << "tableColString:" << tableColString;
  
  unsigned int nRecords = records.size();
  std::cout << "number of " << tableName << " records to insert into the DB:" << nRecords << std::endl;
  
  // preparing the SQL query
  QString queryStr("INSERT INTO %1 %2 VALUES ");
  queryStr = queryStr.arg(QString::fromStdString(tableName)); // insert table name
  queryStr = queryStr.arg(tableColString); // insert table columns
  
  unsigned int nMat = nRecords;
  unsigned int id = 0;
//  foreach(QStringList rec, records) {
  for( const std::vector<std::string>& rec : records) {
    //qDebug() << "rec:" << rec;
    
    ++id;
//    QStringList items;
    std::vector<std::string> items;
    
    for ( const std::string& item : rec) {
      items.push_back('"' + item + '"');
    }
//    QString values = items.join(",");
    std::string values = joinVectorStrings(items, ",");
    queryStr += " (" + QString::number(id) + "," + QString::fromStdString(values) + ")";
    if (id != nMat)
    queryStr += ",";
    else
    queryStr += ";";
    
  }
  
  qWarning() << "Query string:" << queryStr;
  
  // executing the SQL query
  QSqlQuery q;
  if (!q.exec(queryStr)) {
    qWarning() << "ERROR!!! SQL error:";
    showError(q.lastError());
    return false;
  }
  
  return true;
}


//bool GMDBManager::addListOfRecordsToTable(const std::string tableName, const std::vector<std::vector<std::string>> records)
//{
//  // get table columns and format them for query
//  std::string tableColString = "(" + joinVectorStrings(m_tableNames.at(tableName), ", ") + ")";
//
//  std::cout << "tableColString:" << tableColString << std::endl;
//
//  unsigned int nRecords = records.size();
//  std::cout << "number of " << tableName << " records to insert into the DB:" << nRecords << std::endl;
//
//  // preparing the SQL query
//  QString queryStr("INSERT INTO %1 %2 VALUES ");
//  queryStr = queryStr.arg(QString::fromStdString(tableName)); // insert table name
//  queryStr = queryStr.arg(tableColString); // insert table columns
//
//}


// ***Note***
// old syntax, for SQLite in SLC6
// here below we build the syntax for multiple INSERT
// compatible with old SQLite versions
// see: stackoverflow.com/questions/1609637
//
// we want to build a query like this:
//  queryStr = QString("INSERT INTO Materials (id, name) SELECT 1 as id, 'Air' as name UNION ALL SELECT 2,'Silicon' UNION ALL SELECT 368,'ShieldSteel' ")
//
/*
bool GMDBManager::addListOfRecordsToTableOld(const QString tableName, const std::vector<QStringList> records)
{
	// get table columns and format them for query
  std::string tabColNames = joinVectorStrings(m_tableNames.at(tableName.toStdString()), ", ");
  QString tableColString = "(" + QString::fromStdString(tabColNames) + ")";



    QStringList colNames = m_tableNames.at(tableName);
    unsigned int nRecords = records.size();
    qInfo() << "number of " << tableName << "records to insert into the DB:" << nRecords;

 
     // SQLite has a limit on 'union' items, set at 500. So we have to split the items if we have more.
     // See: stackoverflow.com/questions/9527851/
 
    unsigned int bunchSize = 500;
    if ( nRecords > bunchSize ) {
        qWarning() << "WARNING! " << nRecords << " records of type " << tableName << "to store in the DB in one call! Call limit is " << bunchSize << " --> We split them in bunches...";

        std::vector<QStringList> recordsCopy( records ); // TODO: maybe we should use a deque or queue, which have fast pop of first element?

        std::vector<QStringList>::const_iterator first;
        std::vector<QStringList>::const_iterator last;

        unsigned int start = 0;

        while ( recordsCopy.size() > 0 ) {

        // preparing the SQL query
        QString queryStr("INSERT INTO %1 %2 SELECT ");
	    queryStr = queryStr.arg(tableName); // insert table name
	    queryStr = queryStr.arg(tableColString); // insert table columns

        first = recordsCopy.begin();
        last  = recordsCopy.size() > bunchSize ? recordsCopy.begin() + bunchSize : recordsCopy.end();

        std::vector<QStringList> recordsBunch( first, last ); // we take the first 500 records
        recordsCopy.erase( first, last ); // we delete the first 500 records
        if (m_deepDebug) qDebug() << "start:" << start << "recordsBunch size:" << recordsBunch.size() << "- recordsCopy size after removal:" << recordsCopy.size();

	    // --- first record
        // outcome should be like: " 1 as id, 'Air' as name "
        unsigned int id = start+1; // set the first ID as 'start'
        QStringList recFirst = recordsBunch.front(); // access first record from vector
        recordsBunch.erase( recordsBunch.begin() ); // delete first record
        if (m_deepDebug) qDebug() << "after taking first record - recordsBunch size:" << recordsBunch.size();

        // first item in the first record (it is the ID)
        QString firstCol = colNames[0];
        queryStr += QString::number(id) + " as " + firstCol + ", ";

        // the other items in the first record
        unsigned int ii = 1;
        unsigned int nRecs = recFirst.size();
        foreach (QString rec, recFirst) {
            queryStr += "'" + rec + "'" + " as " + colNames[ii];
            if (ii != nRecs)
                queryStr += ","; // add coma, but not on latest
            queryStr += " "; // add space
            //qDebug() << "first element:" << ii << nRecs << queryStr;
            ++ii;
        }
        if (m_deepDebug) qDebug() << "first element query:" << queryStr;
        // --- other records
        // outcome should be: " UNION ALL SELECT 2,'Silicon' "


            foreach(QStringList recs, recordsBunch) {

							// DEBUG
							if (tableName == "Functions") {
								if (recs[1].length() > 65000) {
									std::cout << "LONG STRING! size: " << recs[1].length() << std::endl;
									std::cout << "LONG STRING! string: " << recs[1].toStdString() << std::endl << std::endl;
								}
							}
							//------

								++id;
                // put single quotes around items
                QStringList items;
                foreach (QString item, recs) {
			        		items << "'" + item + "'";
		        		}
                // join items into a string and add the SQL commands
                QString itemsStr = items.join(", ");
                queryStr += " UNION ALL SELECT " + QString::number(id) + ", " + itemsStr;
            }

        //qDebug() << "queryStr:" << queryStr;

	    // executing the SQL query
	    QSqlQuery q;
	    if (!q.exec(queryStr)) {
            qWarning() << "ERROR!!! SQL error:";
		    showError(q.lastError());
		    return false;
	    }
        // JFB commented: qDebug() << bunchSize << "elements have been saved into the DB, starting at:" << start;

        start += bunchSize; // for the next iteration

      } // end of while

    } // end of if(>500)
    else {
    // preparing the SQL query
    QString queryStr("INSERT INTO %1 %2 SELECT ");
	queryStr = queryStr.arg(tableName); // insert table name
	queryStr = queryStr.arg(tableColString); // insert table columns

    unsigned int id = 0;
    // loop on all records
	foreach(QStringList recs, records) {

		++id;

        // first record
        // outcome should be like: " 1 as id, 'Air' as name "
        if (id == 1) {

            // first item if the record (it is the ID)
            QString firstCol = colNames.takeFirst();
            queryStr += QString::number(id) + " as " + firstCol + ", ";

            // the other items in the record
            unsigned int ii = 0;
            unsigned int nRecs = recs.size();
            foreach (QString rec, recs) {
                queryStr += "'" + rec + "'" + " as " + colNames[ii];
                if (ii != nRecs-1)
                    queryStr += ","; // add coma, but not on latest
                queryStr += " "; // add space
                //qDebug() << ii << nRecs << queryStr;
                ++ii;
            }

         }
        // other records
        // outcome should be: " UNION ALL SELECT 2,'Silicon' "
        else {

            // put single quotes around items
            QStringList items;
            foreach (QString item, recs) {
			    items << "'" + item + "'";
		    }
            // join items into a string and add the SQL commands
            QString itemsStr = items.join(", ");
            queryStr += " UNION ALL SELECT " + QString::number(id) + ", " + itemsStr;
        }

	} // end of: foreach(QStringList recs, records)

    //qDebug() << "queryStr:" << queryStr;

	// executing the SQL query
	QSqlQuery q;
	if (!q.exec(queryStr)) {
        qWarning() << "ERROR!!! SQL error:";
		showError(q.lastError());
		return false;
	}
} // end of (else > 500)

	// JFB commented: qDebug() << "DONE. The list of " << nRecords << "records have been inserted into the DB.";
	return true;

}
*/





//QVariant GMDBManager::addShape(const QString &type, const QString &parameters)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addShape()";
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into Shapes(type, parameters) values(?, ?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//
//  q.addBindValue(type);
//  q.addBindValue(parameters);
//  q.exec();
//  return q.lastInsertId();
//}
//
//QVariant GMDBManager::addSerialDenominator(const QString &baseName)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addSerialDenominator("+baseName+")";
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into SerialDenominators(baseName) values(?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//
//  q.addBindValue(baseName);
//  q.exec();
//  return q.lastInsertId();
//}

//QVariant GMDBManager::addNameTag(const QString &name)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addNameTag("+name+")";
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into NameTags(name) values(?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//  q.addBindValue(name);
//  q.exec();
//  return q.lastInsertId();
//}
//
//QVariant GMDBManager::addFunction(const QString expression)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addFunction()";
//
//  // TEST
//  std::cout << "Function - expression string len: " << expression.length();
//  std::cout << "Function - expression: " << expression.toStdString() << std::endl << std::endl;
//  //-----
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into Functions(expression) values(?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//
//  q.addBindValue(expression);
//  q.exec();
//  return q.lastInsertId();
//
//
//}

//QVariant GMDBManager::addSerialTransformer(const unsigned int &funcId, const unsigned int &physvolId, const std::string &physvolType, const unsigned int &copies)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addSerialTransformer()" << funcId << physvolId << copies;
//
//  const unsigned int volTableId = getTableIdFromNodeType(physvolType);
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into SerialTransformers(funcId, volId, volTable, copies) values(?, ?, ?, ?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//
//  q.addBindValue(funcId);
//  q.addBindValue(physvolId);
//  q.addBindValue(volTableId);
//  q.addBindValue(copies);
//  q.exec();
//  return q.lastInsertId();
//}

//QVariant GMDBManager::addTransform(QVector<double> params)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addTransform()";
//
//  // get the 12 matrix elements
//  double xx = params[0];
//  double xy = params[1];
//  double xz = params[2];
//
//  double yx = params[3];
//  double yy = params[4];
//  double yz = params[5];
//
//  double zx = params[6];
//  double zy = params[7];
//  double zz = params[8];
//
//  double dx = params[9];
//  double dy = params[10];
//  double dz = params[11];
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into Transforms(xx, xy, xz, yx, yy, yz, zx, zy, zz, dx, dy, dz) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//
//  q.addBindValue(xx);
//  q.addBindValue(xy);
//  q.addBindValue(xz);
//  q.addBindValue(yx);
//  q.addBindValue(yy);
//  q.addBindValue(yz);
//  q.addBindValue(zx);
//  q.addBindValue(zy);
//  q.addBindValue(zz);
//  q.addBindValue(dx);
//  q.addBindValue(dy);
//  q.addBindValue(dz);
//  q.exec();
//  return q.lastInsertId();
//}
//
//
//QVariant GMDBManager::addAlignableTransform(QVector<double> params)
//{
////  if (m_deepDebug) qDebug() << "GMDBManager::addAlignableTransform()";
//
//  // get the 12 matrix elements
//  double xx = params[0];
//  double xy = params[1];
//  double xz = params[2];
//
//  double yx = params[3];
//  double yy = params[4];
//  double yz = params[5];
//
//  double zx = params[6];
//  double zy = params[7];
//  double zz = params[8];
//
//  double dx = params[9];
//  double dy = params[10];
//  double dz = params[11];
//
//  QSqlQuery q;
//  if (!q.prepare(QLatin1String("insert into AlignableTransforms(xx, xy, xz, yx, yy, yz, zx, zy, zz, dx, dy, dz) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))) {
//    showError(q.lastError());
//    return QVariant();
//  }
//
//  q.addBindValue(xx);
//  q.addBindValue(xy);
//  q.addBindValue(xz);
//  q.addBindValue(yx);
//  q.addBindValue(yy);
//  q.addBindValue(yz);
//  q.addBindValue(zx);
//  q.addBindValue(zy);
//  q.addBindValue(zz);
//  q.addBindValue(dx);
//  q.addBindValue(dy);
//  q.addBindValue(dz);
//  q.exec();
//  return q.lastInsertId();
//}


bool GMDBManager::addRootVolume(const std::vector<std::string> &values)
{
	if (values.size() > 0 ) {
    const unsigned int volId = std::stoi(values[0]);
    const std::string nodeType = values[1];
		return storeRootVolume( volId, nodeType );
	}
	else
  std::cout << "WARNING! No records to save for RootVolume!" << std::endl;
	return false;
}

// insert a single parent-child relationship for a given parent volume
void GMDBManager::addChildPosition(const unsigned int &parentId, const std::string &parentType, const unsigned int &childId, const unsigned int &parentCopyNumber, const unsigned int &childPos, const std::string &childType, const unsigned int &childCopyN)
{
//  if (m_deepDebug) qDebug() << "GMDBManager::addChildPosition(" << parentId << "," << parentType << "," << QString::number(parentCopyNumber) << "," <<childPos << "," << childType << ", " << childId << ", " << QString::number(childCopyN) << ")";

	// get table name -- which the child ID refers to -- based on the child type
	const unsigned int parentTableId = getTableIdFromNodeType(parentType);
	const unsigned int childTableId = getTableIdFromNodeType(childType);

  std::string tableName = "ChildrenPositions";
  std::vector<std::string> cols = getTableColumnNames(tableName);

	QString queryStr = QString("insert into %1(%2, %3, %4, %5, %6, %7, %8) values(?, ?, ?, ?, ?, ?, ?)").arg( QString::fromStdString(tableName) );
	for (int i=0; i < cols.size(); ++i) {
		if (i != 0) // skip the "id" column
			queryStr = queryStr.arg( QString::fromStdString( cols.at(i)) );
	}
//  if (m_deepDebug) qDebug() << "queryStr:" << queryStr;
	QSqlQuery q;
	if (!q.prepare(queryStr)) {
		showError(q.lastError());
		return;
	}
	q.addBindValue(parentId);
	q.addBindValue(parentTableId);
	q.addBindValue(parentCopyNumber);
	q.addBindValue(childPos);
	q.addBindValue(childTableId);
	q.addBindValue(childId);
	q.addBindValue(childCopyN);
	q.exec();
	return;
}

void GMDBManager::addDBversion(const QString version)
{
	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into dbversion(version) values(?)"))) {
		showError(q.lastError());
		return;
	}
	q.addBindValue(version);
	q.exec();
	return;
}


std::vector<std::string> GMDBManager::getItem(std::string geoType, unsigned int id)
{
//  if (m_deepDebug) qDebug() << "GMDBManager::getItem(geoType, id)"<< geoType << QString::number(id);

	/* Get the right DB table */
  std::string tableName = getTableNameFromNodeType(geoType);
	return getItemFromTableName(tableName, id);
}


std::vector<std::string> GMDBManager::getItem(unsigned int tableId, unsigned int id)
{
//  if (m_deepDebug) qDebug() << "GMDBManager::getItem(tableId, id)"<< QString::number(tableId) << QString::number(id);
	/* Get the right DB table */
  std::string tableName = getTableNameFromTableId(tableId);
	return getItemFromTableName(tableName, id);

}

/*
QStringList GMDBManager::getItemAndType(unsigned int tableId, unsigned int id)
{
	if (m_deepDebug) qDebug() << "GMDBManager::getItemAndType(tableId, id)"<< QString::number(tableId) << QString::number(id);
	// Get the right DB table
  std::string tableName = getTableNameFromTableId(tableId);

  std::string nodeType = getNodeTypeFromTableId(tableId);

	QStringList results;
  std::vector<std::string> item =  getItemFromTableName(tableName, id);

	results << nodeType << item;

	return results;

}
*/
std::vector<std::string> GMDBManager::getItemAndType(unsigned int tableId, unsigned int id)
{
  std::vector<std::string> results;
  
  std::string tableName = getTableNameFromTableId(tableId);
  std::string nodeType  = getNodeTypeFromTableId(tableId);
  std::vector<std::string> item  =  getItemFromTableName(tableName, id);

  // add the type
  results.push_back( nodeType );
  // add the item
  results.insert(results.end(), item.begin(), item.end());
  
  return results;
  
}

/*
QStringList GMDBManager::getItemFromTableName(std::string tableName, unsigned int id)
{
	if (m_deepDebug) qDebug() << "GMDBManager::getItemFromTableName(tableName, id)"<< tableName << QString::number(id);
 
	 // 2. Get the object from DB
 
	// prepare a SQL string with the right table name
	QString queryStr = QString("SELECT * FROM %1 WHERE id = (?)").arg(tableName);
	// prepare the query
	QSqlQuery q;
	if (!q.prepare( queryStr )) {
		showError(q.lastError());
		return QStringList();
	}
	q.addBindValue(id);
	q.exec();

	QStringList params;

	// get the number of columns of the DB table
	int nCols = (m_tableNames.at(tableName.toStdString())).size();

	while (q.next()) {

		for( int ii=0; ii<nCols; ++ii)
			params << q.value(ii).toString();
	}
	if (params.length()==0) {
		qWarning() << "WARNING!!" << "Item" << id << "does not exist in table" << tableName << "!!";
	}
	return params;
}
    */

std::vector<std::string> GMDBManager::getItemFromTableName(std::string tableName, unsigned int id)
{
//  if (m_deepDebug) qDebug() << "GMDBManager::getItemFromTableName(tableName, id)"<< tableName << QString::number(id);
  /*
   * 2. Get the object from DB
   */
  // prepare a SQL string with the right table name
  QString queryStr = QString("SELECT * FROM %1 WHERE id = (?)").arg(QString::fromStdString(tableName));
  // prepare the query
  QSqlQuery q;
  if (!q.prepare( queryStr )) {
    showError(q.lastError());
//    return QStringList();
    exit(EXIT_FAILURE);
  }
  q.addBindValue(id);
  q.exec();
  
  std::vector<std::string> params;
  
  // get the number of columns of the DB table
  int nCols = (m_tableNames.at(tableName)).size();
  
  while (q.next()) {
    
    for( int ii=0; ii<nCols; ++ii)
      params.push_back( q.value(ii).toString().toStdString() );
  }
  if (params.size()==0) {
    std::cout << "WARNING!!" << "Item" << id << "does not exist in table" << tableName << "!!";
  }
  return params;
}


// get the list of children for a single GeoVPhysVol (i.e., GeoPhysVol or GeoFullPhysVol)
QMap<unsigned int, QStringList> GMDBManager::getVPhysVolChildren(const unsigned int &id, const std::string &nodeType, const unsigned int &parentCopyNumber)
{
  if (m_deepDebug) std::cout << "GMDBManager::getVPhysVolChildren() - id:" << id << "- type:" << nodeType << "- copyN:" << parentCopyNumber;

	const unsigned int tableId = getTableIdFromNodeType(nodeType);

	// get children and their positions
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT * FROM ChildrenPositions WHERE parentId = (?) AND parentTable = (?) AND parentCopyNumber = (?)"))) {
		showError(q.lastError());
		return QMap<unsigned int, QStringList>();
	}
	q.addBindValue(id);
	q.addBindValue(tableId);
	q.addBindValue(parentCopyNumber);
	q.exec();

	QMap<unsigned int, QStringList> children;
	QStringList childParams;

	// get the number of columns of the DB table
	int nCols = m_tableNames["ChildrenPositions"].size();

	while (q.next()) {

		childParams.clear();

		unsigned int childPos = q.value(3).toUInt();

		for( int ii=0; ii<nCols; ++ii)
			childParams << q.value(ii).toString();

		children[childPos] = childParams;
	}

	return children;
}


// Get all parent-children data from the database in one go
QHash<QString, QMap<unsigned int, QStringList>> GMDBManager::getChildrenTable()
{
	QSqlQuery q = selectAllFromTable("ChildrenPositions");

	QHash< QString, QMap<unsigned int, QStringList> > all_children; // to store all children
	// QMap<unsigned int, QStringList> children; // to temporarily store the children of one parent
	QStringList childParams; // to temporarily store the children parameters

	// get the number of columns of the DB table
	int nCols = m_tableNames["ChildrenPositions"].size();

    // loop over all children's positions stored in the DB
	while (q.next()) {

		childParams.clear();

		QString parentId = q.value(1).toString();
		QString parentTable = q.value(2).toString();
		QString parentCopyNumber = q.value(3).toString();
		unsigned int childPos = q.value(4).toUInt();

		QString key = parentId + ":" + parentTable + ":" + parentCopyNumber;

    for( int ii=0; ii<nCols; ++ii) {
			childParams << q.value(ii).toString();
    }

		all_children[key][childPos] = childParams;
	}
	return all_children;
}

// Get all parent-children data from the database in one go
std::vector<std::vector<std::string>> GMDBManager::getChildrenTableStd()
{
//  std::cout << "GMDBManager::getChildrenTableStd()\n"; // debug
  
  QSqlQuery q;
  QString queryStr = QString("SELECT * FROM ChildrenPositions ORDER BY parentTable, parentId, parentCopyNumber, position");
  if (!q.prepare(queryStr)) {
    showError(q.lastError());
  }
  q.exec();
  
  std::vector<std::vector<std::string>> all_children; // to store all children
  std::vector<std::string> childParams; // to temporarily store the children parameters
  
  // get the number of columns of the DB table
  int nCols = m_tableNames["ChildrenPositions"].size();
  //  std::cout << "num of columns in childrenPos table" << nCols << std::endl; // debug
  
  // loop over all children's positions stored in the DB
  while (q.next()) {
    
    childParams.clear();
    
    for( int ii=0; ii<nCols; ++ii) {
      childParams.push_back( q.value(ii).toString().toStdString() );
    }
    
//    // debug
//    if(childParams[2]=="8920")
//      std::cout << "CoolingTube parent:"; print childParams;
    
    all_children.push_back(childParams);
  }
  return all_children;
}



unsigned int GMDBManager::getTableIdFromNodeType(const std::string &nodeType)
{
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT id FROM GeoNodesTypes WHERE nodeType = (?)"))) {
		showError(q.lastError());
    exit(EXIT_FAILURE);
	}
  q.addBindValue(QString::fromStdString(nodeType));
	q.exec();

	QVariant id;
	while (q.next()) {
		id = q.value(0);
	}

	return id.toUInt();
}

QString GMDBManager::getTableNameFromNodeType(QString nodeType)
{
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT tableName FROM GeoNodesTypes WHERE nodeType = (?)"))) {
		showError(q.lastError());
		return QString();
	}
	q.addBindValue(nodeType);
	q.exec();

	QString tableName;
	while (q.next()) {
		tableName = q.value(0).toString();
	}

	return tableName;
}

      std::string GMDBManager::getTableNameFromNodeType(std::string nodeType)
      {
        QSqlQuery q;
        if (!q.prepare(QLatin1String("SELECT tableName FROM GeoNodesTypes WHERE nodeType = (?)"))) {
          showError(q.lastError());
//          return QString();
          exit(EXIT_FAILURE);
        }
        q.addBindValue(QString::fromStdString(nodeType));
        q.exec();
        
        std::string tableName;
        while (q.next()) {
          tableName = q.value(0).toString().toStdString();
        }
        
        return tableName;
      }
      
      

// TODO: this and other methods could take data from in-memory maps, without asking to the DB all the times
/*
QString GMDBManager::getTableNameFromTableId(unsigned int tabId)
{
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT tableName FROM GeoNodesTypes WHERE id = (?)"))) {
		showError(q.lastError());
		return QString();
	}
	q.addBindValue(tabId);
	q.exec();

	QString tableName;
	while (q.next()) {
		tableName = q.value(0).toString();
	}

	return tableName;
}
 */


std::string GMDBManager::getTableNameFromTableId(unsigned int tabId)
{
  QSqlQuery q;
  if (!q.prepare(QLatin1String("SELECT tableName FROM GeoNodesTypes WHERE id = (?)"))) {
    showError(q.lastError());
//    return QString();
    exit(EXIT_FAILURE);
  }
  q.addBindValue(tabId);
  q.exec();
  
  std::string tableName;
  while (q.next()) {
    tableName = q.value(0).toString().toStdString();
  }
  
  return tableName;
}
/*
QString GMDBManager::getNodeTypeFromTableId(unsigned int tabId)
{
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT nodeType FROM GeoNodesTypes WHERE id = (?)"))) {
		showError(q.lastError());
		return QString();
	}
	q.addBindValue(tabId);
	q.exec();

	QString nodeType;
	while (q.next()) {
		nodeType = q.value(0).toString();
	}

	return nodeType;
}
 */
std::string GMDBManager::getNodeTypeFromTableId(unsigned int tabId)
{
  QSqlQuery q;
  if (!q.prepare(QLatin1String("SELECT nodeType FROM GeoNodesTypes WHERE id = (?)"))) {
    showError(q.lastError());
    // return QString();
    exit(EXIT_FAILURE);
  }
  q.addBindValue(tabId);
  q.exec();
  
  std::string nodeType;
  while (q.next()) {
    nodeType = q.value(0).toString().toStdString();
  }
  
  return nodeType;
}
/*
QHash<unsigned int, QString> GMDBManager::getAll_TableIDsNodeTypes()
{

	QHash<unsigned int, QString> output;

	QSqlQuery q = selectAllFromTable("GeoNodesTypes");

	unsigned int id;
	QString nodeType;
	// QString tableName;
	while (q.next()) {
		id = q.value(0).toUInt();
		nodeType = q.value(1).toString();
		output[id] = nodeType;
	}
	return output;
}
*/
std::unordered_map<unsigned int, std::string> GMDBManager::getAll_TableIDsNodeTypes()
{
  std::unordered_map<unsigned int, std::string> output;
  QSqlQuery q = selectAllFromTable("GeoNodesTypes");
  
  unsigned int id;
  QString nodeType;
  // QString tableName;
  while (q.next()) {
    id = q.value(0).toUInt();
    nodeType = q.value(1).toString();
    output[id] = nodeType.toStdString();
  }
  return output;
}

/*
QHash<QString, unsigned int> GMDBManager::getAll_NodeTypesTableIDs()
{
	QHash<QString, unsigned int> output;

	QSqlQuery q = selectAllFromTable("GeoNodesTypes");

	unsigned int id;
	QString nodeType;
	// QString tableName;
	while (q.next()) {
		id = q.value(0).toUInt();
		nodeType = q.value(1).toString();
		output[nodeType] = id;
	}
	return output;
}
*/

std::unordered_map<std::string, unsigned int> GMDBManager::getAll_NodeTypesTableIDs()
{
  std::unordered_map<std::string, unsigned int> output;
  QSqlQuery q = selectAllFromTable("GeoNodesTypes");
  
  unsigned int id;
  QString nodeType;
  // QString tableName;
  while (q.next()) {
    id = q.value(0).toUInt();
    nodeType = q.value(1).toString();
    output[nodeType.toStdString()] = id;
  }
  return output;
}


//QSqlQuery GMDBManager::selectAllFromTable(QString tableName) const
//{
////  QSqlQuery q;
////
////  QString queryStr = QString("SELECT * FROM %1 ORDER BY id");
////  queryStr = queryStr.arg(tableName);
////
////  if (!q.prepare(queryStr)) {
////    showError(q.lastError());
////    return QSqlQuery();
////  }
////  q.exec();
////  return q;
//
//  return selectAllFromTableSortBy(tableName.toStdString(), "id");
//}

QSqlQuery GMDBManager::selectAllFromTable(std::string tableName) const
{
  return selectAllFromTableSortBy(tableName, "id");
}

      
      
      QSqlQuery GMDBManager::selectAllFromTableSortBy(std::string tableName, std::string sortColumn) const
{
  QSqlQuery q;
  
  if ("" == sortColumn || 0 == sortColumn.size()) {
    sortColumn = "id";
  }
  QString qsortColumn = QString::fromStdString(sortColumn);
  
  QStringList args;
  args << QString::fromStdString(tableName) << qsortColumn;
  
  QString queryStr = QString("SELECT * FROM %1 ORDER BY %2");
//  queryStr = queryStr.arg(tableName);
  for (int i=0; i < args.size(); ++i) {
    queryStr = queryStr.arg( args.at(i) );
  }
  
  if (!q.prepare(queryStr)) {
    showError(q.lastError());
    return QSqlQuery();
  }
  q.exec();
  return q;
}



bool GMDBManager::initDB()
{
	// check if DB is empty
	QSqlDatabase db = QSqlDatabase::database();
	QStringList tables = db.tables();
	if (   tables.contains("LogVols",   Qt::CaseInsensitive)
			|| tables.contains("PhysVols",  Qt::CaseInsensitive)
			|| tables.contains("Materials", Qt::CaseInsensitive)
			|| tables.contains("Elements",  Qt::CaseInsensitive)
			|| tables.contains("Shapes",    Qt::CaseInsensitive)
	) {
		loadTableNamesFromDB();
		return true;
	}

  // TODO: we should check if all needed tables are present; if not throw an error message and exit.

  // TODO: we should create tables only if the DB is really completely empty!
	// if DB is empty, then create tables
	bool tablesOK = createTables();

	// store DB version
	addDBversion(QString::fromStdString(dbversion));

	return tablesOK;

}

bool GMDBManager::createTables()
{
//  QStringList tab;
	QSqlQuery q;
  QString queryStr;
  
  std::string geoNode;
  std::string tableName;
  std::vector<std::string> tab;
//  std::vector<std::string>::iterator it;
  
  // create a table to store the relation between the types of GeoNodes and the name of the table
	tableName = "dbversion";
//  tab << tableName << "id" << "version";
  tab.push_back(tableName); // TODO: find a nicer and faster way compared to this chain of push_back()...
  tab.push_back("id");
  tab.push_back("version" );
	queryStr = QString("create table %1(%2 integer primary key, %3 integer)");
	for (int i=0; i < tab.size(); ++i) {
    queryStr = queryStr.arg( QString::fromStdString(tab.at(i)) );
	}
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	}
	storeTableColumnNames(tab);
	tab.clear();

	// create a table to store the relation between the types of GeoNodes and the name of the table
	tableName = "GeoNodesTypes";
//  tab << tableName << "id" << "nodeType" << "tableName";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("nodeType");
  tab.push_back("tableName");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table GeoNodesTypes(id integer primary key, nodeType varchar, tableName varchar)"))) {
		showError(q.lastError());
		return false;
	}

	// ChildrenPositions table
	tableName = "ChildrenPositions";
//  tab << tableName << "id" << "parentId" << "parentTable" << "parentCopyNumber" << "position" << "childTable" << "childId" << "childCopyNumber";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("parentId");
  tab.push_back("parentTable");
  tab.push_back("parentCopyNumber");
  tab.push_back("position");
  tab.push_back("childTable");
  tab.push_back("childId");
  tab.push_back("childCopyNumber");
	queryStr = QString("create table %1(%2 integer primary key, %3 integer, %4 integer not null REFERENCES GeoNodesTypes(id), %5 integer, %6 integer, %7 integer not null REFERENCES GeoNodesTypes(id), %8 integer not null, %9 integer)"); // FIXME: add "NOT NULL" to copy numbers
	for (int i=0; i < tab.size(); ++i) {
		queryStr = queryStr.arg( QString::fromStdString(tab.at(i)) );
	}
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	}
	storeTableColumnNames(tab);
	tab.clear();


	// RootVolume table
	tableName = "RootVolume";
//  tab << tableName << "id" << "volId" << "volTable";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("volId");
  tab.push_back("volTable");
	queryStr = QString("create table %1(%2 integer primary key, %3 integer not null, %4 integer not null REFERENCES GeoNodesTypes(id))");
	for (int i=0; i < tab.size(); ++i) {
		queryStr = queryStr.arg( QString::fromStdString(tab.at(i)) );
	}
	//qDebug() << "query:" << queryStr;
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	}
	storeTableColumnNames(tab);
	tab.clear();


	// PhysVols table
	geoNode = "GeoPhysVol";
	tableName = "PhysVols";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "logvol" << "parent"; // FIXME: remove "parent" field, it is not used anymore and it's not reliable since it's not using the tableID.
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("logvol");
  tab.push_back("parent");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table PhysVols(id integer primary key, logvol integer not null, parent integer)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// FullPhysVols table
	geoNode = "GeoFullPhysVol";
	tableName = "FullPhysVols";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "logvol" << "parent";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("logvol");
  tab.push_back("parent");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table FullPhysVols(id integer primary key, logvol integer not null, parent integer)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// LogVols table
	geoNode = "GeoLogVol";
	tableName = "LogVols";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "name" << "shape" << "material";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("name");
  tab.push_back("shape");
  tab.push_back("material");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table LogVols(id integer primary key, name varchar, shape integer not null, material integer not null)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// Materials table
	geoNode = "GeoMaterial";
	tableName = "Materials";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "name" << "density" << "elements";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("name");
  tab.push_back("density");
  tab.push_back("elements");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table Materials(id integer primary key, name varchar, density varchar, elements varchar)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// Elements table
	geoNode = "GeoElement";
	tableName = "Elements";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "name" << "symbol" << "Z" << "A";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("name");
  tab.push_back("symbol");
  tab.push_back("Z");
  tab.push_back("A");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table Elements(id integer primary key, name varchar, symbol varchar, Z varchar, A varchar)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// Shapes table
	geoNode = "GeoShape";
	tableName = "Shapes";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "type" << "parameters";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("type");
  tab.push_back("parameters");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table Shapes(id integer primary key, type varchar, parameters varchar)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// SerialDenominators table
	geoNode = "GeoSerialDenominator";
	tableName = "SerialDenominators";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "baseName";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("baseName");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table SerialDenominators(id integer primary key, baseName varchar)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// Functions table
	geoNode = "Function";
	tableName = "Functions";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "expression";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("expression");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table Functions(id integer primary key, expression varchar)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// SerialDenominators table
	geoNode = "GeoSerialTransformer";
	tableName = "SerialTransformers";
	m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "funcId" << "volId" << "volTable" << "copies";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("funcId");
  tab.push_back("volId");
  tab.push_back("volTable");
  tab.push_back("copies");
	storeTableColumnNames(tab);
	queryStr = QString("create table %1(%2 integer primary key, %3 integer not null REFERENCES Functions(id), %4 integer not null, %5 integer not null REFERENCES GeoNodesTypes(id), %6 integer)");
	for (int i=0; i < tab.size(); ++i) {
		queryStr = queryStr.arg( QString::fromStdString(tab.at(i)) );
	}
	tab.clear();
	// qDebug() << "query:" << queryStr;
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}


	// Transforms table
	geoNode = "GeoTransform";
	tableName = "Transforms";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "xx" << "xy" << "xz" << "yx" << "yy" << "yz" << "zx" << "zy" << "zz" << "dx" << "dy" << "dz";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("xx");
  tab.push_back("xy");
  tab.push_back("xz");
  tab.push_back("yx");
  tab.push_back("yy");
  tab.push_back("yz");
  tab.push_back("zx");
  tab.push_back("zy");
  tab.push_back("zz");
  tab.push_back("dx");
  tab.push_back("dy");
  tab.push_back("dz");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table Transforms(id integer primary key, xx real, xy real, xz real, yx real, yy real, yz real, zx real, zy real, zz real, dx real, dy real, dz real)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// AlignableTransforms table
	geoNode = "GeoAlignableTransform";
	tableName = "AlignableTransforms";
  m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "xx" << "xy" << "xz" << "yx" << "yy" << "yz" << "zx" << "zy" << "zz" << "dx" << "dy" << "dz";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("xx");
  tab.push_back("xy");
  tab.push_back("xz");
  tab.push_back("yx");
  tab.push_back("yy");
  tab.push_back("yz");
  tab.push_back("zx");
  tab.push_back("zy");
  tab.push_back("zz");
  tab.push_back("dx");
  tab.push_back("dy");
  tab.push_back("dz");
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table AlignableTransforms(id integer primary key, xx real, xy real, xz real, yx real, yy real, yz real, zx real, zy real, zz real, dx real, dy real, dz real)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// NameTags table
	geoNode = "GeoNameTag";
	tableName = "NameTags";
	m_childType_tableName[geoNode] = tableName; // store type-table relation
//  tab << tableName << "id" << "name";
  tab.push_back(tableName);
  tab.push_back("id");
  tab.push_back("name");
	storeTableColumnNames(tab);
	queryStr = QString("create table %1(%2 integer primary key, %3 varchar)");
	for (int i=0; i < tab.size(); ++i) {
		queryStr = queryStr.arg( QString::fromStdString(tab.at(i)) );
	}
	tab.clear();
	// qDebug() << "query:" << queryStr;
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	return true;

}

void GMDBManager::loadTableNamesFromDB()
{
	QSqlDatabase db = QSqlDatabase::database();
	QStringList qtables = db.tables();
  std::vector<std::string> tables = toStdVectorStrings(qtables);
  for ( auto& tab : tables) {
    std::vector<std::string> tabColNames = getTableColNamesFromDB(tab);
		storeTableColumnNames(tabColNames);
	}
}

//void GMDBManager::storeTableColumnNames(QStringList input)
//{
//  //  qDebug() << "GMDBManager::storeTableColumnNames()";
//  if (! (input.isEmpty()) ) {
//    QString tabName = input.takeFirst();
//    m_tableNames[tabName] = input;
//  }
//}

void GMDBManager::storeTableColumnNames(std::vector<std::string> input)
{
  if (! (input.size() == 0) ) {
    std::string tabName = input.at(0); // get the first element storing the name of the table
    input.erase(input.begin()); // remove the first element storing the name of the table
    m_tableNames[tabName] = input;
  }
}


std::vector<std::string> GMDBManager::getTableColumnNames(const std::string &tableName)
{
	//	qDebug() << "GMDBManager::getTableColumnNames()";
	return m_tableNames.at(tableName);
}

void GMDBManager::storeNodeType(std::string nodeType, std::string tableName)
{
	// JFB commented: qDebug() << "GMDBManager::storeNodeType()";

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into GeoNodesTypes(nodeType, tableName) values(?, ?)"))) {
		showError(q.lastError());
		return;
	}

  q.addBindValue(QString::fromStdString(nodeType));
  q.addBindValue(QString::fromStdString(tableName));
	q.exec();
	return;
}



//void GMDBManager::loadTestData()
//{
//  // create test data
//  QSqlQuery q;
//  //
//  //  qDebug() << "Loading Shapes...";
//  //  if (!q.prepare(QLatin1String("insert into Shapes(name) values(?)"))) {
//  //    showError(q.lastError());
//  //    return;
//  //  }
//  QVariant boxId    = addShape(QLatin1String("Box"), "");
//  QVariant coneId   = addShape(QLatin1String("Cone"), "");
//  QVariant sphereId = addShape(QLatin1String("Sphere"), "");
//
//  qWarning() << "Sample GeoElement data --> to be implemented!";
//
//  QVariant airId  = addMaterial(QLatin1String("Air"),QLatin1String("density"),QLatin1String("elements"));
//  QVariant ironId = addMaterial(QLatin1String("Iron"),QLatin1String("density"),QLatin1String("elements"));
//  QVariant leadId = addMaterial(QLatin1String("Lead"),QLatin1String("density"),QLatin1String("elements"));
//
//  QVariant worldLogId = addLogVol(QLatin1String("WorldLog"), boxId, airId);
//  QVariant toyLogId  = addLogVol(QLatin1String("ToyLog"), coneId, leadId);
//
//  QVariant rootPhysId = addPhysVol(worldLogId, QVariant()); // no parent
//  QVariant childPhysId = addPhysVol(toyLogId, rootPhysId);
//}


std::vector<std::string> GMDBManager::getTableColNamesFromDB(std::string tableName) const
{
  std::vector<std::string> colNames;
	colNames. push_back( tableName );

	// fetch the driver
	QSqlDriver* driver = m_db.driver();
  QSqlRecord record = driver->record(QString::fromStdString(tableName));
	unsigned int colN = record.count();
	for (unsigned int i = 0; i < colN; ++i)
		colNames.push_back( record.fieldName(i).toStdString() );
	return colNames;
}





//void GMDBManager::printTableColNamesFromDB(const std::string &tableName) const
//{
//  // JFB commented: qDebug() << "GMDBManager::printTableColNames:" << tableName;
//
//  // fetch the driver
//  // JFB commented: qDebug() << getTableColNamesFromDB(tableName);
//}

bool GMDBManager::storeRootVolume(const unsigned int &id, const std::string &nodeType)
{
	const unsigned int typeId = getTableIdFromNodeType(nodeType);

  std::string tableName = "RootVolume";
  std::vector<std::string> cols = getTableColumnNames(tableName);

	// prepare the query string
	QString queryStr = QString("insert into %1(%2, %3) values(?, ?)").arg( QString::fromStdString(tableName) );
	for (int i=0; i < cols.size(); ++i) {
		if (i != 0) // skip the "id" column
    queryStr = queryStr.arg( QString::fromStdString(cols.at(i)) );
	}
	//qDebug() << "queryStr:" << queryStr;
	// prepare the query
	QSqlQuery q;
	if (!q.prepare(queryStr)) {
		showError(q.lastError());
		return false;
	}
	q.addBindValue(id);
	q.addBindValue(typeId);
	// run the query
	q.exec();
	return true;
}

/*
std::vector<std::string> GMDBManager::getRootPhysVol()
{
	// get the ID of the ROOT vol from the table "RootVolume"
	// QSqlQuery query("SELECT * FROM RootVolume");
	QSqlQuery query = selectAllFromTable("RootVolume");

	QVariant id;
	QVariant typeId;
	while (query.next()) {
		id = query.value(1);
		typeId = query.value(2);
	}
	return getItemAndType(typeId.toUInt(), id.toUInt());
}
*/

std::vector<std::string> GMDBManager::getRootPhysVol()
{
  // get the ID of the ROOT vol from the table "RootVolume"
  QSqlQuery query = selectAllFromTable("RootVolume");
  
  unsigned int id;
  unsigned int typeId;
  while (query.next()) {
    id = query.value(1).toUInt();
    typeId = query.value(2).toUInt();
  }
  return getItemAndType(typeId, id);
}


std::string GMDBManager::getDBFilePath()
{
    return m_dbpath;
}

// TODO: move to an utility class
int lastIndexOf(std::vector<std::string> v, std::string str, int pos = 0) {
  auto it = std::find(std::next(v.rbegin(), v.size() - pos), v.rend(), str);
  if (it != v.rend())
  {
    auto idx = std::distance(v.begin(), it.base() - 1);
//    std::cout << idx << std::endl;
    return idx;
  }
  return -1; // no item matched
}

int GMDBManager::getTableColIndex(const std::string &tableName, const std::string &colName)
{
  std::vector<std::string> colFields = m_tableNames.at(tableName);
	return lastIndexOf(colFields, colName);
}


