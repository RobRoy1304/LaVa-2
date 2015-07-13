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

#include "cinputdialogtablesetting.h"
#include "ui_cinputdialogtablesetting.h"

CInputDialogTableSetting::CInputDialogTableSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogTableSetting)
{
    ui->setupUi(this);

    //disable auto default buttons
    ui->pushButtonAligCenter->setAutoDefault(false);
    ui->pushButtonAligLeft->setAutoDefault(false);
    ui->pushButtonAligRight->setAutoDefault(false);
    ui->pushButtonDown->setAutoDefault(false);
    ui->pushButtonUp->setAutoDefault(false);
    ui->pushButtonVisible->setAutoDefault(false);

    //disable sorting
    ui->tableWidget->setSortingEnabled(false);

    //set selection mode
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    //set not editable
    ui->tableWidget->setEditTriggers(NULL);

    //set columns width
    int i,column_width=(ui->tableWidget->width()/ui->tableWidget->columnCount())-20;
    for(i=0;i<ui->tableWidget->columnCount();i++)
        ui->tableWidget->setColumnWidth(i,column_width);

    check_user_input();

    //connects
    connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),SLOT(check_user_input()));
    connect(ui->pushButtonUp,SIGNAL(clicked()),SLOT(button_up()));
    connect(ui->pushButtonDown,SIGNAL(clicked()),SLOT(button_down()));
    connect(ui->pushButtonAligLeft,SIGNAL(clicked()),SLOT(button_left()));
    connect(ui->pushButtonAligCenter,SIGNAL(clicked()),SLOT(button_center()));
    connect(ui->pushButtonAligRight,SIGNAL(clicked()),SLOT(button_right()));
    connect(ui->pushButtonVisible,SIGNAL(clicked()),SLOT(button_visible()));
}

CInputDialogTableSetting::~CInputDialogTableSetting()
{
    delete ui;
}

bool CInputDialogTableSetting::check_user_input(void)
{
    int i,row=-1;
    QString s;
    bool bUp=true,bDown=true,bLeft=true,bRight=true,bCenter=true,bVisible=true;
    QList<QTableWidgetItem*> ls=ui->tableWidget->selectedItems();
    if(ls.count()<=0)
        bUp=bDown=bLeft=bRight=bCenter=bVisible=false;//nothing selected -> all disable
    else
    {
        if(ls.count()>0)
        {
            row=ls[0]->row();
            if(row==0 || row-1==m_tcdNewSetting.get_id_column())//selected first row or next id column?
                bUp=false;
            if(row==ui->tableWidget->rowCount()-1 || row+1==m_tcdNewSetting.get_id_column())//selected last row? or next id column
                bDown=false;
            if(row==m_tcdNewSetting.get_id_column())//if selected row = id column -> not move
                bUp=bDown=false;
        }
        //-
        if(ls.count()>1)
        {
            i=m_tcdNewSetting.get_column_alignment(row);
            if(i==TABLE_ALIGNMENT_LEFT)
                bLeft=false;
            else if(i==TABLE_ALIGNMENT_RIGHT)
                bRight=false;
            else
                bCenter=false;
        }
        //-
        if(ls.count()>2)
        {
            if(m_tcdNewSetting.get_column_visible(row)==false)
                ui->pushButtonVisible->setText(QString::fromUtf8("sichtbar"));
            else
            {
                ui->pushButtonVisible->setText(QString::fromUtf8("nicht sichtbar"));
                //min. 2 column must visible
                if(m_tcdNewSetting.get_table()!=NULL)
                {
                    if(m_tcdNewSetting.get_count_not_visible_columns()+2 >= m_tcdNewSetting.get_table()->columnCount())
                        bVisible=false;
                }
            }
        }
    }
    //-
    ui->pushButtonAligCenter->setEnabled(bCenter);
    ui->pushButtonAligLeft->setEnabled(bLeft);
    ui->pushButtonAligRight->setEnabled(bRight);
    ui->pushButtonDown->setEnabled(bDown);
    ui->pushButtonUp->setEnabled(bUp);
    ui->pushButtonVisible->setEnabled(bVisible);
    //-
    ls.clear();
    return true;
}

