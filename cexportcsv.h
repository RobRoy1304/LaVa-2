#ifndef CEXPORTCSV_H
#define CEXPORTCSV_H

#include <QtGui/QDialog>

#include "csettings.h"
#include "cdbconnection.h"
#include "cwidgetinterface.h"

class CExportCSV
{
public:
    CExportCSV();

    bool write(QString sFile, QList<QString> & ls);
    bool check_format(QList<QString> & ls);
    QString check_format(QString s);
    bool open_filedialog(QString & sPath, QWidget * pParent, QString sFilename);
    bool create_data_table(QTableWidget * pTable, QList<QString> & ls, QString sTitle=QString(""),bool bLastColumn=false);
    bool create_data_table(QTableWidget * pTable, QList<QString> & ls, int iFromColumn, int iToColumn, QString sTitle=QString(""));
    bool create_data_list(QListWidget * pList, QList<QString> & ls, QString sTitle=QString(""));
    bool create_data_list_table(QListWidget * pList, QTableWidget * pTable, QList<QString> & ls, QString sTitle=QString(""),bool bLastColumn=false);
    bool create_data_tree(QTreeWidget * pTree, QList<QString> & ls, QString sTitle=QString(""));
    bool create_data_tree_children(QTreeWidgetItem * pItemParent, QList<QString> & ls, int iSubLevel);
    bool write_data_table(QWidget * pParent, QTableWidget * pTable, QString sFileDescription, QString sTitle=QString(""),bool bLastColumn=false);
    bool write_data_list(QWidget * pParent, QListWidget * pList, QString sFileDescription, QString sTitle=QString(""));
    bool write_data_list_table(QWidget * pParent, QListWidget * pList, QTableWidget * pTable, QString sFileDescription, QString sTitle=QString(""),bool bLastColumn=false);
    bool write_data_tree(QWidget * pParent,QTreeWidget * pTree, QString sFileDescription, QString sTitle=QString(""));
    bool write_data_maker(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, QString sFileDescription, QString sTitle=QString(""));
    bool write_data_dealer(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, QString sFileDescription, QString sTitle=QString(""));
    bool write_data_customer(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, QString sFileDescription, QString sTitle=QString(""));
    bool write_data_inventory(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, CWidgetInterface & widget, QString sFileDescription, QString sTitle=QString(""));
    bool write_data_article(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, QString sFileDescription, QString sTitle=QString(""));
    bool write_data_ordering(QWidget * pParent, QTableWidget * pTable, CDbConnection & db,CWidgetInterface & widget, QString sFileDescription, QString sTitle=QString(""));
    bool write_data_trade(QWidget * pParent, QTableWidget * pTable, CDbConnection & db,CWidgetInterface & widget, QString sFileDescription, QString sTitle=QString(""));
};

#endif // CEXPORTCSV_H
