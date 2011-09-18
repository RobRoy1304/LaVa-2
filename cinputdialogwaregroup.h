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

#ifndef CINPUTDIALOGWAREGROUP_H
#define CINPUTDIALOGWAREGROUP_H

#include <QtGui/QDialog>
#include "cworkthread.h"
#include "csettings.h"

namespace Ui {
    class CInputDialogWaregroup;
}

class CInputDialogWaregroup : public QDialog
{
    Q_OBJECT

private:
    Ui::CInputDialogWaregroup *ui;
    int m_iWaregroupId;//only for edit mode
    CWorkThread * m_pThread;

protected:
    void changeEvent(QEvent *e);
    bool check_waregroupname(void);

public:
    explicit CInputDialogWaregroup(QWidget *parent = 0);
    ~CInputDialogWaregroup();
    void set_thread(CWorkThread * pThread=NULL);
    void set_data(CWaregroup & wg);
    void get_data(CWaregroup & wg);
    void update_parent_label(void);
    void update_parent_label(int iId);
    void update_waregroup_tree(int iSelectId);
    bool settings(bool bUpdate=false);

public slots:
    bool check_user_input();
    bool treewidget_clicked(QTreeWidgetItem* pItem);
};

#endif // CINPUTDIALOGWAREGROUP_H
