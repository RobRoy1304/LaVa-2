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

#include <QtGui>
#include "data_classes.h"

#ifndef CPRINTJOB_H
#define CPRINTJOB_H

//print job
#define PRINT_JOB_LIST 1
#define PRINT_JOB_TABLE 2
#define PRINT_JOB_TREE 3
#define PRINT_JOB_TABLE_AND_LIST 4

class CPrintJob
{
public:
    CPointerMemory m_memory;
    QPrinter * m_pPrinter;
    QString m_sFirstRow;
    //-
    CPrintJob();
    ~CPrintJob();
    int get_type(void);
    bool check_text_space(QString & sText, QFontMetrics * pMetric, QRectF rRect);
    int get_text_width(QString sText, QFontMetrics * pMetric);
    bool draw_text(QPainter * pPainter, QFont * pFont, QRectF rRect, QString sText, bool bold=false, bool underline=false, bool italic=false);
    bool print_list(void);
    bool print_table(void);
    bool print_table_and_list(void);
    bool print_tree(void);
    bool print_tree_cildren(QTreeWidgetItem * pParent, int & iRow, int iRowSpace, QRect fillRect, QPainter * pPainter, QFont & font);
};

#endif // CPRINTJOB_H