bool CInputDialogTableSetting::set_data(CTableColumnsData & tcdData)
{
    m_tcdNewSetting=tcdData;
    fill_table();
    check_user_input();
    return true;
}

bool CInputDialogTableSetting::get_data(CTableColumnsData & tcdData, QList<QString> & lsHeaderNames)
{
    //get columns data
    tcdData=m_tcdNewSetting;

    //get new sort header names
    QString s;
    lsHeaderNames.clear();
    for(int i=0;i<ui->tableWidget->rowCount();i++)
    {
        s=ui->tableWidget->item(i,0)->text();
        lsHeaderNames.push_back(s);
    }
    return true;
}

bool CInputDialogTableSetting::fill_table(void)
{
    bool vis,bReturn=false;
    int i,j,row=0,ali;
    QString s;
    QTableWidget * pTable=m_tcdNewSetting.get_table();
    QTableWidgetItem * pItem;

    if(pTable!=NULL)
    {
        if(pTable->columnCount()==m_tcdNewSetting.get_columns_count())
        {
            for(i=0;i<pTable->columnCount();i++,row++)
            {
                ui->tableWidget->insertRow(row);
                for(j=0;j<3;j++)
                {
                    s=QString::fromUtf8("");
                    if(j==0)//column name
                        s=pTable->horizontalHeaderItem(i)->text();//get header text
                    if(j==1)//alignment
                    {
                        ali=m_tcdNewSetting.get_column_alignment(i);
                        if(ali==TABLE_ALIGNMENT_LEFT)
                            s=QString::fromUtf8("links");
                        else if(ali==TABLE_ALIGNMENT_RIGHT)
                            s=QString::fromUtf8("rechts");
                        else
                            s=QString::fromUtf8("mitte");
                    }
                    if(j==2)//visible
                    {
                        vis=m_tcdNewSetting.get_column_visible(i);
                        if(vis==true)
                            s=QString::fromUtf8("sichtbar");
                        else
                            s=QString::fromUtf8("nicht sichtbar");
                    }
                    //-
                    if(s.length()<=0)
                        break;//error
                    else
                    {
                        pItem=new QTableWidgetItem(s);
                        if(pItem!=NULL)
                            ui->tableWidget->setItem(row,j,pItem);
                        else
                            break;
                    }
                }
                if(j<3)
                    break;//error
            }
            if(i==pTable->columnCount())//all right
                bReturn=true;
        }
    }

    return bReturn;
}

bool CInputDialogTableSetting::button_up(void)
{
    int i,row;
    QTableWidgetItem* pItem;
    QList<QTableWidgetItem*> lsSel=ui->tableWidget->selectedItems();
    QList<QString> lsSelRow,lsRowBefore;
    if(lsSel.count()>2)
    {
        row=lsSel[0]->row();//row == column
        if(row>0)
        {
            //mark text
            for(i=0;i<ui->tableWidget->columnCount();i++)
            {
                lsSelRow.push_back(ui->tableWidget->item(row,i)->text());
                lsRowBefore.push_back(ui->tableWidget->item(row-1,i)->text());
            }

            //delete rows
            ui->tableWidget->removeRow(row);
            ui->tableWidget->removeRow(row-1);

            //new rows (switch)
            ui->tableWidget->insertRow(row-1);
            ui->tableWidget->insertRow(row);
            for(i=0;i<ui->tableWidget->columnCount();i++)
            {
                pItem=new QTableWidgetItem(lsSelRow[i]);
                    ui->tableWidget->setItem(row-1,i,pItem);

                pItem=new QTableWidgetItem(lsRowBefore[i]);
                if(pItem!=NULL)
                    ui->tableWidget->setItem(row,i,pItem);
            }
            ui->tableWidget->setCurrentItem(ui->tableWidget->item(row-1,0));

            m_tcdNewSetting.swap_columns(row-1,row);
            check_user_input();
        }
    }
    lsSel.clear();
    lsSelRow.clear();
    lsRowBefore.clear();
    return true;
}

