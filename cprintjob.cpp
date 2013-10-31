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

#include "cprintjob.h"


//print job------------------------------------------------------------------------------------------------------
CPrintJob::CPrintJob()
{
    m_pPrinter=NULL;
}

CPrintJob::~CPrintJob()
{
    m_memory.clear();
}

int CPrintJob::get_type(void)
{
    int r=-1;
    if(m_memory.get_listwidget()!=NULL && m_memory.get_tablewidget()==NULL)
        r=PRINT_JOB_LIST;
    else if(m_memory.get_tablewidget()!=NULL && m_memory.get_tablewidget2()==NULL)//1 table
        r=PRINT_JOB_TABLE;
    else if(m_memory.get_treewidget()!=NULL)
        r=PRINT_JOB_TREE;
    else if(m_memory.get_tablewidget()!=NULL && m_memory.get_tablewidget2()!=NULL)//2 table
        r=PRINT_JOB_TABLE_AND_LIST;
    return r;
}

bool CPrintJob::check_text_space(QString & sText, QFontMetrics * pMetric, QRectF rRect)
{
    bool b=false;
    QString s=sText;
    QString sPoints=QString("...");
    int i,width=rRect.right()-rRect.left();
    if(pMetric!=NULL && width>0 && sText.length()>0)
    {
        width-=pMetric->width(sPoints);
        while(pMetric->width(sText) > width)
        {
            i=sText.length()-1;
            if(i<=0)
                break;
            sText.remove(i,1);
        }
        if(s!=sText && sText.length()>0)//change?
            sText+=QString("...");
        b=true;
    }
    return b;
}

int CPrintJob::get_text_width(QString sText, QFontMetrics * pMetric)
{
    int iReturn=-1;
    if(pMetric!=NULL && sText.length()>0)
        iReturn=pMetric->width(sText);
    return iReturn;
}

bool CPrintJob::draw_text(QPainter * pPainter, QFont * pFont, QRectF rRect, QString sText, bool bold, bool underline, bool italic)
{
    bool b=false;
    if(pPainter!=NULL && pFont!=NULL)
    {
        QFont font(*pFont);
        if(bold)
            font.setBold(true);
        else
            font.setBold(false);
        if(underline)
            font.setUnderline(true);
        else
            font.setUnderline(false);
        if(italic)
            font.setItalic(true);
        else
            font.setItalic(false);
        pPainter->setFont(font);
        //-
        QFontMetrics metric(pPainter->fontMetrics());
        check_text_space(sText,&metric,rRect);
        pPainter->drawText(rRect,sText);
        //-
        pPainter->setFont(*pFont);
        b=true;
    }
    return b;
}

