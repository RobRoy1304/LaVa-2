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

#ifndef CINPUTDLGBARCODE_H
#define CINPUTDLGBARCODE_H

#include <QtGui/QDialog>
#include "cworkthread.h"
#include "csettings.h"
#include "cinputdialogbrowsearticle.h"

#define ARTICLE_DLG_TYPE_INCOMING 0
#define ARTICLE_DLG_TYPE_OUTGOING 1
#define ARTICLE_DLG_TYPE_ORDERING 2
#define RETURN_ARTICLE_NOT_FOUND -1
#define RETURN_USER_EXIT 0
#define RETURN_ARTICLE_FOUND 2

namespace Ui {
class CInputDlgBarcode;
}

class CInputDlgBarcode : public QDialog
{
    Q_OBJECT

private:
    Ui::CInputDlgBarcode *ui;
    //-
    int m_iType;
    int m_iParentColumnCount;
    int m_iParentColumnId;
    QTableWidget * m_pParentTable;
    CWorkThread * m_pThread;
    QDate m_dtDate;

public:
    explicit CInputDlgBarcode(QWidget *parent = 0);
    ~CInputDlgBarcode();
    virtual void keyPressEvent(QKeyEvent * event);
    //-
    bool set(CWorkThread * pThread, QTableWidget * pParentTable, int iColumnCount, int iColumnId, int iType, QDate dtDate=QDate(0,0,0));
    bool settings(bool bUpdate=false);
    bool add_ware(void);
    int get_article_id_by_barcode(QString s, int number, CArticle & ar);
    int get_parent_table_count(int iArticleId);
    int get_count_on_date(int iArticleId);
    int get_ordering_count(int iArticleId);

public slots:
    void articlenumber_changed(void);
};

#endif // CINPUTDLGBARCODE_H
