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

#ifndef CINPUTDIALOGBROWSEARTICLE_H
#define CINPUTDIALOGBROWSEARTICLE_H

#include <QDialog>
#include "cworkthread.h"
#include "csettings.h"

namespace Ui {
    class CInputDialogBrowseArticle;
}

class CInputDialogBrowseArticle : public QDialog
{
    Q_OBJECT

private:
    Ui::CInputDialogBrowseArticle *ui;
    CWorkThread * m_pThread;

public:
    explicit CInputDialogBrowseArticle(QWidget *parent = 0);
    ~CInputDialogBrowseArticle();
    virtual void keyPressEvent(QKeyEvent * event);
    void set_thread(CWorkThread * pThread){m_pThread=pThread;}
    bool get_data(int & iArticleId);
    bool set_data(int iArticleId);
    bool settings(bool bUpdate=false);

public slots:
    bool update_table(void);
    void checkbox_auto_clicked(void);
    void mask_edit(void);
    bool check_user_input(void);
};

#endif // CINPUTDIALOGBROWSEARTICLE_H