bool CPrintJob::print_list(void)
{
    if(m_pPrinter==NULL)
        return false;

    //date+time
    QDateTime dtTi=QDateTime::currentDateTime();
    QString sDateTime=QString(" (erstellt %1 Uhr, %2)").arg(dtTi.toString("hh:mm:ss"),dtTi.toString("dd.MM.yyyy"));

    bool b=false;
    QRectF rF;
    QString s;
    int i,row=0,count,row_space,copies_count,page,copies=0;
    QFont font("Sans",8,0,0);
    qreal marginLeft,marginTop,marginRight,marginBottom;
    QRect paper = m_pPrinter->pageRect();
    QListWidget * pList=m_memory.get_listwidget();
    QListWidgetItem * pItem=NULL;
    if(m_pPrinter!=NULL && pList!=NULL)
    {
        b=true;

        //copy count
        copies_count=m_pPrinter->numCopies();

        //painter
        QPainter painter(m_pPrinter);
        painter.setPen(Qt::black);

        //margin
        m_pPrinter->getPageMargins(&marginLeft,&marginTop,&marginRight,&marginBottom,QPrinter::DevicePixel);
        QRect fillRect=QRect(marginLeft,marginTop,(paper.right()-paper.left())-((int)marginRight+marginLeft),(paper.bottom()-paper.top())-(((int)marginBottom+marginTop)));

        //font
        painter.setFont(font);
        QFontMetrics metric(painter.fontMetrics());
        row_space=metric.height();

        //copies
        while(copies<copies_count)
        {
            row=0;
            page=1;

            //first row(title)
            if(m_sFirstRow.length()>0)
            {
                rF=QRectF(fillRect.left(),fillRect.top(),fillRect.right(),fillRect.top()+row_space);
                draw_text(&painter,&font,rF,m_sFirstRow,true,true);
            }

            //date+time
            rF=QRectF(fillRect.right()-get_text_width(sDateTime,&metric),fillRect.top()+(row*row_space),fillRect.right(),fillRect.top()+((row+1)*row_space));
            draw_text(&painter,&font,rF,sDateTime,false,false,false);//normal
            row+=4;

            //items
            count=pList->count();
            for(i=0;i<count;i++,row++)
            {
                if(fillRect.top()+((row+2)*row_space)>fillRect.bottom())
                {
                    m_pPrinter->newPage();
                    row=-1;
                    i-=1;
                    continue;
                }
                //-
                rF=QRectF(fillRect.left(),fillRect.top()+(row*row_space),fillRect.right()-fillRect.left(),fillRect.top()+((row+1)*row_space));
                pItem=pList->item(i);
                if(pItem!=NULL)
                {
                    s=pItem->text();
                    if(s.length()>0)//empty row?
                    {
                        if(pItem->font().bold())
                            draw_text(&painter,&font,rF,s,true,false,true);//italic+bold
                        else
                            draw_text(&painter,&font,rF,s,false,false,false);//normal
                    }
                }
                //-
                if(i+1==count || fillRect.top()+((row+2)*row_space)>fillRect.bottom())//last row on page? ->print page count
                {
                    //print page / column count(center)
                    s=QString("Seite %1").arg(page);
                    rF=QRectF(fillRect.left()+((fillRect.right()-fillRect.left()-metric.width(s))/2),fillRect.bottom()-row_space,(fillRect.right()-fillRect.left())/2,row_space);
                    draw_text(&painter,&font,rF,s,false,false,false);
                    page++;
                }
            }
            //-
            copies++;
            if(copies<copies_count)
                m_pPrinter->newPage();
        }
        //-
        b=true;
    }
    //-
    return b;
}

