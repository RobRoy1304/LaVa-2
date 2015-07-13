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

#include "cprintjob.h"


//print job------------------------------------------------------------------------------------------------------
CPrintJob::CPrintJob()
{
    m_iType=-1;
    m_pPrinter=NULL;
    m_pThread=NULL;
}

CPrintJob::~CPrintJob()
{
    m_memory.clear();
}

int CPrintJob::get_type(void)
{
    return m_iType;
}

void CPrintJob::set_type(int iType)
{
    m_iType=iType;
}

bool CPrintJob::check_text_space(QString & sText, QFontMetrics * pMetric, QRectF rRect)
{
    bool b=false;
    QString s=sText;
    QString sPoints=QString::fromUtf8("...");
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
            sText+=QString::fromUtf8("...");
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
        font.setBold(bold);
        font.setUnderline(underline);
        font.setItalic(italic);
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
    QString sDateTime=QString::fromUtf8(" (erstellt %1 Uhr, %2)").arg(dtTi.toString("hh:mm:ss"),dtTi.toString("dd.MM.yyyy"));

    bool b=false;
    QRectF rF;
    QString s;
    int i,row=0,count,row_space,copies_count,page,copies=0;
    QFont font("Sans",8,0,0);
    QRect paper = m_pPrinter->pageRect();
    QListWidget * pList=m_memory.get_listwidget();
    QListWidgetItem * pItem=NULL;
    if(m_pPrinter!=NULL && pList!=NULL)
    {
        b=true;

        //copy count
        copies_count=m_pPrinter->copyCount();

        //painter
        QPainter painter(m_pPrinter);
        painter.setPen(Qt::black);

        //fillRect
        QRect fillRect=QRect(0,0,paper.right()-paper.left(),paper.bottom()-paper.top());

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
                    s=QString::fromUtf8("Seite %1").arg(page);
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
    bool bFirst=true,bPrintAllPages=true;
    int wAll,i,j,z,x,l,row=0,count,row_space,distTopLeftCorner,page,copies_count,copies=0,dx,iAliRight=Qt::AlignRight|Qt::AlignVCenter,iAliCenter=Qt::AlignHCenter|Qt::AlignVCenter;
    int iFromPage=0,iToPage=0,iStartRow=0,iFinishRow=-1,iMaxRowPerPage=0,iMaxRowPerFirstPage=0;
    QFont font("Sans",8,0,0);
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
    QString sDateTime=QString::fromUtf8(" (erstellt %1 Uhr, %2)").arg(dtTi.toString("hh:mm:ss"),dtTi.toString("dd.MM.yyyy"));

    if(m_pPrinter!=NULL && pTable!=NULL)
    {
        b=true;

        //copy count
        copies_count=m_pPrinter->copyCount();

        //painter
        QPainter painter(m_pPrinter);
        painter.setPen(Qt::black);

        //fillRect
        QRect fillRect=QRect(0,0,paper.right()-paper.left(),paper.bottom()-paper.top());

        //font
        painter.setFont(font);
        QFontMetrics metric(painter.fontMetrics());
        distTopLeftCorner=metric.height()/4;
        row_space=metric.height()+(2*distTopLeftCorner);

        //page count + find start & finish row from table by start & end page, if not all pages print
        iFromPage=m_pPrinter->fromPage();
        iToPage=m_pPrinter->toPage();
        if(iToPage<iFromPage)
        {
            i=iFromPage;
            iFromPage=iToPage;
            iToPage=i;
        }
        if(iFromPage==0 && iToPage==0)
        {// from & to page unset
            iFromPage++;
            iToPage++;
        }
        else
        {
            bPrintAllPages=false;

            //rows per page
            for(iMaxRowPerPage=0;fillRect.top()+((iMaxRowPerPage+3)*row_space)<=fillRect.bottom();iMaxRowPerPage++){}
            iMaxRowPerFirstPage=iMaxRowPerPage-2;

            //calc start & finish row
            if(iFromPage>1)
            {
                iStartRow+=iMaxRowPerFirstPage;
                if(iFromPage>2)
                    iStartRow+=(iFromPage-2)*iMaxRowPerPage;
            }
            if(iToPage>0)
            {
                iFinishRow=iStartRow;
                i=iToPage-iFromPage+1;
                if(iFromPage==1)
                {
                    iFinishRow+=iMaxRowPerFirstPage;
                    i--;
                }
                if(i>0)
                    iFinishRow+=i*iMaxRowPerPage;
                iFinishRow-=1;
            }
        }

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
            for(x=lsColumnsWidth.count()-(1+iDiff);x>=0 && x<pTable->columnCount();x--)//search last visible column
            {
                if(lsColumnsWidth[x]>0 && i<j)
                {
                    lsColumnsWidth[x]+=j-i;//add the rest
                    break;
                }

                if(lsColumnsWidth[x]>0 && i>j)
                {
                    lsColumnsWidth[x]-=i-j;//- the rest
                    break;
                }
            }
        }


        //copies
        while(copies<copies_count && pTable->columnCount()>0)
        {
            row=0;
            page=iFromPage;

            //first row(title)
            if(m_sFirstRow.length()>0 && page==1)
            {
                rF=QRectF(fillRect.left(),fillRect.top(),fillRect.right(),fillRect.top()+row_space);
                draw_text(&painter,&font,rF,m_sFirstRow,true,true,false);
            }

            //date+time
            if(page==1)
            {
                rF=QRectF(fillRect.right()-get_text_width(sDateTime,&metric),fillRect.top()+(row*row_space),fillRect.right(),fillRect.top()+((row+1)*row_space));
                draw_text(&painter,&font,rF,sDateTime,false,false,false);//normal
                row+=2;//+empty row
            }

            //items
            count=pTable->rowCount();
            if(iFinishRow==-1)// not set
                iFinishRow=count;
            for(i=iStartRow;i<count && i<=iFinishRow;row++,i++)
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
                            else
                                s=QString::fromUtf8("");
                        }
                        else
                        {
                            pItem=pTable->item(i,x);
                            if(pItem!=NULL)
                                s=pItem->text();
                            else
                                s=QString::fromUtf8("");
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
                    s=QString::fromUtf8("Seite %1").arg(page);
                    rF=QRectF(fillRect.left()+((fillRect.right()-fillRect.left()-metric.width(s))/2),fillRect.bottom()-(row_space-(2*distTopLeftCorner)),fillRect.right()-fillRect.left(),row_space);
                    draw_text(&painter,&font,rF,s,false,false,false);
                    page++;
                }

                if(fillRect.top()+((row+3)*row_space)>fillRect.bottom())//new page?
                {
                    if(page<=iToPage || bPrintAllPages==true)
                    {
                        //next page
                        m_pPrinter->newPage();
                        bFirst=true;
                        row=-1;
                    }
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
    QString sDateTime=QString::fromUtf8(" (erstellt %1 Uhr, %2)").arg(dtTi.toString("hh:mm:ss"),dtTi.toString("dd.MM.yyyy"));

    if(m_pPrinter!=NULL && pTable!=NULL)
    {
        b=true;

        //copy count
        copies_count=m_pPrinter->copyCount();

        //painter
        QPainter painter(m_pPrinter);
        painter.setPen(Qt::black);

        //fillRect
        QRect fillRect=QRect(0,0,paper.right()-paper.left(),paper.bottom()-paper.top());

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
                            s+=QString::fromUtf8(":");
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
                        else
                            s=QString::fromUtf8("");
                    }
                    else
                    {
                        pItem=pTable->item(i,x);
                        if(pItem!=NULL)
                            s=pItem->text();
                        else
                            s=QString::fromUtf8("");
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
                    s=QString::fromUtf8("Seite %1").arg(page);
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
    QRect paper = m_pPrinter->pageRect();
    QTreeWidget * pTree=m_memory.get_treewidget();
    QTreeWidgetItem * pItem=NULL;

    //date+time
    QDateTime dtTi=QDateTime::currentDateTime();
    QString sDateTime=QString::fromUtf8(" (erstellt %1 Uhr, %2)").arg(dtTi.toString("hh:mm:ss"),dtTi.toString("dd.MM.yyyy"));
    //-
    if(m_pPrinter!=NULL && pTree!=NULL)
    {
        b=true;

        //copy count
        copies_count=m_pPrinter->copyCount();

        //painter
        QPainter painter(m_pPrinter);
        painter.setPen(Qt::black);

        //fillRect
        QRect fillRect=QRect(0,0,paper.right()-paper.left(),paper.bottom()-paper.top());

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
                        s+=QString::fromUtf8(" (%1)").arg(s2);
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
                    s+=QString::fromUtf8(" (%1)").arg(s2);
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

bool CPrintJob::print_barcode_overview(void)
{
    CEan ean;
    QRectF rF;
    CArticle ar;
    QRect fillRect,paper;
    QList<int> lsArticleIds;
    CPointerMemory memory;
    QFont font("Sans",8,0,0);
    QList<int> lsColumnWidth;
    bool bFirst=true,bPrintAll=true;
    QDateTime dtTi=QDateTime::currentDateTime();
    QString s,sArticlenumber1,sArticlenumber2;
    int sum_column_w=0,w,i,row,row_space,page,copies_count,copies,index,column;
    int iFromPage=0,iToPage=0,iStartIndex=0,iFinishIndex=-1,iMaxRowPerPage=0;

    //header
    QStringList lsHeader;
    lsHeader.push_back("Artikelinformation");
    lsHeader.push_back("Barcode(Artikelnr.1)");
    lsHeader.push_back("Barcode(Artikelnr.2)");

    //painter
    QPainter painter(m_pPrinter);
    painter.setPen(Qt::black);

    //line height(barcode)
    int line_height=get_barcode_height(&painter,TYPE_EAN_13);
    line_height*=1.25;

    //-
    if(m_pPrinter!=NULL && m_pThread!=NULL && m_pThread->m_pDbInterface!=NULL)
    {
        //paper rect
        paper = m_pPrinter->pageRect();

        //font
        painter.setFont(font);
        QFontMetrics metric(painter.fontMetrics());
        row_space=metric.height()+(2*(metric.height()/4));

        //copy count
        copies_count=m_pPrinter->copyCount();

        //fillRect
        fillRect=QRect(0,0,paper.right()-paper.left(),paper.bottom()-paper.top());

        //page count of rows
        iFromPage=m_pPrinter->fromPage();
        iToPage=m_pPrinter->toPage();
        if(iToPage<iFromPage)
        {
            i=iFromPage;
            iFromPage=iToPage;
            iToPage=i;
        }
        if(iFromPage==0 && iToPage==0)
        {// from & to page unset
            iFromPage++;
            iToPage++;
        }
        else
        {
            bPrintAll=false;

            //rows per page
            for(iMaxRowPerPage=0;fillRect.top()+(3*row_space)+((iMaxRowPerPage+1)*line_height)<=fillRect.bottom();iMaxRowPerPage++){};

            //calc start & finish row
            if(iFromPage>1)
                iStartIndex+=(iFromPage-1)*iMaxRowPerPage;
            if(iToPage>0)
            {
                iFinishIndex=iStartIndex;
                i=iToPage-iFromPage+1;
                if(i>0)
                    iFinishIndex+=i*iMaxRowPerPage;
                iFinishIndex-=1;
            }
        }

        //column-width's
        lsColumnWidth.push_back(line_height*1.3);
        lsColumnWidth.push_back(line_height*1.3);
        lsColumnWidth.push_front(fillRect.width()-(2*line_height*1.3));

        for(i=0;i<lsColumnWidth.count();i++)//mark sum
            {sum_column_w+=lsColumnWidth[i];}

        //get all article (id) with barcode
        memory.set_int_list(&lsArticleIds);
        if(m_pThread->set_work(WORK_ARTICLE_GET_ALL_WITH_BARCODE,&memory))//by thread
        {
            //copies
            for(copies=0;copies<copies_count;copies++)
            {
                if(copies>0)//more copies
                    m_pPrinter->newPage();

                //rows---
                if(iFinishIndex==-1)
                    iFinishIndex=lsArticleIds.count()-1;//to page not set -> print all index's

                for(index=iStartIndex,row=0,page=iFromPage,bFirst=true;index<lsArticleIds.count() && index <=iFinishIndex;index++)
                {
                    //first?
                    if(bFirst)
                    {
                        bFirst=false;

                        if(page==1)
                        {
                            //title
                            rF=QRectF(fillRect.left(),fillRect.top(),fillRect.right(),row_space);
                            draw_text(&painter,&font,rF,QString::fromUtf8("Artikelübersicht-Barcode"),true,true,false);

                            //date+time
                            s=QString::fromUtf8(" (erstellt %1 Uhr, %2)").arg(dtTi.toString("hh:mm:ss"),dtTi.toString("dd.MM.yyyy"));
                            rF=QRectF(fillRect.right()-get_text_width(s,&metric),fillRect.top(),fillRect.right(),row_space);
                            draw_text(&painter,&font,rF,s,false,false,false);//normal
                        }

                        //page-count
                        s=QString::fromUtf8("Seite %1").arg(page);
                        rF=QRectF(fillRect.left()+((fillRect.right()-fillRect.left()-metric.width(s))/2),fillRect.top(),fillRect.right()-fillRect.left(),row_space);
                        draw_text(&painter,&font,rF,s,false,false,false);

                        //header
                        rF=QRectF(fillRect.left(),fillRect.top()+(2*row_space),sum_column_w,row_space);//dark grey rect
                        painter.fillRect(rF,QColor(200,200,200));
                        for(i=0,w=0;i<lsHeader.count() && i<lsColumnWidth.count();i++)
                        {
                            //add column width before
                            if(i>0 && i<lsColumnWidth.count())
                                w+=lsColumnWidth[i-1];

                            //border rect
                            rF=QRectF(fillRect.left()+w,fillRect.top()+(2*row_space),lsColumnWidth[i],row_space);
                            painter.drawRect(rF);

                            //text
                            rF=QRectF(fillRect.left()+w+(metric.height()/4),fillRect.top()+(2*row_space),lsColumnWidth[i],row_space);
                            draw_text(&painter,&font,rF,lsHeader[i],true,false,false);
                        }
                    }

                    //columns---
                    for(column=0,w=0;column<lsHeader.count() && column<lsColumnWidth.count();column++)
                    {
                        //add column width before
                        if(column>0 && column<lsColumnWidth.count())
                             w+=lsColumnWidth[column-1];

                        //draw border rect
                        rF=QRectF(fillRect.left()+w,fillRect.top()+(3*row_space)+(row*line_height),lsColumnWidth[column],line_height);
                        painter.drawRect(rF);

                        //clear + get article
                        ar.clear();
                        sArticlenumber1=sArticlenumber2=QString("");//clear
                        if(m_pThread->m_pDbInterface->article_get(lsArticleIds[index],ar))
                        {
                            sArticlenumber1=ar.get_articlenumber();
                            sArticlenumber2=ar.get_articlenumber2();
                        }

                        //article-info , text
                        if(column==0)
                        {
                            //name
                            rF=QRectF(fillRect.left()+w+(metric.height()/4),fillRect.top()+(3*row_space)+(row*line_height),lsColumnWidth[column],row_space);
                            s=QString::fromUtf8("Artikelbez: ")+ar.get_name();
                            draw_text(&painter,&font,rF,s,false,false,false);

                            //maker
                            rF=QRectF(fillRect.left()+w+(metric.height()/4),fillRect.top()+(4*row_space)+(row*line_height),lsColumnWidth[column],row_space);
                            s=QString::fromUtf8("Hersteller: ")+m_pThread->m_pDbInterface->maker_get_name(ar.get_maker_id());
                            draw_text(&painter,&font,rF,s,false,false,false);

                            //waregroup
                            rF=QRectF(fillRect.left()+w+(metric.height()/4),fillRect.top()+(5*row_space)+(row*line_height),lsColumnWidth[column],row_space);
                            s=QString::fromUtf8("Warengruppe: ")+m_pThread->m_pDbInterface->waregroup_get_path(ar.get_waregroup_id());
                            draw_text(&painter,&font,rF,s,false,false,false);
                        }

                        //barcode's
                        if(column>0)
                        {
                            if(column==1)
                                ean.set_barcode(sArticlenumber1);//set 1.articlenumber
                            if(column==2)//2.articlenumber
                                ean.set_barcode(sArticlenumber2);//set 2.articlenumber

                            //draw
                            draw_barcode(ean,&painter,QPoint(fillRect.left()+w,fillRect.top()+(3*row_space)+(row*line_height)));
                        }
                    }
                    //row
                    row++;

                    //next page?
                    if(page+1 <= iToPage || bPrintAll==true)
                    {
                        if(fillRect.top()+(3*row_space)+((row+1)*line_height) > fillRect.bottom())
                        {
                            page++;
                            row=0;
                            bFirst=true;
                            m_pPrinter->newPage();
                        }
                    }
                }
            }
        }
    }
    //-
    lsColumnWidth.clear();
    lsArticleIds.clear();
    lsHeader.clear();
    //-
    return true;
}

bool CPrintJob::draw_barcode(CEan & ean, QPainter * pPainter, QPoint ptPos)
{
    bool bLong;
    QRect r,rTarget;
    QString sDigiStruct;
    int i=0,sep_pos,index;
    qreal x_proportion_px_mm, y_proportion_px_mm,line_width,line_height=0,width=0,height=0,margin;
    QFont font("Sans",8,0,0);

    QBrush brushWhite;
    brushWhite.setStyle(Qt::SolidPattern);
    brushWhite.setColor(Qt::white);
    QBrush brushBlack;
    brushBlack.setStyle(Qt::SolidPattern);
    brushBlack.setColor(Qt::black);
    //-
    if(m_pPrinter!=NULL && pPainter!=NULL && ean.is_ean_barcode())
    {
        //proportion pixel / mm
        x_proportion_px_mm=m_pPrinter->logicalDpiX() / 25.4;
        y_proportion_px_mm=m_pPrinter->logicalDpiY() / 25.4;

        //barcode margin (better read)
        margin=((x_proportion_px_mm+y_proportion_px_mm)/2) * 4;

        //font
        pPainter->setFont(font);

        //norminal minimal
        if(ean.get_type()==TYPE_EAN_8)
        {
            width=x_proportion_px_mm * 22 + (2*margin);
            height=y_proportion_px_mm * 18 + (2*margin);
        }
        else if(ean.get_type()==TYPE_EAN_13)
        {
            width=x_proportion_px_mm * 31 + (2*margin);
            height=y_proportion_px_mm * 22 + (2*margin);
        }

        //barcode line width
        if(ean.get_type()==TYPE_EAN_8)
            i=67;//count of lines
        else if(ean.get_type()==TYPE_EAN_13)
            i=95;//count of lines
        line_width= (width-(2*margin)) / i;
        if(line_width<1)
        {//line_width min. 1 pixel
            line_width=1;
            width=2*margin+(line_width*i);
        }

        //barcode rect
        rTarget=QRect(ptPos.x()+margin,ptPos.y()+margin,width,height);

        //digital struct of barcode
        if(ean.get_digital_struct(sDigiStruct))
        {
            //seperator pos
            sep_pos=(int) (sDigiStruct.length()-5)/2;//pos before seperator

            //fill rect white (underground)
            pPainter->fillRect(rTarget,brushWhite);

            //draw line's
            for(index=0;index<sDigiStruct.length();index++)
            {
                if(sDigiStruct.mid(index,1)==QString::fromUtf8("1"))
                {
                    //line height , if lead tail or seperator -> line longer
                    bLong=false;
                    if(index<3 || index>=sDigiStruct.length()-3)//left or right lead tail
                        bLong=true;
                    if(index>=sep_pos && index<=sep_pos+4)// or seperator
                        bLong=true;

                    if(bLong)
                        line_height=rTarget.height()-(2*margin);//line long
                    else
                        line_height=rTarget.height()-(2.6*margin);//line short

                    //border
                    pPainter->drawRect(rTarget);

                    //draw line
                    r=QRect(rTarget.x() + (index*line_width) + margin, rTarget.y() + margin, line_width, line_height);
                    pPainter->fillRect(r,brushBlack);
                }
            }

            //draw text barcode-----------------------
            if(ean.get_type()==TYPE_EAN_8)
            {
                //left+right part of barcode
                pPainter->drawText(QPointF(rTarget.x()+(margin*1.5)+(3*line_width),rTarget.y()+line_height+(1.25*margin)),ean.get_barcode_number_string(0,4));
                pPainter->drawText(QPointF(rTarget.x()+(margin*1.5)+(36*line_width),rTarget.y()+line_height+(1.25*margin)),ean.get_barcode_number_string(4,4));
            }
            else if(ean.get_type()==TYPE_EAN_13)
            {
                //country code
                pPainter->drawText(QPointF(rTarget.x()+(margin/4),rTarget.y()+line_height+(1.25*margin)),ean.get_barcode_number_string(0,1));
                //left+right part of barcode
                pPainter->drawText(QPointF(rTarget.x()+(margin*2)+(3*line_width),rTarget.y()+line_height+(1.25*margin)),ean.get_barcode_number_string(1,6));
                pPainter->drawText(QPointF(rTarget.x()+(margin*2)+(50*line_width),rTarget.y()+line_height+(1.25*margin)),ean.get_barcode_number_string(7,6));
            }
        }
    }
    return true;
}

qreal CPrintJob::get_barcode_height(CEan & ean, QPainter * pPainter)
{
    return get_barcode_height(pPainter,ean.get_type());
}

qreal CPrintJob::get_barcode_height(QPainter * pPainter, int type_ean)
{
    qreal x_proportion_px_mm, y_proportion_px_mm,margin,height=-1;
    //-
    if(m_pPrinter!=NULL && pPainter!=NULL)
    {
        x_proportion_px_mm=m_pPrinter->logicalDpiX() / 25.4;
        y_proportion_px_mm=m_pPrinter->logicalDpiY() / 25.4;
        margin=((x_proportion_px_mm+y_proportion_px_mm)/2) * 4;
        if(type_ean==TYPE_EAN_8)
            height=y_proportion_px_mm * 18 + (2*margin);
        else if(type_ean==TYPE_EAN_13)
            height=y_proportion_px_mm * 22 + (2*margin);
    }
    //-
    return height;
}
