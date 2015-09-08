/*  LaVa 2, a inventory managment tool
    Copyright (C) 2015 - Robert Ewert - robert.ewert@gmail.com - www.robert.ewert.de.vu
    created with QtCreator(Qt 4.8)

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

#ifndef CINPUTDIALOGIMPORT_H
#define CINPUTDIALOGIMPORT_H

#include <QtGui/QDialog>
#include <QFileDialog>
#include <QTableWidget>
#include <QInputDialog>
#include <QMessageBox>
#include "cworkthread.h"
#include "data_classes.h"
#include "csettings.h"


#define ARTICLE_MEMBER_TYPE {0,1,1,0,0,0,0,0,1,2,2,0,0,0,0} //0=text 1=int 2=double

namespace Ui {
class CInputDialogImport;
}

class CInputDialogImport : public QDialog
{
    Q_OBJECT
    
public:
    explicit CInputDialogImport(QWidget *parent = 0);
    ~CInputDialogImport();

    void set_thread(CWorkThread * pThread){m_pThread=pThread;}
    bool settings(bool bUpdate=false);

    //step 1
    bool update_view(void);
    bool check_and_set(QString sFile);
    bool set_error_msg_step1(bool bError);
    bool tranfer_fileview_data(void);

    //step 2
    bool set_allocation_by_header(void);
    bool update_allocation_table(void);
    bool check_allocation_settings(void);
    int get_next_column_number_where_not_set(void);
    bool check_column_data_and_member_type(int iSourceColumn, int iMemberRow);
    bool set_error_msg_step2(bool bError);

    //step 3
    bool update_record_table(void);
    bool check_record_table_data(void);
    bool check_update_double_names_recordtable(void);
    
private:
    Ui::CInputDialogImport *ui;
    QString m_sFile;
    QList<QString> m_lsFileRows;
    int m_iTabChangeToEnable;
    bool m_bUpdateStep1to2;
    bool m_bUpdateStep2to3;
    QString m_sMarkValueUserEditFileview2;
    QString m_sMarkValueUserEditRecordTable;
    bool m_bCheckItemDataRecordTable;
    CWorkThread * m_pThread;
    int m_iDataType;//0-article 1-maker 2-dealer 3-customer 4-waregroup
    bool m_bCheckboxFirstRowHeader;

public slots:
    //basic
    QString get_source_file(void){return m_sFile;}
    bool current_tab_changed(int i);
    int get_data_type(void){return m_iDataType;}

    //step 1
    bool push_browse_file(void);
    bool check_comma(void);
    bool check_space(void);
    bool check_semi(void);
    bool check_tab(void);
    bool changed_textsplitter(void);
    bool edit_split_char(void);
    bool changed_spin_from(int i);
    bool changed_spin_to(int i);
    bool push_button_next_1_clicked(void);
    bool changed_combobox_codec(void);
    bool check_first_row_header(bool bChecked);

    //step 2
    bool combobox_data_type_changed(void);
    bool push_button_back_1_clicked(void);
    bool push_button_next_2_clicked(void);
    bool allocation_table_clicked(void);
    bool column_allocation_change(void);
    bool table_fileview2_double_clicked(QTableWidgetItem* pItem);
    bool table_fileview2_item_changed(QTableWidgetItem* pItem);

    //step 3
    bool push_button_back_2_clicked(void);
    bool record_table_clicked(void);
    bool push_button_delete_row_clicked(void);
    bool table_record_double_clicked(QTableWidgetItem* pItem);
    bool table_record_item_changed(QTableWidgetItem* pItem);
    bool import_button_clicked(void);
    };

#endif // CINPUTDIALOGIMPORT_H