bool CPrintJob::print_table(void)
{

    if(m_pPrinter==NULL)
        return false;

    bool b=false;
    QList<int> lsColumnsWidth;
    QRectF rF;
    QString s;
    bool bFirst=true;
    int wAll,i,j,z,x,l,row=0,count,row_space,distTopLeftCorner,page,copies_count,copies=0,dx,iAliRight=Qt::AlignRight|Qt::AlignVCenter,iAliCenter=Qt::AlignHCenter|Qt::AlignVCenter;
    QFont font("Sans",8,0,0);
    qreal marginLeft,marginTop,marginRight,marginBottom;
    QRect paper = m_pPrinter->pageRect();
    QTableWidget * pTable=m_memory.get_tablewidget();
    QTableWidgetItem * pItem=NULL;
    int iDiff=1,iDiff2=0;
    if(m_memory.get_bool()!=NULL)//all columns?
    {
        if(*m_memory.get_bool()==true)
            iDiff=0;
    }

    //date+time
    QDateTime dtTi=QDateTime::currentDateTime();
    QString sDateTime=QString(" (erstellt %1 Uhr, %2)").arg(dtTi.toString("hh:mm:ss"),dtTi.toString("dd.MM.yyyy"));

    if(m_pPrinter!=NULL && pTable!=NULL)
    {
        b=true;

        //copy count
        copies_count=m_pPrinter->numCopies();

        //painter
        QPainter painter(m_pPrinter);
        painter.setPen(Qt::black);

        //margin
        m_pPrinter->getPageMargins(&marginLeft,&marginTop,&marginRight,&marginBottom,QPrinter::DevicePixel);
        QRect fillRect=QRect(marginLeft,marginTop,(paper.right()-paper.left())-((int)marginRight+marginLeft),(paper.bottom()-paper.top())-(((int)marginBottom+marginTop)));

        //font
        painter.setFont(font);
        QFontMetrics metric(painter.fontMetrics());
        distTopLeftCorner=metric.height()/4;
        row_space=metric.height()+(2*distTopLeftCorner);

        //columns width
        for(wAll=0,i=0;i<pTable->columnCount();i++)
            wAll+=pTable->columnWidth(i);//really column width(all) (100%)

        for(x=0,i=0;i<pTable->columnCount();i++)
        {
            j=(int) ((pTable->columnWidth(i)*100 )/wAll);// %
            z=((fillRect.right()-fillRect.left())/100)*j;// calc % of printer (px)
            if(z<0)
                z=0;
            if(z==0)
                iDiff2++;//mark columns not visible(width=0)
            lsColumnsWidth.push_back(z); //mark
            x+=z;//mark all
        }

        //rounding errors
        j=fillRect.right()-fillRect.left();
        if(x<j && lsColumnsWidth.count()>1)
        {
            z=(j-x)/(pTable->columnCount()-iDiff-iDiff2);
            for(x=0,i=0;x<pTable->columnCount()-iDiff;x++)
            {
                if(lsColumnsWidth[x]>0)//column visible?
                {
                    lsColumnsWidth[x]+=z;
                    i+=lsColumnsWidth[x];
                }
            }
            if(i<j)
            {
                for(x=lsColumnsWidth.count()-(1+iDiff);x>=0 && x<pTable->columnCount();x--)//search last visible column
                {
                    if(lsColumnsWidth[x]>0)
                    {
                        lsColumnsWidth[x]+=j-i;//add the rest
                        break;
                    }
                }
            }
        }


        //copies
        while(copies<copies_count && pTable->columnCount()>0)
        {
            row=0;
            page=1;

            //first row(title)
            if(m_sFirstRow.length()>0)
            {
                rF=QRectF(fillRect.left(),fillRect.top(),fillRect.right(),fillRect.top()+row_space);
                draw_text(&painter,&font,rF,m_sFirstRow,true,true,false);
            }

            //date+time
            rF=QRectF(fillRect.right()-get_text_width(sDateTime,&metric),fillRect.top()+(row*row_space),fillRect.right(),fillRect.top()+((row+1)*row_space));
            draw_text(&painter,&font,rF,sDateTime,false,false,false);//normal
            row+=2;//+empty row

            //items
            count=pTable->rowCount();
            for(i=0;i<count;row++,i++)
            {
                if(bFirst)//header?
                {
                    rF=QRectF(fillRect.left(),fillRect.top()+(row*row_space),fillRect.right()-fillRect.left(),row_space);
                    painter.fillRect(rF,QColor(200,200,200));
                }
                else if(i%2!=0)//table items - all 2. row light grey
                {
                    rF=QRectF(fillRect.left(),fillRect.top()+(row*row_space),fillRect.right()-fillRect.left(),row_space);
                    painter.fillRect(rF,QColor(235,235,235));
                }

                for(l=0,x=0;x<lsColumnsWidth.count()-iDiff;x++)//last column is id(not print)
                {
                    if(lsColumnsWidth[x]>0)//column visible?
                    {
                        //draw border rect
                        rF=QRectF(fillRect.left()+l,fillRect.top()+(row*row_space),lsColumnsWidth[x],row_space);
                        painter.drawRect(rF);

                        //get text
                        if(bFirst)//header?
                        {
                            pItem=pTable->horizontalHeaderItem(x);
                            if(pItem!=NULL)
                                s=pItem->text();
                            else{}
                        }
                        else
                        {
                            pItem=pTable->item(i,x);
                            if(pItem!=NULL)
                                s=pItem->text();
                        }

                        //get dist. by aglinment
                        dx=distTopLeftCorner;//left=fefault
                        if(pItem!=NULL && bFirst==false)
                        {
                            if(lsColumnsWidth[x] > get_text_width(s,&metric))
                            {
                                if(pItem->textAlignment()==iAliRight)//right
                                    dx=lsColumnsWidth[x]-(get_text_width(s,&metric)+(2*distTopLeftCorner));
                                else if(pItem->textAlignment()==iAliCenter)//center
                                    dx=(lsColumnsWidth[x]-(get_text_width(s,&metric)+(2*distTopLeftCorner)))/2;
                                //-
                                if(dx<distTopLeftCorner)
                                    dx=distTopLeftCorner;
                            }
                        }
                        pItem=NULL;

                        //calc draw rect
                        rF=QRectF(fillRect.left()+l+dx,fillRect.top()+(row*row_space)+distTopLeftCorner,lsColumnsWidth[x]-(2*distTopLeftCorner),row_space-(2*distTopLeftCorner));

                        //draw text
                        if(bFirst)//header?
                            draw_text(&painter,&font,rF,s,true,false,false);
                        else
                            draw_text(&painter,&font,rF,s,false,false,false);

                        //-
                        l+=lsColumnsWidth[x];
                    }
                }
                if(bFirst)
                {
                    bFirst=false;
                    i--;
                }

                if(i+1==count || fillRect.top()+((row+3)*row_space)>fillRect.bottom())//last row on page? ->print page count
                {
                    //print page / column count(center)
                    s=QString("Seite %1").arg(page);
                    rF=QRectF(fillRect.left()+((fillRect.right()-fillRect.left()-metric.width(s))/2),fillRect.bottom()-(row_space-(2*distTopLeftCorner)),fillRect.right()-fillRect.left(),row_space);
                    draw_text(&painter,&font,rF,s,false,false,false);
                    page++;
                }

                if(fillRect.top()+((row+3)*row_space)>fillRect.bottom())//new page?
                {
                    //next page
                    m_pPrinter->newPage();
                    bFirst=true;
                    row=-1;
                }
            }
            //-
            copies++;
            if(copies<copies_count)
            {
                m_pPrinter->newPage();
                bFirst=true;
            }
        }
    }
    lsColumnsWidth.clear();
    //-
    return b;
}