bool CInputDialogTableSetting::button_down(void)
{
    int i,row;
    QTableWidgetItem* pItem;
    QList<QTableWidgetItem*> lsSel=ui->tableWidget->selectedItems();
    QList<QString> lsSelRow,lsRowBehind;
    if(lsSel.count()>2)
    {
        row=lsSel[0]->row();//row == column
        if(row+1<ui->tableWidget->rowCount())
        {
            //mark text
            for(i=0;i<ui->tableWidget->columnCount();i++)
            {
                lsSelRow.push_back(ui->tableWidget->item(row,i)->text());
                lsRowBehind.push_back(ui->tableWidget->item(row+1,i)->text());
            }

            //delete rows
            ui->tableWidget->removeRow(row+1);
            ui->tableWidget->removeRow(row);

            //new rows (switch)
            ui->tableWidget->insertRow(row);
            ui->tableWidget->insertRow(row+1);
            for(i=0;i<ui->tableWidget->columnCount();i++)
            {
                pItem=new QTableWidgetItem(lsRowBehind[i]);
                if(pItem!=NULL)
                    ui->tableWidget->setItem(row,i,pItem);

                pItem=new QTableWidgetItem(lsSelRow[i]);
                    ui->tableWidget->setItem(row+1,i,pItem);
            }
            ui->tableWidget->setCurrentItem(ui->tableWidget->item(row+1,0));

            m_tcdNewSetting.swap_columns(row+1,row);
            check_user_input();
        }
    }
    lsSel.clear();
    lsSelRow.clear();
    lsRowBehind.clear();
    return true;
}

bool CInputDialogTableSetting::button_left(void)
{
    int row;
    QList<QTableWidgetItem*> ls=ui->tableWidget->selectedItems();
    if(ls.count()>1)
    {
        row=ls[1]->row();//row == column
        if(row>=0 && row<m_tcdNewSetting.get_columns_count())
        {
            ls[1]->setText(QString::fromUtf8("links"));
            m_tcdNewSetting.set_column_alignment(row,TABLE_ALIGNMENT_LEFT);
            check_user_input();
        }
    }
    ls.clear();
    return true;
}

bool CInputDialogTableSetting::button_center(void)
{
    int row;
    QList<QTableWidgetItem*> ls=ui->tableWidget->selectedItems();
    if(ls.count()>1)
    {
        row=ls[1]->row();//row == column
        if(row>=0 && row<m_tcdNewSetting.get_columns_count())
        {
            ls[1]->setText(QString::fromUtf8("mitte"));
            m_tcdNewSetting.set_column_alignment(row,TABLE_ALIGNMENT_CENTER);
            check_user_input();
        }
    }
    ls.clear();
    return true;
}

bool CInputDialogTableSetting::button_right(void)
{
    int row;
    QList<QTableWidgetItem*> ls=ui->tableWidget->selectedItems();
    if(ls.count()>1)
    {
        row=ls[1]->row();//row == column
        if(row>=0 && row<m_tcdNewSetting.get_columns_count())
        {
            ls[1]->setText(QString::fromUtf8("rechts"));
            m_tcdNewSetting.set_column_alignment(row,TABLE_ALIGNMENT_RIGHT);
            check_user_input();
        }
    }
    ls.clear();
    return true;
}

bool CInputDialogTableSetting::button_visible(void)
{
    int row;
    bool bVis;
    QString s;
    QList<QTableWidgetItem*> ls=ui->tableWidget->selectedItems();
    if(ls.count()>2)
    {
        row=ls[2]->row();//row == column
        if(row>=0 && row<m_tcdNewSetting.get_columns_count())
        {
            if(m_tcdNewSetting.get_column_visible(row)==true)
            {
                s=QString::fromUtf8("nicht sichtbar");
                bVis=false;
            }
            else
            {
                s=QString::fromUtf8("sichtbar");
                bVis=true;
            }
            ls[2]->setText(s);
            m_tcdNewSetting.set_column_visible(row,bVis);
            check_user_input();
        }
    }
    ls.clear();
    return true;
}
