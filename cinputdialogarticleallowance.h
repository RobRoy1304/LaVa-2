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

#ifndef CINPUTDIALOGARTICLEALLOWANCE_H
#define CINPUTDIALOGARTICLEALLOWANCE_H

#include <QtGui/QDialog>
#include "cworkthread.h"
#include "csettings.h"

#define ARTICLE_DLG_TYPE_INCOMING 0
#define ARTICLE_DLG_TYPE_OUTGOING 1
#define ARTICLE_DLG_TYPE_ORDERING 2

namespace Ui {
    class CInputDialogArticleAllowance;
}

class CInputDialogArticleAllowance : public QDialog {
    Q_OBJECT
public:
    CInputDialogArticleAllowance(QWidget *parent = 0);
    ~CInputDialogArticleAllowance();
    virtual void keyPressEvent(QKeyEvent * event);

    bool set(CWorkThread * pThread, QTableWidget * pParentTable, int iColumnCount, int iColumnId, int iType, int iArticleId, QDate dtDate=QDate(0,0,0));
    bool update_table(void);
    bool check_count(void);
    int get_old_count(int iArticleId);
    bool get_data(QString & sData);
    bool set_count(int iCount);
    int get_count_on_date(int iArticleId);
    bool settings(bool bUpdate=false);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CInputDialogArticleAllowance *ui;
    int m_iArticleId;
    int m_iType;
    int m_iParentColumnCount;
    int m_iParentColumnId;
    QTableWidget * m_pParentTable;
    CWorkThread * m_pThread;
    QDate m_dtDate;

public slots:
    void mask_changed(void);
    void article_changed(void);
    void checkbox_auto_clicked(void);
    void mask_edit(void);
    void count_changed(void);
};

#endif // CINPUTDIALOGARTICLEALLOWANCE_H