bool CPrintJob::print_table_and_list(void)
{
    if(m_pPrinter==NULL)
        return false;

    bool b=false;
    QList<int> lsColumnsWidth;
    QRectF rF;
    QString s;
    bool bFirst=true;
    int wAll,i,j,z,x,l,row=0,count,row_space,distTopLeftCorner,copies_count,page,copies=0,dx,iAliRight=Qt::AlignRight|Qt::AlignVCenter,iAliCenter=Qt::AlignHCenter|Qt::AlignVCenter;
    QFont font("Sans",8,0,0);
    qreal marginLeft,marginTop,marginRight,marginBottom;
    QRect paper = m_pPrinter->pageRect();
    QTableWidget * pTable=m_memory.get_tablewidget();
    QTableWidget * pTableInfo=m_memory.get_tablewidget2();
    QTableWidgetItem * pItem=NULL;
    QList<QTableWidgetItem*> ls;
    QListWidget * pList=m_memory.get_listwidget();
    QListWidgetItem * pListItem=NULL;
    int iDiff=1;
    if(m_memory.get_bool()!=NULL)//all columns?
    {
        if(*m_memory.get_bool()==true)
            iDiff=0;
    }

    //date+time
    QDateTime dtTi=QDateTime::currentDateTime();
    QString sDateTime=QString(" (erstellt %1 Uhr, %2)").arg(dtTi.toString("hh:mm:ss"),dtTi.toString("dd.MM.yyyy"));

    if(m_pPrinter!=NULL && pTable!=NULL)
    {
        b=true;

        //copy count
        copies_count=m_pPrinter->numCopies();

        //painter
        QPainter painter(m_pPrinter);
        painter.setPen(Qt::black);

        //margin
        m_pPrinter->getPageMargins(&marginLeft,&marginTop,&marginRight,&marginBottom,QPrinter::DevicePixel);
        QRect fillRect=QRect(marginLeft,marginTop,(paper.right()-paper.left())-((int)marginRight+marginLeft),(paper.bottom()-paper.top())-(((int)marginBottom+marginTop)));

        //font
        painter.setFont(font);
        QFontMetrics metric(painter.fontMetrics());
        distTopLeftCorner=metric.height()/4;
        row_space=metric.height()+(2*distTopLeftCorner);

        //columns width
        for(wAll=0,i=0;i<pTable->columnCount();i++)
            wAll+=pTable->columnWidth(i);//really column width(all) (100%)

        for(x=0,i=0;i<pTable->columnCount();i++)
        {
            j=(int) ((pTable->columnWidth(i)*100 )/wAll);// %
            z=((fillRect.right()-fillRect.left())/100)*j;// calc % of printer (px)
            lsColumnsWidth.push_back(z); //mark
            x+=z;//mark all
        }

        //rounding errors
        j=fillRect.right()-fillRect.left();
        if(x<j && lsColumnsWidth.count()>1)
        {
            z=(j-x)/pTable->columnCount();
            for(x=0,i=0;x<pTable->columnCount();x++)
            {
                lsColumnsWidth[x]+=z;
                i+=lsColumnsWidth[x];
            }
            if(i<j)
               lsColumnsWidth[lsColumnsWidth.count()-1]+=(j-i);
        }



        //copies
        while(copies<copies_count && pTable->columnCount()>0)
        {
            row=0;
            page=1;

            //first row(title)
            if(m_sFirstRow.length()>0)
            {
                rF=QRectF(fillRect.left(),fillRect.top(),fillRect.right(),fillRect.top()+row_space);
                draw_text(&painter,&font,rF,m_sFirstRow,true,true,false);
            }

            //date+time
            rF=QRectF(fillRect.right()-get_text_width(sDateTime,&metric),fillRect.top()+(row*row_space),fillRect.right(),fillRect.top()+((row+1)*row_space));
            draw_text(&painter,&font,rF,sDateTime,false,false,false);//normal
            row+=2;//+empty row

            //list(data from table)
            if(pTableInfo!=NULL)
            {
                count=pTableInfo->columnCount();
                ls=pTableInfo->selectedItems();
                for(int mark=0,i=0;i<count-iDiff;row++,mark++)
                {
                    //is item in table empty?? -> continue
                    if(i<ls.count())
                    {
                        if(ls[i]!=NULL)
                        {
                            s=ls[i]->text();
                            if(s.length()<=0)
                            {
                                i++;
                                row--;
                                mark--;
                                continue;
                            }
                        }
                    }
                    //-
                    rF=QRectF(fillRect.left(),fillRect.top()+(row*row_space),fillRect.right()-fillRect.left(),fillRect.top()+((row+1)*row_space));
                    if(fillRect.top()+((row+1)*row_space)>fillRect.bottom())
                    {
                        m_pPrinter->newPage();
                        row=-1;
                        continue;
                    }
                    if(mark%2==0)
                    {
                        pItem=pTableInfo->horizontalHeaderItem(i);//get header
                        if(pItem!=NULL)
                        {
                            s=pItem->text();
                            s+=QString(":");
                            draw_text(&painter,&font,rF,s,true,false,true);//italic+bold
                        }
                    }
                    if(mark%2!=0)
                    {
                        if(i<ls.count())//get seleted row data
                        {
                            pItem=ls[i];
                            if(pItem!=NULL)
                            {
                                s=pItem->text();
                                draw_text(&painter,&font,rF,s,false,false,false);//normal
                            }
                        }
                        i++;
                        row++;
                    }
                }
                ls.clear();
            }

            //list(listwidget)
            if(pList!=NULL)
            {
                count=pList->count();
                for(i=0;i<count;i++,row++)
                {
                    if(fillRect.top()+((row+1)*row_space)>fillRect.bottom())
                    {
                        m_pPrinter->newPage();
                        row=-1;
                        i--;
                        continue;
                    }
                    //-
                    rF=QRectF(fillRect.left(),fillRect.top()+(row*row_space),fillRect.right()-fillRect.left(),fillRect.top()+((row+1)*row_space));
                    pListItem=pList->item(i);
                    if(pListItem!=NULL)
                    {
                        s=pListItem->text();
                        if(s.length()>0)//empty row?
                        {
                            if(pListItem->font().bold())
                                draw_text(&painter,&font,rF,s,true,false,true);//bold+ital.
                            else
                            {
                                draw_text(&painter,&font,rF,s,false,false,false);//normal
                                row++;//empty row
                            }
                        }
                    }
                }
            }

            //empty row
            row++;

            //items
            count=pTable->rowCount();
            for(i=0;i<count;row++,i++)
            {
                if(bFirst)//header?
                {
                    rF=QRectF(fillRect.left(),fillRect.top()+(row*row_space),fillRect.right()-fillRect.left(),row_space);
                    painter.fillRect(rF,QColor(200,200,200));
                }
                else if(i%2!=0)//table items - all 2. row light grey
                {
                    rF=QRectF(fillRect.left(),fillRect.top()+(row*row_space),fillRect.right()-fillRect.left(),row_space);
                    painter.fillRect(rF,QColor(235,235,235));
                }

                for(l=0,x=0;x<lsColumnsWidth.count();x++)//last column is id(not print)
                {
                    //rect
                    rF=QRectF(fillRect.left()+l,fillRect.top()+(row*row_space),lsColumnsWidth[x],row_space);
                    painter.drawRect(rF);

                    //get text
                    if(bFirst)//header?
                    {
                        pItem=pTable->horizontalHeaderItem(x);
                        if(pItem!=NULL)
                            s=pItem->text();
                        else{}
                    }
                    else
                    {
                        pItem=pTable->item(i,x);
                        if(pItem!=NULL)
                            s=pItem->text();
                    }

                    //get dist. by aglinment
                    dx=distTopLeftCorner;//left=fefault
                    if(pItem!=NULL && bFirst==false)
                    {
                        if(lsColumnsWidth[x] > get_text_width(s,&metric))
                        {
                            if(pItem->textAlignment()==iAliRight)//right
                                dx=lsColumnsWidth[x]-(get_text_width(s,&metric)+(2*distTopLeftCorner));
                            else if(pItem->textAlignment()==iAliCenter)//center
                                dx=(lsColumnsWidth[x]-(get_text_width(s,&metric)+(2*distTopLeftCorner)))/2;
                            //-
                            if(dx<distTopLeftCorner)
                                dx=distTopLeftCorner;
                        }
                    }
                    pItem=NULL;

                    //calc draw rect
                    rF=QRectF(fillRect.left()+l+dx,fillRect.top()+(row*row_space)+distTopLeftCorner,lsColumnsWidth[x]-(2*distTopLeftCorner),row_space-(2*distTopLeftCorner));

                    //draw text
                    if(bFirst)//header?
                        draw_text(&painter,&font,rF,s,true,false,false);
                    else
                        draw_text(&painter,&font,rF,s,false,false,false);
                    l+=lsColumnsWidth[x];
                }
                if(bFirst)
                {
                    bFirst=false;
                    i--;
                }
                //-
                if(i+1==count || fillRect.top()+((row+3)*row_space)>fillRect.bottom())//last row on page? ->print page count
                {
                    //print page / column count(center)
                    s=QString("Seite %1").arg(page);
                    rF=QRectF(fillRect.left()+((fillRect.right()-fillRect.left()-metric.width(s))/2),fillRect.bottom()-(row_space-(2*distTopLeftCorner)),fillRect.right()-fillRect.left(),row_space);
                    draw_text(&painter,&font,rF,s,false,false,false);
                    page++;
                }
                //-
                if(fillRect.top()+((row+3)*row_space)>fillRect.bottom())//new page?
                {
                    m_pPrinter->newPage();
                    bFirst=true;
                    row=-1;
                }
            }
            //-
            copies++;
            if(copies<copies_count)
            {
                m_pPrinter->newPage();
                bFirst=true;
            }
        }
    }
    lsColumnsWidth.clear();
    //-
    return b;
}

