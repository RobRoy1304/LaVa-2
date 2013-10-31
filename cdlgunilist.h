/*  LaVa 2, a inventory managment tool
    Copyright (C) 2013 - Robert Ewert - robert.ewert@gmail.com - www.robert.ewert.de.vu
    created with QtCreator(Qt 4.8.0)

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

#ifndef CDLGUNILIST_H
#define CDLGUNILIST_H

#include <QDialog>
#include "cworkthread.h"
#include "cprintjob.h"
#include "csettings.h"
#include "cexportcsv.h"


#define TYPE_WARN_LIST 1
#define TYPE_INVENTORYS_ON_DATE 2
#define TYPE_INVENTORY_LIST 3

namespace Ui {
    class CDlgUniList;
}

class CDlgUniList : public QDialog
{
    Q_OBJECT

private:
    Ui::CDlgUniList *ui;
    CWorkThread * m_pThread;
    int m_iType;
    CPrintJob m_print_job;

public:
    explicit CDlgUniList(QWidget *parent = 0);
    ~CDlgUniList();
    bool check_user_input(void);
    void set_thread(CWorkThread * pThread){m_pThread=pThread;}
    void set_type(int iType);
    bool create_table_columns(QList<QString> & lsStrColumnname);
    bool update_table(void);
    bool settings(bool bUpdate=false);

public slots:
    bool print_button_press(void);
    bool export_button_press(void);
    bool print(QPrinter * printer);
    void date_changed(void);
};

#endif // CDLGUNILIST_H
