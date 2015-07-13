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

#include <QtGui>
#include <QPrinter>
#include "data_classes.h"
#include "cworkthread.h"

#ifndef CPRINTJOB_H
#define CPRINTJOB_H

//print job
#define PRINT_JOB_LIST 1
#define PRINT_JOB_TABLE 2
#define PRINT_JOB_TREE 3
#define PRINT_JOB_TABLE_AND_LIST 4
#define PRINT_JOB_BARCODE_OVERVIEW 5

class CPrintJob
{
public:
    int m_iType;
    CPointerMemory m_memory;
    QPrinter * m_pPrinter;
    QString m_sFirstRow;
    CWorkThread * m_pThread;
    void set_db(CWorkThread * pThread){m_pThread=pThread;}
    //-
    CPrintJob();
    ~CPrintJob();
    int get_type(void);
    void set_type(int iType);
    bool check_text_space(QString & sText, QFontMetrics * pMetric, QRectF rRect);
    int get_text_width(QString sText, QFontMetrics * pMetric);
    bool draw_text(QPainter * pPainter, QFont * pFont, QRectF rRect, QString sText, bool bold=false, bool underline=false, bool italic=false);
    bool print_list(void);
    bool print_table(void);
    bool print_table_and_list(void);
    bool print_tree(void);
    bool print_tree_cildren(QTreeWidgetItem * pParent, int & iRow, int iRowSpace, QRect fillRect, QPainter * pPainter, QFont & font);
    bool print_barcode_overview(void);
    bool draw_barcode(CEan & ean, QPainter * pPainter, QPoint ptPos);
    qreal get_barcode_height(CEan & ean, QPainter * pPainter);
    qreal get_barcode_height(QPainter * pPainter, int TYPE_EAN);
};

#endif // CPRINTJOB_H