bool CPrintJob::print_tree(void)
{
    if(m_pPrinter==NULL)
        return false;

    bool b=false;
    QRectF rF;
    QString s,s2;
    int i,row=0,row_space,copies_count,copies=0;
    QFont font("Sans",8,0,0);
    qreal marginLeft,marginTop,marginRight,marginBottom;
    QRect paper = m_pPrinter->pageRect();
    QTreeWidget * pTree=m_memory.get_treewidget();
    QTreeWidgetItem * pItem=NULL;

    //date+time
    QDateTime dtTi=QDateTime::currentDateTime();
    QString sDateTime=QString(" (erstellt %1 Uhr, %2)").arg(dtTi.toString("hh:mm:ss"),dtTi.toString("dd.MM.yyyy"));
    //-
    if(m_pPrinter!=NULL && pTree!=NULL)
    {
        b=true;

        //copy count
        copies_count=m_pPrinter->numCopies();

        //painter
        QPainter painter(m_pPrinter);
        painter.setPen(Qt::black);

        //margin
        m_pPrinter->getPageMargins(&marginLeft,&marginTop,&marginRight,&marginBottom,QPrinter::DevicePixel);
        QRect fillRect=QRect(marginLeft,marginTop,(paper.right()-paper.left())-((int)marginRight+marginLeft),(paper.bottom()-paper.top())-(((int)marginBottom+marginTop)));

        //font
        painter.setFont(font);
        QFontMetrics metric(painter.fontMetrics());
        row_space=metric.height();

        //copies
        while(copies<copies_count)
        {
            row=0;

            //first row(title)
            if(m_sFirstRow.length()>0)
            {
                rF=QRectF(fillRect.left(),fillRect.top(),fillRect.right(),fillRect.top()+row_space);
                draw_text(&painter,&font,rF,m_sFirstRow,true,true,false);
            }

            //date+time
            rF=QRectF(fillRect.right()-get_text_width(sDateTime,&metric),fillRect.top()+(row*row_space),fillRect.right(),fillRect.top()+((row+1)*row_space));
            draw_text(&painter,&font,rF,sDateTime,false,false,false);//normal
            row+=2;//+empty row

            for(i=0;i<pTree->topLevelItemCount();i++)
            {
                //top item
                rF=QRectF(fillRect.left(),fillRect.top()+(row*row_space),fillRect.right()-fillRect.left(),fillRect.top()+((row+1)*row_space));
                if(fillRect.top()+((row+1)*row_space)>fillRect.bottom())
                {
                    m_pPrinter->newPage();
                    row=0;
                    i--;
                    continue;
                }
                pItem=pTree->topLevelItem(i);
                if(pItem!=NULL)
                {
                    s=pItem->text(0);
                    s2=pItem->text(1);
                    if(s2.length()>0)
                        s+=QString(" (%1)").arg(s2);
                    draw_text(&painter,&font,rF,s,false,false,false);
                    row++;

                    //cildren
                    if(pItem->childCount()>0)//cildren?
                        print_tree_cildren(pItem,row,row_space,fillRect,&painter,font);//print cildren
                }
            }
            //-
            copies++;
            if(copies<copies_count)
                m_pPrinter->newPage();
        }
    }
    //-
    return b;
}

