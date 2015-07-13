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

#ifndef CDLGBALANCELIST_H
#define CDLGBALANCELIST_H

#include <QDialog>
#include <QPrintPreviewDialog>
#include "cworkthread.h"
#include "cinputdialogbrowsearticle.h"
#include "cprintjob.h"
#include "cexportcsv.h"


namespace Ui {
    class CDlgBalanceList;
}

class CDlgBalanceList : public QDialog
{
    Q_OBJECT

private:
    Ui::CDlgBalanceList *ui;
    CWorkThread * m_pThread;
    int m_iArticleId;
    CPrintJob m_print_job;

public:
    explicit CDlgBalanceList(QWidget *parent = 0);
    ~CDlgBalanceList();
    void set_thread(CWorkThread * pThread){m_pThread=pThread;}
    bool update_balance_list(void);
    bool update_article_info(void);
    bool check_user_input(void);
    bool settings(bool bUpdate=false);

public slots:
    bool browse_article_dlg(void);
    bool dates_changed(void);
    bool print_button_press(void);
    bool export_button_press(void);
    bool print(QPrinter * printer);
};

#endif // CDLGBALANCELIST_H
