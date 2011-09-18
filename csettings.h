/*  LaVa 2, a inventory managment tool
    Copyright (C) 2011 - Robert Ewert - robert.ewert@gmail.com - www.robert.ewert.de.vu
    created with QtCreator(Qt 4.7.0)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CSETTINGS_H
#define CSETTINGS_H

#include <QtGui>

class CSettings
{
public:
    CSettings(){}
    ~CSettings(){}
    bool create_default_setting_file(void);
    bool load(QList<QString> & lsSType, QList<QString> & lsSValue);
    bool load(QString sType, QString & sValue);
    bool write(QList<QString> & lsSType, QList<QString> & lsSValue);
    bool write(QString sType, QString sValue);
    bool set_table_width(QTableWidget * pTable, QString sValues, int iDefaultValue);
    bool set_table_sort(QTableWidget * pTable, QString sValues, int iDefaultValue);
    bool set_checkbox(QCheckBox * pCheckBox,QString sValue, bool bDefaultValue);
    bool set_combobox(QComboBox * pComboBox, QString sValue, int iDefaultValue);
    bool set_tree_width(QTreeWidget * pTree,QString sValues, int iDefaultValue);
    bool set_tab_order(QTabWidget * pTab, QString sValues);
    bool get_table_width(QTableWidget * pTable, QString & sValues);
    bool get_table_sort(QTableWidget * pTable, QString & sValues);
    bool get_checkbox(QCheckBox * pCheckBox,QString & sValue);
    bool get_combobox(QComboBox * pComboBox, QString & sValue);
    bool get_tree_width(QTreeWidget * pTree, QString & sValues);
    bool get_tab_order(QTabWidget * pTab, QString & sValues, QList<QWidget*> & lsPWidget);
    bool remove_line(QString sType);
    bool cast_string_to_int_list(QString s, QList<int> & lsInt, bool bWithType=false);
    QString cast_int_list_to_string(QList<int> & lsInt, QString sType=QString(""));
    bool give_it_setting_file(void);
    bool is_program_running(void);
};

#endif // CSETTINGS_H