bool CPrintJob::print_tree_cildren(QTreeWidgetItem * pParent, int & iRow, int iRowSpace, QRect fillRect, QPainter * pPainter, QFont & font)
{
    bool b=false;
    int i=0,mark=iRow;
    QString s,s2;
    QTreeWidgetItem * pItem=NULL;
    QRectF rF;
    QPoint pt,pt2;
    //-
    if(pParent!=NULL && iRow>=0 && iRowSpace>0 && pPainter!=NULL)
    {
        b=true;

        //tab
        fillRect=QRect(fillRect.left()+iRowSpace,fillRect.top(),fillRect.right()-iRowSpace,fillRect.bottom());

        //item(cildren)
        for(i=0;i<pParent->childCount();i++)
        {
            rF=QRectF(fillRect.left(),fillRect.top()+(iRow*iRowSpace),fillRect.right()-fillRect.left(),fillRect.top()+((iRow+1)*iRowSpace));
            if(fillRect.top()+((iRow+1)*iRowSpace)>fillRect.bottom())
            {
                m_pPrinter->newPage();
                iRow=mark=0;
                i--;
                continue;
            }
            pItem=pParent->child(i);
            if(pItem!=NULL)
            {
                //text
                s=pItem->text(0);
                s2=pItem->text(1);
                if(s2.length()>0)
                    s+=QString(" (%1)").arg(s2);
                draw_text(pPainter,&font,rF,s,false,false,false);

                //line(horz)
                pt=QPoint(rF.left()-iRowSpace+(iRowSpace/5),rF.top()+(iRowSpace/2));
                pt2=QPoint(rF.left()-(iRowSpace/5),rF.top()+(iRowSpace/2));
                pPainter->drawLine(pt,pt2);

                //next row
                iRow++;

                //line(vert)
                pPainter->drawLine(fillRect.left()-iRowSpace+(iRowSpace/5),fillRect.top()+(mark*iRowSpace),fillRect.left()-iRowSpace+(iRowSpace/5),fillRect.top()+(iRow*iRowSpace)-(iRowSpace/2));

                //cildren of cildren
                if(pItem->childCount()>0)//cildren?
                    print_tree_cildren(pItem,iRow,iRowSpace,fillRect,pPainter,font);
            }
        }
    }
    return b;
}

