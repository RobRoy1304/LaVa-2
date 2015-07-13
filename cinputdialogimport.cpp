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

#include "cinputdialogimport.h"
#include "ui_cinputdialogimport.h"

CInputDialogImport::CInputDialogImport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogImport)
{
    m_iTabChangeToEnable=0;
    ui->setupUi(this);
    ui->tabWidgetSteps->setCurrentIndex(0);
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
    m_iDataType=0;
    m_bCheckboxFirstRowHeader=true;

    //settings
    settings(false);//load & set settings

    //disable auto default buttons
    ui->pushButtonBack_1->setAutoDefault(false);
    ui->pushButtonBack_2->setAutoDefault(false);
    ui->pushButtonBrowseFile->setAutoDefault(false);
    ui->pushButtonColumnChange->setAutoDefault(false);
    ui->pushButtonDeleteRow->setAutoDefault(false);
    ui->pushButtonImport->setAutoDefault(false);
    ui->pushButtonNext_1->setAutoDefault(false);
    ui->pushButtonNext_2->setAutoDefault(false);

    //step 1
    ui->pushButtonNext_1->setEnabled(false);
    ui->pushButtonNext_2->setEnabled(false);
    ui->pushButtonBrowseFile->setFocus();
    set_error_msg_step1(false);//enable error message step 1

    //step 2
    for (int i=0;i<3;i++)
        ui->tableWidgetAllocation->setColumnWidth(i,300);
    ui->pushButtonColumnChange->setEnabled(false);
    set_error_msg_step2(false);//enable error message step 2

    //step 3
    ui->pushButtonDeleteRow->setEnabled(false);
    ui->label_error_3->setVisible(false);

    //basic
    connect(ui->tabWidgetSteps,SIGNAL(currentChanged(int)),SLOT(current_tab_changed(int)));

    //step 1
    connect(ui->pushButtonBrowseFile,SIGNAL(clicked(bool)),SLOT(push_browse_file()));
    connect(ui->checkBoxComma,SIGNAL(clicked()),SLOT(check_comma()));
    connect(ui->checkBoxSpace,SIGNAL(clicked()),SLOT(check_space()));
    connect(ui->checkBoxSemi,SIGNAL(clicked()),SLOT(check_semi()));
    connect(ui->checkBoxTab,SIGNAL(clicked()),SLOT(check_tab()));
    connect(ui->comboBoxTextSpliter,SIGNAL(currentIndexChanged(int)),SLOT(changed_textsplitter()));
    connect(ui->lineEditSplitChar,SIGNAL(textChanged(QString)),SLOT(edit_split_char()));
    connect(ui->spinBoxFromRow,SIGNAL(valueChanged(int)),SLOT(changed_spin_from(int)));
    connect(ui->spinBoxToRow,SIGNAL(valueChanged(int)),SLOT(changed_spin_to(int)));
    connect(ui->pushButtonNext_1,SIGNAL(clicked()),SLOT(push_button_next_1_clicked()));
    connect(ui->comboBoxCodec,SIGNAL(currentIndexChanged(int)),this,SLOT(changed_combobox_codec()));
    connect(ui->checkBoxFirstRowHeader,SIGNAL(clicked(bool)),this,SLOT(check_first_row_header(bool)));

    //step 2
    connect(ui->comboBoxTyp,SIGNAL(currentIndexChanged(int)),SLOT(combobox_data_type_changed()));
    connect(ui->pushButtonBack_1,SIGNAL(clicked()),SLOT(push_button_back_1_clicked()));
    connect(ui->tableWidgetAllocation,SIGNAL(cellClicked(int,int)),SLOT(allocation_table_clicked()));
    connect(ui->pushButtonColumnChange,SIGNAL(clicked()),SLOT(column_allocation_change()));
    connect(ui->tableWidgetAllocation,SIGNAL(cellDoubleClicked(int,int)),SLOT(column_allocation_change()));
    connect(ui->pushButtonNext_2,SIGNAL(clicked()),SLOT(push_button_next_2_clicked()));
    connect(ui->tableWidgetFileView_2,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),SLOT(table_fileview2_double_clicked(QTableWidgetItem*)));
    connect(ui->tableWidgetFileView_2,SIGNAL(itemChanged(QTableWidgetItem*)),SLOT(table_fileview2_item_changed(QTableWidgetItem*)));

    //step 3
    connect(ui->pushButtonBack_2,SIGNAL(clicked()),SLOT(push_button_back_2_clicked()));
    connect(ui->tableWidgetRecordData,SIGNAL(cellClicked(int,int)),SLOT(record_table_clicked()));
    connect(ui->pushButtonDeleteRow,SIGNAL(clicked()),SLOT(push_button_delete_row_clicked()));
    connect(ui->tableWidgetRecordData,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),SLOT(table_record_double_clicked(QTableWidgetItem*)));
    connect(ui->tableWidgetRecordData,SIGNAL(itemChanged(QTableWidgetItem*)),SLOT(table_record_item_changed(QTableWidgetItem*)));
    connect(ui->pushButtonImport,SIGNAL(clicked()),SLOT(import_button_clicked()));
}

CInputDialogImport::~CInputDialogImport()
{
    settings(true);//save settings if update
    m_lsFileRows.clear();
    delete ui;
}

bool CInputDialogImport::settings(bool bUpdate)
{
    bool b;
    QString s;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;

    lsSType.push_back(QString::fromUtf8("DLG_IMPORT_CSV_DIALOG_CODEC"));
    lsSType.push_back(QString::fromUtf8("DLG_IMPORT_CSV_DIALOG_SPLIT_TEXT_CHAR"));
    lsSType.push_back(QString::fromUtf8("DLG_IMPORT_CSV_DIALOG_SPLIT_CHAR"));
    lsSType.push_back(QString::fromUtf8("DLG_IMPORT_CSV_DIALOG_SELECT_TYPE"));
    lsSType.push_back(QString::fromUtf8("DLG_IMPORT_CSV_FIRST_ROW_HEADER"));
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //codec combobox
            if(lsSValue.count()>0)
                settings.set_combobox(ui->comboBoxCodec,lsSValue[0],0);

            //split text char combobox
            if(lsSValue.count()>1)
                settings.set_combobox(ui->comboBoxTextSpliter,lsSValue[1],0);

            //checkbox's split char
            if(lsSValue.count()>2)
            {
                s=lsSValue[2];

                if(lsSValue[2].contains(QString::fromUtf8(";")))
                    ui->checkBoxSemi->setChecked(true);
                s.remove(QString::fromUtf8(";"));
                //-
                if(lsSValue[2].contains(QString::fromUtf8(",")))
                    ui->checkBoxComma->setChecked(true);
                s.remove(QString::fromUtf8(","));
                //-
                if(lsSValue[2].contains(QString::fromUtf8("\t")))
                    ui->checkBoxTab->setChecked(true);
                s.remove(QString::fromUtf8("\t"));
                //-
                if(lsSValue[2].contains(QString::fromUtf8(" ")))
                    ui->checkBoxSpace->setChecked(true);
                s.remove(QString::fromUtf8(" "));
                //-
                if(s.length()>0)//other split char set?
                    ui->lineEditSplitChar->setText(s);

                //type combobox
                if(lsSValue.count()>3)
                    settings.set_combobox(ui->comboBoxTyp,lsSValue[3],0);

                //checkbox first row header
                if(lsSValue.count()>4)
                    settings.set_checkbox(ui->checkBoxFirstRowHeader,lsSValue[4],true);
            }
        }
        if(bUpdate)//write
        {
            //combobox codec
            if(lsSValue.count()>0)
            {
                if(settings.get_combobox(ui->comboBoxCodec,lsSValue[0]))
                {
                    lsSUpdateType.push_back(lsSType[0]);
                    lsSUpdateValue.push_back(lsSValue[0]);
                }
            }

            //comboxbox split text char
            if(lsSValue.count()>1)
            {
                if(settings.get_combobox(ui->comboBoxTextSpliter,lsSValue[1]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[1]);
                    lsSUpdateValue.push_back(lsSValue[1]);
                }
            }

            //checkbox's split char
            if(lsSValue.length()>2)
            {
                s=QString::fromUtf8("");

                if(ui->checkBoxSemi->isChecked())
                    s+=QString::fromUtf8(";");
                if(ui->checkBoxComma->isChecked())
                    s+=QString::fromUtf8(",");
                if(ui->checkBoxTab->isChecked())
                    s+=QString::fromUtf8("\t");
                if(ui->checkBoxSpace->isChecked())
                    s+=QString::fromUtf8(" ");
                if(ui->lineEditSplitChar->text().length()>0)//other split char set
                    s+=ui->lineEditSplitChar->text();

                if(s!=lsSValue[2])
                {
                    lsSUpdateType.push_back(lsSType[2]);
                    lsSUpdateValue.push_back(s);
                }
            }

            //comboxbox type
            if(lsSValue.count()>3)
            {
                if(settings.get_combobox(ui->comboBoxTyp,lsSValue[3]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[3]);
                    lsSUpdateValue.push_back(lsSValue[3]);
                }
            }

            //checkbox first row header
            if(lsSValue.count()>4)
            {
                if(settings.get_checkbox(ui->checkBoxFirstRowHeader,lsSValue[4]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[4]);
                    lsSUpdateValue.push_back(lsSValue[4]);
                }
            }

            //write updates
            if(lsSUpdateType.count()>0 && lsSUpdateType.count()==lsSUpdateValue.count())
                b=settings.write(lsSUpdateType,lsSUpdateValue);
        }
    }
    //-
    lsSValue.clear();
    lsSType.clear();
    lsSUpdateValue.clear();
    lsSUpdateType.clear();
    return b;
}


bool CInputDialogImport::current_tab_changed(int i)
{
    if(i!=m_iTabChangeToEnable)
        ui->tabWidgetSteps->setCurrentIndex(m_iTabChangeToEnable);
    return true;
}


//--Step 1---------------------------------------------------------------------------------------------

bool CInputDialogImport::check_comma(void)
{
    return update_view();
}

bool CInputDialogImport::check_space(void)
{
    return update_view();
}

bool CInputDialogImport::check_semi(void)
{
    return update_view();
}

bool CInputDialogImport::check_tab(void)
{
    return update_view();
}

bool CInputDialogImport::changed_textsplitter(void)
{
    return update_view();
}

bool CInputDialogImport::edit_split_char(void)
{
    return update_view();
}

bool CInputDialogImport::changed_spin_from(int i)
{
    if(i<1)
        ui->spinBoxFromRow->setValue(1);
    if(i> ui->spinBoxToRow->value())
        ui->spinBoxFromRow->setValue(ui->spinBoxToRow->value());
    return update_view();
}

bool CInputDialogImport::changed_spin_to(int i)
{
    if(i< ui->spinBoxFromRow->value())
        ui->spinBoxToRow->setValue(ui->spinBoxFromRow->value());
    if(i> m_lsFileRows.count())
        ui->spinBoxToRow->setValue(m_lsFileRows.count());
    return update_view();
}

bool CInputDialogImport::push_button_next_1_clicked(void)
{
    if(m_bUpdateStep1to2)
    {
        update_allocation_table();//fill allocation table
        tranfer_fileview_data();
        m_bUpdateStep1to2=false;
    }
    //-
    m_iTabChangeToEnable=1;
    ui->tabWidgetSteps->setCurrentIndex(1);//change tab
    return true;
}

bool CInputDialogImport::push_browse_file(void)
{
    QString sFile,sImportPath;
    QStringList ls;
    QFileDialog dlg(this,QString::fromUtf8("Datei öffnen"));
    CSettings settings;
    QDir dir;
    //-
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setFileMode(QFileDialog::ExistingFile);
    if(m_sFile.length()>0)
        dlg.selectFile(m_sFile);
    else
    {
        //import path (read)
        settings.load(QString::fromUtf8("IMPORT_PATH"),sImportPath);
        dlg.setDirectory(sImportPath);
    }
    //-
    if(dlg.exec())
    {
        ls=dlg.selectedFiles();
        if(ls.count()>0)
        {
            sFile=ls[0];//get selected file
            if(sFile!=m_sFile)//changed?
            {
                if(check_and_set(sFile))
                {
                    ui->lineEditFile->setText(m_sFile);
                    ui->spinBoxFromRow->setValue(1);
                    update_view();
                }
            }
        }
        ls.clear();

        //import path (write)
        dir=dlg.directory();
        sImportPath=dir.path();
        settings.write(QString::fromUtf8("IMPORT_PATH"),sImportPath);
    }
    return true;
}

bool CInputDialogImport::changed_combobox_codec()
{
    check_and_set(m_sFile);
    update_view();
    return true;
}

bool CInputDialogImport::check_first_row_header(bool bChecked)
{
    m_bCheckboxFirstRowHeader=bChecked;
    update_view();
    return true;
}

bool CInputDialogImport::check_and_set(QString sFile)
{
    if(sFile.length()<=0)
        return false;

    int i=0;
    QString s;
    bool b=true;
    QFile file(sFile);
    QList<QString> lsFileRows;
    //-
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            //codec
            i=ui->comboBoxCodec->currentIndex();
            QTextStream stream(&file);
            if(i==0)//utf-8 selected
                stream.setCodec("UTF-8");
            else if(i==1)//windows 1252 selected
                stream.setCodec("Windows-1252");

            //-
            while(!stream.atEnd())
            {
                s=stream.readLine();
                lsFileRows.push_back(s);
            }

            //check row count
            if(lsFileRows.count()<=0)
                b=false;
            else
            {
                m_sFile=sFile;
                m_lsFileRows.clear();
                while(lsFileRows.count()>0)//copy
                {
                    m_lsFileRows.push_back(lsFileRows[0]);
                    lsFileRows.removeFirst();
                }
                //-
                if(ui->spinBoxFromRow->value()>m_lsFileRows.count())//from row to high?
                    ui->spinBoxFromRow->setValue(m_lsFileRows.count());
                ui->spinBoxToRow->setValue(m_lsFileRows.count());//set to row
            }
        }
    }
    file.close();
    lsFileRows.clear();
    return b;
}

bool CInputDialogImport::set_error_msg_step1(bool bError)
{
    if(bError)
    {
        ui->label_error_1->setVisible(false);
        ui->label_error_1->setText(QString::fromUtf8(""));
    }
    else
    {
        ui->label_error_1->setVisible(true);
        ui->label_error_1->setText(QString::fromUtf8("Es wurde keine Datei ausgewählt."));
    }
    return true;
}

bool CInputDialogImport::update_view(void)
{
    if(m_lsFileRows.count()<=0)
        return false;

    QFont font;
    bool b=false;
    int j,i,c,row,column;
    QString s,sSplitter;
    QStringList ls;
    QTableWidgetItem * pItem=NULL;
    int iFromRow=ui->spinBoxFromRow->value()-1;
    int iToRow=ui->spinBoxToRow->value()-1;

    //clear table
    while(ui->tableWidgetFileView->rowCount()>0){
        ui->tableWidgetFileView->removeRow(ui->tableWidgetFileView->rowCount()-1);}
    while(ui->tableWidgetFileView->columnCount()>0){
        ui->tableWidgetFileView->removeColumn(ui->tableWidgetFileView->columnCount()-1);}

    //splitter
    if(ui->checkBoxSemi->isChecked())
        sSplitter+=QString::fromUtf8(";");
    if(ui->checkBoxComma->isChecked())
    {
        if(sSplitter.length()>0)
            sSplitter+=QString::fromUtf8(",");
        sSplitter+=QString::fromUtf8(",");
    }
    if(ui->checkBoxTab->isChecked())
    {
        if(sSplitter.length()>0)
            sSplitter+=QString::fromUtf8(",");
        sSplitter+=QString::fromUtf8("\t");
    }
    if(ui->checkBoxSpace->isChecked())
    {
        if(sSplitter.length()>0)
            sSplitter+=QString::fromUtf8(",");
        sSplitter+=QString::fromUtf8(" ");
    }
    if(ui->lineEditSplitChar->text().length()>0)
    {
        if(sSplitter.length()>0)
            sSplitter+=QString::fromUtf8(",");
        sSplitter+=ui->lineEditSplitChar->text();
    }

    //fill table
    for(i=iFromRow,row=0;i<=iToRow;i++)
    {
        if(i<m_lsFileRows.count())
        {
            s=m_lsFileRows[i];
            if(sSplitter.length()<=0)//nothing split char?
                ls.push_back(s);
            else
                ls=s.split(sSplitter);
            if(ls.count()>0)
            {
                //add row & columns
                ui->tableWidgetFileView->insertRow(row);
                while(ui->tableWidgetFileView->columnCount()<ls.count())
                {
                    ui->tableWidgetFileView->insertColumn(ui->tableWidgetFileView->columnCount());
                    ui->tableWidgetFileView->setColumnWidth(ui->tableWidgetFileView->columnCount()-1,150);
                }

                //fill
                for(column=0;column<ui->tableWidgetFileView->columnCount() && column<ls.count();column++)
                {
                    s=ls[column];
                    if(ui->comboBoxTextSpliter->currentIndex()!=2)//text spliter 0=" 1=' 2=nothing
                        s=s.remove(ui->comboBoxTextSpliter->itemText(ui->comboBoxTextSpliter->currentIndex()));
                    pItem=new QTableWidgetItem(s);
                    if(pItem!=NULL)
                        ui->tableWidgetFileView->setItem(row,column,pItem);
                }

                //first row = header? -> yes, make items of the row bold
                font=ui->tableWidgetFileView->font();
                if(m_bCheckboxFirstRowHeader)
                    font.setBold(true);
                for(c=0;c<ui->tableWidgetFileView->columnCount();c++)
                {
                    pItem=ui->tableWidgetFileView->item(0,c);
                    if(pItem!=NULL)
                        pItem->setFont(font);
                }
                //-
                row++;
            }
            ls.clear();
        }
    }
    //check data in table -> enable button
    for(j=0;j<ui->tableWidgetFileView->rowCount();j++)
    {
        for(i=0;i<ui->tableWidgetFileView->columnCount();i++)
        {
            if(ui->tableWidgetFileView->item(j,i)!=NULL)
            {
                if(ui->tableWidgetFileView->item(j,i)->text()!=QString::fromUtf8(""))
                {
                    b=true;
                    break;
                }
            }
        }
        if(i<ui->tableWidgetFileView->columnCount())//break?
            break;
    }
    ui->pushButtonNext_1->setEnabled(b);//enable next putton step 1
    //-
    this->set_error_msg_step1(b);//error
    //-
    m_bUpdateStep1to2=true;//update data(step 2)
    //-
    return true;
}

bool CInputDialogImport::tranfer_fileview_data(void)
{
    int rs,c,r;
    QString s;
    QTableWidgetItem * pItem;

    //first row header -> item's bold
    QFont font;
    font=ui->tableWidgetFileView_2->font();
    if(m_bCheckboxFirstRowHeader)
        font.setBold(true);

    //clear table
    while(ui->tableWidgetFileView_2->rowCount()>0){
        ui->tableWidgetFileView_2->removeRow(ui->tableWidgetFileView_2->rowCount()-1);}
    while(ui->tableWidgetFileView_2->columnCount()>0){
        ui->tableWidgetFileView_2->removeColumn(ui->tableWidgetFileView_2->columnCount()-1);}
    //-
    if(ui->tableWidgetFileView->rowCount()<=0)//nothing data in source table(step 1)
        return false;
    else
    {
        for(rs=0,r=0;rs<ui->tableWidgetFileView->rowCount();r++,rs++)
        {
            //check source row empty?
            s=QString::fromUtf8("");
            for(c=0;c<ui->tableWidgetFileView->columnCount() && s==QString::fromUtf8("");c++)
            {
                if(ui->tableWidgetFileView->item(rs,c)!=NULL)
                    s+=ui->tableWidgetFileView->item(rs,c)->text();
            }
            if(s==QString::fromUtf8(""))
            {
                r--;
                continue;
            }

            //add row
            ui->tableWidgetFileView_2->insertRow(ui->tableWidgetFileView_2->rowCount());

            for(c=0;c<ui->tableWidgetFileView->columnCount();c++)
            {
                if(c>=ui->tableWidgetFileView_2->columnCount())//add columns
                {
                    ui->tableWidgetFileView_2->insertColumn(ui->tableWidgetFileView_2->columnCount());
                    ui->tableWidgetFileView_2->setColumnWidth(ui->tableWidgetFileView_2->columnCount()-1,150);

                    //set column header number
                    if(ui->tableWidgetFileView_2->columnCount()-1>=0)
                    {
                        s=QString::fromUtf8("Spalte : %1").arg(c+1);
                        pItem=new QTableWidgetItem(s);
                        if(pItem!=NULL)
                            ui->tableWidgetFileView_2->setHorizontalHeaderItem(c,pItem);
                    }
                }

                //fill item
                s=QString::fromUtf8("");
                if(ui->tableWidgetFileView->item(rs,c)!=NULL)
                    s=ui->tableWidgetFileView->item(rs,c)->text();
                pItem=new QTableWidgetItem(s);
                if(pItem!=NULL)
                {
                    if(m_bCheckboxFirstRowHeader==true && r==0)//first row header -> items bold
                        pItem->setFont(font);
                    ui->tableWidgetFileView_2->setItem(r,c,pItem);
                }
            }
        }
    }
    //-
    set_allocation_by_header(); //if first row=header set -> allocation by header
    //-
    return true;
}

//--Step 2-----------------------------------------------------------------------------------------------------------------------------------------

bool CInputDialogImport::set_allocation_by_header(void)
{//get header text of columns to set allocation auto

    bool b;
    int r,c,i,j;
    QString s,s2;
    QTableWidgetItem * pItem;
    QList<int> lsAllocation;
    //-
    if(m_bCheckboxFirstRowHeader)
    {
        //search header column text & text of value from data type
        for(r=0;r<ui->tableWidgetAllocation->rowCount();r++)
        {
            //text allocation
            pItem=ui->tableWidgetAllocation->item(r,0);
            if(pItem!=NULL)
            {
                s=pItem->text();

                //check text columns from header
                for(c=0;c<ui->tableWidgetFileView_2->columnCount();c++)
                {
                    pItem=ui->tableWidgetFileView_2->item(0,c);
                    if(pItem!=NULL)
                    {
                        s2=pItem->text();
                        if(s.compare(s2,Qt::CaseInsensitive)==0)
                        {
                            lsAllocation.push_back(c);//mark, if found
                            break;
                        }
                    }
                }
                //-
                if(c>=ui->tableWidgetFileView_2->columnCount())//nothing found?
                    lsAllocation.push_back(-1);
            }
        }

        //search double
        for(i=0;i<lsAllocation.count();i++)
        {
            for(j=1;j<lsAllocation.count() && i!=j && lsAllocation[i]!=-1; j++)
            {
                if(lsAllocation[i]==lsAllocation[j])
                    lsAllocation[j]=-1;//delete double
            }
        }

        //set allocation
        for(i=0;i<lsAllocation.count();i++)
        {
            if(lsAllocation[i]>=0 && lsAllocation[i]<ui->tableWidgetFileView_2->columnCount())
            {
                s=QString::fromUtf8("%1").arg(lsAllocation[i]+1);
                pItem=ui->tableWidgetAllocation->item(i,2);
                if(pItem!=NULL)
                    pItem->setText(s);
            }
        }

        //check data from columns, of type int or double(only by article-type)
        if(ui->comboBoxTyp->currentIndex()==0)//article?
        {
            for(r=0;r<ui->tableWidgetAllocation->rowCount();r++)
            {
                //get source column by auto set
                pItem=ui->tableWidgetAllocation->item(r,2);
                if(pItem!=NULL)
                {
                    s=pItem->text();
                    if(s.length()>0)
                    {
                        c=s.toInt(&b);
                        if(b)
                        {
                            //check column
                            if(!check_column_data_and_member_type(c-1,r))
                            {//error, some of cell in source row not int / double
                                pItem->setText(QString::fromUtf8(""));//delete allocation
                            }
                        }
                    }
                }
            }
        }
        //-
        m_iDataType=ui->comboBoxTyp->currentIndex();
        if(check_allocation_settings())
            m_bUpdateStep2to3=true;
    }
    //-
    lsAllocation.clear();
    //-
    return true;
}

bool CInputDialogImport::combobox_data_type_changed(void)
{
    update_allocation_table();
    m_iDataType=ui->comboBoxTyp->currentIndex();
    ui->pushButtonColumnChange->setEnabled(false);
    m_bUpdateStep2to3=true;
    set_allocation_by_header();
    return true;
}

bool CInputDialogImport::push_button_back_1_clicked(void)
{
    m_iTabChangeToEnable=0;
    ui->tabWidgetSteps->setCurrentIndex(0);//change tab
    return true;
}

bool CInputDialogImport::allocation_table_clicked(void)
{
    QList<QTableWidgetItem*> ls=ui->tableWidgetAllocation->selectedItems();
    if(ls.count()>0)
        ui->pushButtonColumnChange->setEnabled(true);
    else
        ui->pushButtonColumnChange->setEnabled(false);
    ls.clear();
    return true;
}

bool CInputDialogImport::table_fileview2_double_clicked(QTableWidgetItem* pItem)
{
    m_bUpdateStep2to3=true;

    //mark cell value, before user edit (only number)
    if(pItem!=NULL)
        m_sMarkValueUserEditFileview2=pItem->text();
    return true;
}

bool CInputDialogImport::table_fileview2_item_changed(QTableWidgetItem* pItem)
{
    bool b=true;
    int y,c,iaArticleTyp[]=ARTICLE_MEMBER_TYPE;
    QString s,sInput;
    QMessageBox msg(this);
    msg.setWindowTitle(QString::fromUtf8("Fehler..."));

    if(pItem!=NULL && m_sMarkValueUserEditFileview2.length()>0)//edit by user?
    {
        //check input , type int or double ->number
        sInput=pItem->text();
        if(sInput.length()>0)
        {
            if(ui->comboBoxTyp->currentIndex()==0)//type article? (has only number types)
            {
                //search in allocation
                c=pItem->column();
                s=QString::fromUtf8("%1").arg(c+1);
                for(y=0;y<ui->tableWidgetAllocation->rowCount();y++)
                {
                    if(ui->tableWidgetAllocation->item(y,2)!=NULL)
                    {
                        if(ui->tableWidgetAllocation->item(y,2)->text()==s)//found
                            break;
                    }
                }
                //check type
                if(y<ui->tableWidgetAllocation->rowCount())
                {
                    if(iaArticleTyp[y]==1)//int type
                    {
                        sInput.toInt(&b);
                        s=QString::fromUtf8("Ganzzahl");
                    }
                    if(iaArticleTyp[y]==2)//double type
                    {
                        sInput.toDouble(&b);
                        s=QString::fromUtf8("Kommazahl");
                    }

                    //input error
                    if(!b)
                    {
                        s=QString::fromUtf8("Ihre Eingabe muss eine %1 sein! Siehe Spaltenzuordnung und Typ.").arg(s);
                        msg.setText(s);
                        msg.exec();
                        pItem->setText(m_sMarkValueUserEditFileview2);
                    }
                }
            }
        }

        m_sMarkValueUserEditFileview2=QString::fromUtf8("");//clear mark
    }
    return true;
}

bool CInputDialogImport::column_allocation_change(void)
{
    bool b;
    QString s,sDataName,sMark;
    QMessageBox msg(this);
    msg.setWindowTitle(QString::fromUtf8("Fehler..."));
    QList<QTableWidgetItem*> ls=ui->tableWidgetAllocation->selectedItems();
    int iaArticleTyp[]=ARTICLE_MEMBER_TYPE;
    int i,c,iValue=1,iMaxValue=ui->tableWidgetFileView_2->columnCount();
    //-
    if(ls.count()>2)
    {
        if(ls[0]!=NULL && ls[1]!=NULL && ls[2]!=NULL)
        {
            //column set?
            if(ls[2]->text()==QString::fromUtf8(""))
            {
                iValue=get_next_column_number_where_not_set();//get next free column number(where not set)
                if(iValue<0)
                    iValue=0;
            }
            if(ls[2]->text()!=QString::fromUtf8(""))
            {
                sMark=ls[2]->text();
                iValue=sMark.toInt(&b);
                if(!b)
                {
                    ls[2]->setText(QString::fromUtf8(""));
                    iValue=0;
                }
            }

            sDataName=QString::fromUtf8("Spaltenzuordnung bearbeiten:   Datenfeld : '%1'   Typ: %2").arg(ls[0]->text(),ls[1]->text());
            c=QInputDialog::getInt(this,QString::fromUtf8("Spaltenzuordnung bearbeiten"),sDataName,iValue,0,iMaxValue,1,&b);
            if(b)//press ok?
            {
                if(c==0)//disable?
                {
                    if(ui->tableWidgetAllocation->item(ls[0]->row(),2)!=NULL)
                        ui->tableWidgetAllocation->item(ls[0]->row(),2)->setText(QString::fromUtf8(""));//clear item
                    else{}
                }
                else
                {//ok
                    s=QString::fromUtf8("%1").arg(c);

                    //check column number is select by another
                    for(i=0;i<ui->tableWidgetAllocation->rowCount();i++)
                    {
                        if(ui->tableWidgetAllocation->item(i,2)!=NULL)
                        {
                            if(ui->tableWidgetAllocation->item(i,2)->text()==s && s!=sMark)
                                break;
                        }
                    }
                    if(i<ui->tableWidgetAllocation->rowCount())
                        msg.setText(QString::fromUtf8("Diese Spaltennummer ist bereits von einem anderen Datenfeld gesetzt!"));//set error
                    else
                    {
                        //check type
                        if(check_column_data_and_member_type(c-1,ls[0]->row()))
                            ls[2]->setText(s);//all ok
                        else
                        {//error
                            s=QString::fromUtf8("Die Daten in der Spalte '%1' sind keine/nicht ausschliesslich ").arg(c);
                            if(ls[0]->row()>=0 && ls[0]->row()<14)//in array range?
                            {
                                if(iaArticleTyp[ls[0]->row()]==1)//integer type?
                                    s+=QString::fromUtf8("Zahlen (Ganzzahlen)!");
                                else//double type?
                                    s+=QString::fromUtf8("Zahlen (Kommazahlen)!");
                            }
                            msg.setText(s);
                        }

                    }
                }
                check_allocation_settings();//enable next 2 button if ok
                m_bUpdateStep2to3=true;
            }
        }
    }
    //error message
    if(msg.text()!=QString::fromUtf8(""))//set?
        msg.exec();
    //-
    ls.clear();
    return true;
}

bool CInputDialogImport::check_column_data_and_member_type(int iSourceColumn, int iMemberRow)
{
    bool b=true;
    QString s;
    int i,iaArticleTyp[]=ARTICLE_MEMBER_TYPE;

    //check type & data in column
    if(ui->comboBoxTyp->currentIndex()==0)//actile? (has only number type's)
    {
        if(ui->tableWidgetFileView_2->columnCount()>=0 && iSourceColumn<ui->tableWidgetFileView_2->columnCount())//column in range?
        {
            if(iMemberRow>=0 && iMemberRow<14)//member row in range? (array)
            {
                if(iaArticleTyp[iMemberRow]>0)//type not text
                {
                    //if first row header -> ignore first row
                    i=0;
                    if(m_bCheckboxFirstRowHeader)
                        i=1;
                    //-
                    for(;i<ui->tableWidgetFileView_2->rowCount() && b==true;i++)
                    {
                        if(ui->tableWidgetFileView_2->item(i,iSourceColumn)!=NULL)
                        {
                            s=ui->tableWidgetFileView_2->item(i,iSourceColumn)->text();
                            if(s.length()>0)//set?
                            {
                                if(iaArticleTyp[iMemberRow]==1)//type int
                                    s.toInt(&b);
                                if(iaArticleTyp[iMemberRow]==2)//type double
                                {
                                    //change ',' to '.' //german format
                                    s.replace(QString::fromUtf8(","),QString::fromUtf8("."));
                                    s.toDouble(&b);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //-
    return b;
}

int CInputDialogImport::get_next_column_number_where_not_set(void)
{
    bool b;
    QString s;
    int i,n,next_column=-1;
    QList<int> lsColumns;

    //get all columns where set
    for(i=0;i<ui->tableWidgetAllocation->rowCount();i++)
    {
        if(ui->tableWidgetAllocation->item(i,2)!=NULL)
        {
            s=ui->tableWidgetAllocation->item(i,2)->text();
            n=s.toInt(&b);
            if(b)
                lsColumns.push_back(n);
        }
    }

    //search next free column number where not set
    for(i=0;i<ui->tableWidgetAllocation->rowCount();i++)//max column = count of member
    {
        for(n=0;n<lsColumns.count();n++)
        {
            if(lsColumns[n]==i+1)//found
                    break;
        }
        if(n>=lsColumns.count())//not found
        {
            next_column=i+1;
            break;
        }
    }
    //.
    lsColumns.clear();
    return next_column;
}

bool CInputDialogImport::check_allocation_settings(void)
{
    bool b=false;
    //-
    if(ui->tableWidgetAllocation->item(0,2)!=NULL)
    {
        if(ui->tableWidgetAllocation->item(0,2)->text()!=QString::fromUtf8(""))//name datafield column set?
            b=true;
    }
    ui->pushButtonNext_2->setEnabled(b);//next 2 button
    set_error_msg_step2(b);//set error message
    return b;
}

bool CInputDialogImport::update_allocation_table(void)
{
    QString s;
    QTableWidgetItem * pItem;
    QList<QString> lsMember;
    int i,iaArticleTyp[]=ARTICLE_MEMBER_TYPE;
    int sel=ui->comboBoxTyp->currentIndex();
    //-
    //members
    switch(sel)
    {
        case 0://article
            lsMember.push_back("Artikelbezeichnung");
            lsMember.push_back("Bestand");
            lsMember.push_back(QString::fromUtf8("max.Lagerkapazität"));
            lsMember.push_back("Einheit");
            lsMember.push_back("1.Artikelnummer");
            lsMember.push_back("2.Artikelnummer");
            lsMember.push_back("Hersteller");
            lsMember.push_back("Warengruppe");
            lsMember.push_back("Warnung");
            lsMember.push_back("EK");
            lsMember.push_back("VK");
            lsMember.push_back(QString::fromUtf8("Währung"));
            lsMember.push_back("Standort");
            lsMember.push_back("Kommentar");
            break;
        case 1:
        case 2:
            lsMember.push_back("Name");
            lsMember.push_back("Adresse");
            lsMember.push_back("Telefonnummer");
            lsMember.push_back("Faxnummer");
            lsMember.push_back("Emailadresse");
            lsMember.push_back("homepage");
            lsMember.push_back("Kontaktperson");
            if(sel==1)
                 lsMember.push_back("Kundennummer");
            lsMember.push_back("Kommentar");
            break;
        case 3:
            lsMember.push_back("Nachname");
            lsMember.push_back("Vorname");
            lsMember.push_back("Strasse");
            lsMember.push_back("Postleitzahl");
            lsMember.push_back("Ort");
            lsMember.push_back("Kundennummer");
            lsMember.push_back("Telefonnummer");
            lsMember.push_back("Faxnummer");
            lsMember.push_back("Emailadresse");
            lsMember.push_back("Kommentar");
            break;
        case 4:
            lsMember.push_back("Pfad/.../Bezeichnung");
            lsMember.push_back("Kommentar");
            break;
    }

    //clear table
    while(ui->tableWidgetAllocation->rowCount()>0){
        ui->tableWidgetAllocation->removeRow(ui->tableWidgetAllocation->rowCount()-1);
    }

    //fill table
    for(i=0;i<lsMember.count();i++)
    {
        //member name
        ui->tableWidgetAllocation->insertRow(i);
        pItem=new QTableWidgetItem(lsMember[i]);
        if(i==0)
            pItem->setTextColor(QColor(Qt::darkRed));
        if(pItem!=NULL)
            ui->tableWidgetAllocation->setItem(i,0,pItem);//name member = textcolor red

        //member typ
        s=QString::fromUtf8("Text");
        if(sel==0)//numbers? (only article)
        {
            if(iaArticleTyp[i]==1)
                s=QString::fromUtf8("Ganzzahl");
            if(iaArticleTyp[i]==2)
                s=QString::fromUtf8("Kommazahl");
        }
        pItem=new QTableWidgetItem(s);
        if(i==0)
            pItem->setTextColor(QColor(Qt::darkRed));
        if(pItem!=NULL)
            ui->tableWidgetAllocation->setItem(i,1,pItem);

        //empty item
        pItem=new QTableWidgetItem(QString::fromUtf8(""));
        if(i==0)
            pItem->setTextColor(QColor(Qt::darkRed));
        if(pItem!=NULL)
            ui->tableWidgetAllocation->setItem(i,2,pItem);
    }
    //-
    lsMember.clear();
    //-
    check_allocation_settings();
    //-
    return true;
}

bool CInputDialogImport::set_error_msg_step2(bool bError)
{
    if(bError)
    {
        ui->label_error_2->setVisible(false);
        ui->label_error_2->setText(QString::fromUtf8(""));
    }
    else
    {
        ui->label_error_2->setVisible(true);
        ui->label_error_2->setText(QString::fromUtf8("Das Datenfeld Bezeichnung bzw. Name muss einer Spalte zugeordnet sein."));
    }
    return true;
}

bool CInputDialogImport::push_button_next_2_clicked(void)
{
    QString s;

    if(m_bUpdateStep2to3)
    {
        //set label data type
        s=ui->comboBoxTyp->currentText();
        if(s.length()>0)
            ui->label_data_type->setText(s);

        update_record_table();
        m_bUpdateStep2to3=false;
    }
    //-
    m_iTabChangeToEnable=2;
    ui->tabWidgetSteps->setCurrentIndex(2);//change tab
    return true;
}

//-- step 3 -----------------------------------------------------------------------------------------------------------------------------------------------------

bool CInputDialogImport::update_record_table(void)
{
    m_bCheckItemDataRecordTable=false;//check disable
    //-
    bool b;
    int y,x,c;
    QString s;
    QTableWidgetItem * pItem;
    QList<int> lsColumnAllocation;

    //clear record table
    while(ui->tableWidgetRecordData->rowCount()>0){
        ui->tableWidgetRecordData->removeRow(ui->tableWidgetRecordData->rowCount()-1);}
    while(ui->tableWidgetRecordData->columnCount()>0){
        ui->tableWidgetRecordData->removeColumn(ui->tableWidgetRecordData->columnCount()-1);}

    //add columns in record table (copy from allocation table)
    for(y=0;y<ui->tableWidgetAllocation->rowCount();y++)
    {
        //add
        if(ui->tableWidgetAllocation->item(y,0)!=NULL)
        {
            s=ui->tableWidgetAllocation->item(y,0)->text();
            if(s.length()>0)
            {
                ui->tableWidgetRecordData->insertColumn(ui->tableWidgetRecordData->columnCount());
                ui->tableWidgetRecordData->setColumnWidth(ui->tableWidgetRecordData->columnCount()-1,150);

                if(ui->tableWidgetRecordData->columnCount()-1>=0)
                {
                    pItem=new QTableWidgetItem(s);
                    if(pItem!=NULL)
                        ui->tableWidgetRecordData->setHorizontalHeaderItem(ui->tableWidgetRecordData->columnCount()-1,pItem);
                }
            }
        }

        //mark allocation
        if(ui->tableWidgetAllocation->item(y,2)!=NULL)
        {
            //column allocation
            s=ui->tableWidgetAllocation->item(y,2)->text();
            if(s.length()<=0)//allocation set?
                lsColumnAllocation.push_back(-1);//not set
            else
            {
                c=s.toInt(&b);//cast column allocation to int
                if(b)//ok?
                    lsColumnAllocation.push_back(c-1);//mark column allocation (real id.)
                else
                    lsColumnAllocation.push_back(-1);//cast fail
            }
        }
    }


    //add rows in record table
    y=0;
    if(m_bCheckboxFirstRowHeader)//if first row header -> ignore
        y=1;
    //-
    for(;y<ui->tableWidgetFileView_2->rowCount();y++)
    {
        //check is table item name empty
        b=false;
        if(ui->tableWidgetFileView_2->item(y,lsColumnAllocation[0])!=NULL)
        {
            s=ui->tableWidgetFileView_2->item(y,lsColumnAllocation[0])->text();
            if(s.length()>0)
                b=true;
        }

        if(b)
        {//add row
            ui->tableWidgetRecordData->insertRow(ui->tableWidgetRecordData->rowCount());

            for(x=0;x<lsColumnAllocation.count();x++)
            {
                s=QString::fromUtf8("");
                if(lsColumnAllocation[x]>=0)//allocation set
                {
                    if(ui->tableWidgetFileView_2->item(y,lsColumnAllocation[x])!=NULL)
                        s=ui->tableWidgetFileView_2->item(y,lsColumnAllocation[x])->text();
                }

                //copy data
                pItem=new QTableWidgetItem(s);
                if(pItem!=NULL && ui->tableWidgetRecordData->rowCount()-1>=0)
                    ui->tableWidgetRecordData->setItem(ui->tableWidgetRecordData->rowCount()-1,x,pItem);
            }
        }
    }
    //-
    ui->pushButtonDeleteRow->setEnabled(false);//disable button delete row
    lsColumnAllocation.clear();
    //-
    check_record_table_data();
    check_update_double_names_recordtable();
    //-
    m_bCheckItemDataRecordTable=true;//check enable
    return true;
}

bool CInputDialogImport::push_button_back_2_clicked(void)
{
    m_iTabChangeToEnable=1;
    ui->tabWidgetSteps->setCurrentIndex(1);//change tab
    return true;
}

bool CInputDialogImport::record_table_clicked(void)
{
    QList<QTableWidgetItem*> ls=ui->tableWidgetRecordData->selectedItems();
    if(ls.count()>0)
        ui->pushButtonDeleteRow->setEnabled(true);
    else
        ui->pushButtonDeleteRow->setEnabled(false);
    ls.clear();
    return true;
}

bool CInputDialogImport::push_button_delete_row_clicked(void)
{
    int row;
    QList<QTableWidgetItem*> ls=ui->tableWidgetRecordData->selectedItems();
    if(ls.count()>0)
    {
        row=ls[0]->row();
        ui->tableWidgetRecordData->removeRow(row);
    }
    ls.clear();
    //-
    if(ui->tableWidgetRecordData->rowCount()<=0)//record table empty?
    {
        ui->pushButtonDeleteRow->setEnabled(false);//disable button delete row
        m_bUpdateStep2to3=true;
    }
    //-
    check_record_table_data();
    return true;
}

bool CInputDialogImport::check_record_table_data(void)
{
    bool bEnable;
    QString s,sError;
    //-

    if(ui->tableWidgetRecordData->rowCount()<=0)//rows?
       sError=QString::fromUtf8("Es gibt keine Daten in der Tabelle 'Vorschau importierender Datensätze'!");

    //Error? -> set text
    if(sError.length()>0)
    {
        s=QString::fromUtf8("<font color='darkRed'>%1</font>").arg(sError);
        ui->label_error_3->setText(s);
        ui->label_error_3->setVisible(true);
        bEnable=false;
    }
    else
    {
        ui->label_error_3->setText(QString::fromUtf8(""));
        ui->label_error_3->setVisible(false);
        bEnable=true;
    }

    //import button
    ui->pushButtonImport->setEnabled(bEnable);//check data ok -> enable import button
    //-
    return true;
}

bool CInputDialogImport::table_record_double_clicked(QTableWidgetItem* pItem)
{
    //mark cell value, before user edit (only number)
    if(pItem!=NULL)
        m_sMarkValueUserEditRecordTable=pItem->text();
    return true;
}

bool CInputDialogImport::table_record_item_changed(QTableWidgetItem* pItem)
{
    bool b=true;
    int c,iaArticleTyp[]=ARTICLE_MEMBER_TYPE,iDataType=ui->comboBoxTyp->currentIndex();
    QString s,sCondition,sInput,sError,sDataTypeName=ui->comboBoxTyp->currentText();
    QMessageBox msg(this);
    msg.setWindowTitle(QString::fromUtf8("Fehler..."));
    //-
    if(m_pThread==NULL || pItem==NULL)
        b=false;
    if(m_pThread->m_pDbInterface==NULL)
        b=false;

    if(m_bCheckItemDataRecordTable==true)//check enable
    {
        //user input name empty? -> error
        sInput=pItem->text();
        if(sInput.length()<=0 && pItem->column()==0)
        {
            sError=QString::fromUtf8("Kein Datenfeld in der Spalte Name bzw. Bezeichnung darf leer sein!");
            b=false;
        }

        //check user input
        if(b==true && pItem!=NULL)
        {
            //check user input if type = number
            if(ui->comboBoxTyp->currentIndex()==0)//index 0 = type article -> has only number types
            {
                //search in allocation
                c=pItem->column();
                if(c>=0 && c<14)
                {
                    if(iaArticleTyp[c]==1)//int type
                    {
                        sInput.toInt(&b);
                        s=QString::fromUtf8("Ganzzahl");
                    }
                    if(iaArticleTyp[c]==2)//double type
                    {
                        sInput.toDouble(&b);
                        s=QString::fromUtf8("Kommazahl");
                    }

                    //input error
                    if(!b)
                        sError=QString::fromUtf8("Ihre Eingabe muss eine %1 sein! Siehe Spaltenzuordnung und Typ.").arg(s);
                }
            }

            if(ui->comboBoxTyp->currentIndex()!=3)//check by names (index 3 = customer -> name indifferent)
            {
                //check name by database & by table
                if(pItem->column()==0 && b==true)//column name?
                {
                    //name double? (check by database)
                    switch(iDataType)
                    {
                        case 0://article
                            sCondition=QString::fromUtf8("name = '%1' AND delete_status=0").arg(sInput);//search not delete article
                            c=m_pThread->m_pDbInterface->article_get_count(sCondition);
                            break;
                        case 1://dealer
                            c=m_pThread->m_pDbInterface->dealer_get_id(sInput);
                            break;
                        case 2://maker
                            c=m_pThread->m_pDbInterface->maker_get_id(sInput);
                            break;
                        case 4://waregroup
                            c=m_pThread->m_pDbInterface->waregroup_get_id(sInput);
                            break;
                        default:
                            break;
                    }
                    if(c>0)//found in db
                    {
                        sError=QString::fromUtf8("Einen Datensatz vom Typ '%1' mit dem Namen bzw. Bezeichnung ").arg(sDataTypeName);
                        sError+=QString::fromUtf8("'%1' gibt es schon in der Datenbank.").arg(sInput);
                        b=false;
                    }

                    //name double? (check by record table)
                    if(b)
                    {
                        for(c=0;c<ui->tableWidgetRecordData->rowCount();c++)
                        {
                            if(ui->tableWidgetRecordData->item(c,0)!=NULL && c!=pItem->row())
                            {
                                if(ui->tableWidgetRecordData->item(c,0)->text()==sInput)
                                {
                                    sError=QString::fromUtf8("Einen Datensatz mit dem Namen bzw. Bezeichnung '%1' ").arg(sInput);
                                    sError+=QString::fromUtf8("gibt es schon in der Tabelle in Zeile %1.").arg(c+1);
                                    b=false;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        //-
        if(!b)
        {//input error
            msg.setText(sError);
            msg.exec();
            pItem->setText(m_sMarkValueUserEditRecordTable);
        }
        //-
        m_sMarkValueUserEditRecordTable=QString::fromUtf8("");//clear mark
    }
    return b;
}

bool CInputDialogImport::check_update_double_names_recordtable(void)
{
    bool b=true,bTableUpdate=false;
    QString sCondition,sName,sName_new,s;
    int k,i,c,r,iDataType=ui->comboBoxTyp->currentIndex();
    QList<QList<int>*> lsDoubleInTable;
    QList<QList<QString>*> lsFreeNamesInDB;
    //-
    if(m_pThread==NULL)
        b=false;
    if(m_pThread->m_pDbInterface==NULL)
        b=false;

    //search double names
    if(b==true && iDataType!=3)//type 3==customer - name indifferent
    {
        for(c=0,r=0;r<ui->tableWidgetRecordData->rowCount();r++)
        {
            lsDoubleInTable.push_back(new QList<int>);
            if(lsDoubleInTable.last()!=NULL)
                c++;
        }
        for(r=0;r<ui->tableWidgetRecordData->rowCount();r++)
            lsFreeNamesInDB.push_back(new QList<QString>);

        //-
        if(c==lsDoubleInTable.count() && c==ui->tableWidgetRecordData->rowCount())
        {
            //search double names in record table
            for(r=0;r<ui->tableWidgetRecordData->rowCount();r++)
            {
                if(ui->tableWidgetRecordData->item(r,0)!=NULL)
                {
                    sName=ui->tableWidgetRecordData->item(r,0)->text();
                    for(i=0;i<ui->tableWidgetRecordData->rowCount();i++)
                    {
                        if(r!=i && lsDoubleInTable[i]->count()<=0)
                        {
                            if(ui->tableWidgetRecordData->item(i,0)!=NULL)
                            {
                                s=ui->tableWidgetRecordData->item(i,0)->text();
                                if(s==sName)//found
                                {
                                    if(lsDoubleInTable[r]->count()<=0)
                                        lsDoubleInTable[r]->push_back(r); //push back self
                                    lsDoubleInTable[r]->push_back(i);
                                    lsDoubleInTable[i]->push_back(-1);
                                }
                            }
                        }
                    }
                }
            }

            //search in db for free names -> double names
            for(r=0;r<lsDoubleInTable.count() && r<lsFreeNamesInDB.count();r++)
            {
                if(lsDoubleInTable[r]!=NULL && lsFreeNamesInDB[r]!=NULL)
                {
                    if(lsDoubleInTable[r]->count()>0 && lsDoubleInTable[r]->first()!=-1)
                    {
                        if(ui->tableWidgetRecordData->item(r,0)!=NULL)
                        {
                            sName_new=sName=ui->tableWidgetRecordData->item(r,0)->text();

                            //search db
                            for(k=0,i=0;k<lsDoubleInTable[r]->count();k++)
                            {
                                for(c=1;c>0;i++)
                                {
                                    if(i>0)
                                        sName_new=sName+QString::fromUtf8("_%1").arg(i);
                                    //-
                                    switch(iDataType)
                                    {
                                        case 0://article
                                        sCondition+=QString::fromUtf8("name = '%1' AND delete_status=0").arg(sName_new);//search not delete article
                                        c=m_pThread->m_pDbInterface->article_get_count(sCondition);
                                        break;
                                        case 1://dealer
                                        c=m_pThread->m_pDbInterface->dealer_get_id(sName_new);
                                        break;
                                        case 2://maker
                                        c=m_pThread->m_pDbInterface->maker_get_id(sName_new);
                                        break;
                                        case 4://waregroup
                                        c=m_pThread->m_pDbInterface->waregroup_get_id(sName_new);
                                        break;
                                        default:
                                        break;
                                    }
                                }
                                //-
                                lsFreeNamesInDB[r]->push_back(sName_new);
                            }
                        }
                    }
                }
            }

            //set free names in record-table
            for(i=0;i<lsDoubleInTable.count() && i<lsFreeNamesInDB.count();i++)
            {
                if(lsDoubleInTable[i]!=NULL && lsFreeNamesInDB[i]!=NULL)
                {
                    for(k=0;k<lsDoubleInTable[i]->count() && k<lsFreeNamesInDB[i]->count();k++)
                    {
                        if(ui->tableWidgetRecordData->item(lsDoubleInTable[i]->at(k),0)!=NULL)
                        {
                            ui->tableWidgetRecordData->item(lsDoubleInTable[i]->at(k),0)->setText(lsFreeNamesInDB[i]->at((k)));
                            bTableUpdate=true;
                        }
                    }
                }
            }

        }


        //check all names in table - exis. in db?
        for(r=0;r<ui->tableWidgetRecordData->rowCount();r++)
        {
            if(ui->tableWidgetRecordData->item(r,0)!=NULL)
            {
                sName_new=sName=ui->tableWidgetRecordData->item(r,0)->text();

                for(c=1,i=0,sCondition=QString::fromUtf8("");c>0;i++)
                {
                    if(i>0)
                        sName_new=sName+QString::fromUtf8("_%1").arg(i);
                    //-
                    switch(iDataType)
                    {
                        case 0://article
                        sCondition+=QString::fromUtf8("name = '%1' AND delete_status=0").arg(sName_new);//search not delete article
                        c=m_pThread->m_pDbInterface->article_get_count(sCondition);
                        break;
                        case 1://dealer
                        c=m_pThread->m_pDbInterface->dealer_get_id(sName_new);
                        break;
                        case 2://maker
                        c=m_pThread->m_pDbInterface->maker_get_id(sName_new);
                        break;
                        case 4://waregroup
                        c=m_pThread->m_pDbInterface->waregroup_get_id(sName_new);
                        break;
                        default:
                        break;
                    }
                }
                //-
                if(i>1)
                {//find in db
                    ui->tableWidgetRecordData->item(r,0)->setText(sName_new);//change item text (name + number)
                    bTableUpdate=true;
                }
            }
        }

        //set info text -> table updated
        if(bTableUpdate)
        {
            ui->label_error_3->setText(QString::fromUtf8("<font color='black'>Einige Namen bzw. Bezeichnungen wurden angepasst! (Grund: Mehrfachvergabe)</font>"));
            ui->label_error_3->setVisible(true);
        }
        else
        {
            ui->label_error_3->setText(QString::fromUtf8(""));
            ui->label_error_3->setVisible(false);
        }


        //clear
        for(i=0;i<lsDoubleInTable.count();i++)
        {
            if(lsDoubleInTable[i]!=NULL)
            {
                lsDoubleInTable[i]->clear();
                delete lsDoubleInTable[i];
            }
        }
        lsDoubleInTable.clear();
        //-
        for(i=0;i<lsFreeNamesInDB.count();i++)
        {
            if(lsFreeNamesInDB[i]!=NULL)
            {
                lsFreeNamesInDB[i]->clear();
                delete lsFreeNamesInDB[i];
            }
        }
        lsFreeNamesInDB.clear();
    }
    //-
    return b;
}

bool CInputDialogImport::import_button_clicked(void)
{
    bool b,bFound;
    CArticle ar;
    CMaker mk;
    CDealer de;
    CCustomer cu;
    CWaregroup wg;
    QString s,sCondition;
    double d;
    QList<QString> lsMark;
    QList<int> lsInt;
    QStringList sls;
    int id,p,j,i,r,c,iType=ui->comboBoxTyp->currentIndex();
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    //-
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            if(iType==0)//only type article
            {
                //maker
                //find maker-> unknow (in database)
                for(r=0;r<ui->tableWidgetRecordData->rowCount();r++)
                {
                    if(ui->tableWidgetRecordData->item(r,6)!=NULL)//column 6 = maker
                    {
                        s=ui->tableWidgetRecordData->item(r,6)->text();
                        if(s.length()>0)//set?
                        {
                            c=m_pThread->m_pDbInterface->maker_get_id(s);
                            if(c<=0)//maker not found in database?
                            {
                                //double in lsMark?
                                for(i=0,bFound=false;i<lsMark.count();i++)
                                {
                                    if(lsMark[i]==s)
                                    {
                                        bFound=true;
                                        break;
                                    }
                                }
                                if(!bFound)
                                    lsMark.push_back(s);
                            }
                        }
                    }
                }
                //question create??
                if(lsMark.count()>0)
                {
                    if(lsMark.count()==1)
                    {
                        s=QString::fromUtf8("Der Hersteller '%1' existiert nicht in der Datenbank!").arg(lsMark[0]);
                        msg.setText(s);
                        msg.setInformativeText("Soll dieser Hersteller angelegt werden?");
                    }
                    else
                    {
                        s=QString::fromUtf8("Es gibt %1 Hersteller die in der Datenbank nicht existieren!").arg(lsMark.count());
                        msg.setText(s);
                        msg.setInformativeText("Sollen diese Hersteller angelegt werden?");
                    }
                    msg.exec();
                    if(msg.clickedButton()==yesButton)
                    {
                        //add unknow maker
                        while(lsMark.count()>0)
                        {
                            mk.set_name(lsMark[0]);
                            m_pThread->m_pDbInterface->maker_add(mk);
                            lsMark.removeFirst();
                            mk.clear();
                        }
                    }
                }
                lsMark.clear();



                //waregroup
                //find waregroup-> unknow (in database)
                for(j=0,r=0;r<ui->tableWidgetRecordData->rowCount();r++)
                {
                    if(ui->tableWidgetRecordData->item(r,7)!=NULL)//column 7 = waregroup
                    {
                        s=ui->tableWidgetRecordData->item(r,7)->text();
                        if(s.length()>0)//set?
                        {
                            i=m_pThread->m_pDbInterface->waregroup_get_unknow_count(s,p);
                            j+=i;//count unknow
                            //-
                            if(i>0)
                            {//unknow'S?
                                lsMark.push_back(s);//mark path
                                lsInt.push_back(p);//mark last exis. parent

                                //search double in mark_list
                                for(i=0,bFound=false;i<lsMark.count()-1;i++)
                                {
                                    if(lsMark[i]==s)
                                    {
                                        bFound=true;
                                        break;
                                    }
                                }
                                if(bFound)
                                {
                                    lsMark.removeLast();//is double -> delete
                                    lsInt.removeLast();//  -"-
                                }
                            }
                        }
                    }
                }
                //question create??
                if(j>0)
                {
                    s=QString::fromUtf8("Es gibt %1 Warengruppen die in der Datenbank nicht existieren!").arg(j);
                    msg.setText(s);
                    msg.setInformativeText("Sollen diese Warengruppen angelegt werden?");
                    msg.exec();
                    if(msg.clickedButton()==yesButton)
                    {
                        //add unknow maker
                        while(lsMark.count()>0 && lsInt.count()>0)
                        {
                            s=lsMark[0];
                            sls=s.split("/");
                            id=lsInt[0];//last exis. parent id
                            while(sls.count()>0)
                            {
                                wg.set_parent_id(id);
                                wg.set_name(sls[0]);
                                m_pThread->m_pDbInterface->waregroup_add(wg);
                                id=wg.get_id();//set parent id -> last add waregroup
                                //-
                                sls.removeFirst();
                                wg.clear();
                            }
                            sls.clear();
                            lsMark.removeFirst();
                            lsInt.removeFirst();

                        }
                    }
                }
                lsMark.clear();
                lsInt.clear();
            }




            //import
            for(r=0;r<ui->tableWidgetRecordData->rowCount();r++)
            {
                //clear
                ar.clear();
                mk.clear();
                de.clear();
                cu.clear();
                wg.clear();


                for(c=0;c<ui->tableWidgetRecordData->columnCount();c++)
                {
                    if(ui->tableWidgetRecordData->item(r,c)!=NULL)
                    {
                        s=ui->tableWidgetRecordData->item(r,c)->text();
                        //-
                        switch(iType)
                        {
                            //article
                            case 0:
                                if(c==0)//name
                                    ar.set_name(s);
                                if(c==1 || c==2)//inventory
                                {
                                    i=s.toInt(&b);
                                    if(b)
                                        ar.set_inventory(i);
                                }
                                if(c==2)//max.inventory
                                {
                                    i=s.toInt(&b);
                                    if(b)
                                        ar.set_max_inventory(i);
                                }
                                if(c==3)//unit
                                    ar.set_unit(s);
                                if(c==4)//1.articlenumber
                                    ar.set_articlenumber(s);
                                if(c==5)//2.   "
                                    ar.set_articlenumber2(s);
                                if(c==6)//maker
                                {
                                    i=m_pThread->m_pDbInterface->maker_get_id(s);
                                    ar.set_maker_id(i);
                                }
                                if(c==7)//waregroup
                                {
                                    i=m_pThread->m_pDbInterface->waregroup_get_id(s);
                                    ar.set_waregroup_id(i);
                                }
                                if(c==8)//warning
                                {
                                    i=s.toInt(&b);
                                    if(b)
                                        ar.set_warning_limit(i);
                                }
                                if(c==9)//base price
                                {
                                    d=s.toDouble(&b);
                                    if(b)
                                        ar.set_base_price(d);
                                }
                                if(c==10)//sale price
                                {
                                    d=s.toDouble(&b);
                                    if(b)
                                        ar.set_sales_price(d);
                                }
                                if(c==11)//valuta
                                    ar.set_valuta(s);
                                if(c==12)//location
                                    ar.set_location(s);
                                if(c==13)//comment
                                    ar.set_comment(s);
                                break;

                            //dealer
                            case 1:
                                if(c==0)
                                    de.set_name(s);
                                if(c==1)
                                    de.set_adress(s);
                                if(c==2)
                                    de.set_callnumber(s);
                                if(c==3)
                                    de.set_faxnumber(s);
                                if(c==4)
                                    de.set_email(s);
                                if(c==5)
                                    de.set_homepage(s);
                                if(c==6)
                                    de.set_contectperson(s);
                                if(c==7)
                                    de.set_customernumber(s);
                                if(c==8)
                                    de.set_comment(s);
                                break;

                            //maker
                            case 2:
                                if(c==0)
                                    mk.set_name(s);
                                if(c==1)
                                    mk.set_adress(s);
                                if(c==2)
                                    mk.set_callnumber(s);
                                if(c==3)
                                    mk.set_faxnumber(s);
                                if(c==4)
                                    mk.set_email(s);
                                if(c==5)
                                    mk.set_homepage(s);
                                if(c==6)
                                    mk.set_contectperson(s);
                                if(c==7)
                                    mk.set_comment(s);
                                break;

                            //customer
                            case 3:
                                if(c==0)
                                    cu.set_name(s);
                                if(c==1)
                                    cu.set_first_name(s);
                                if(c==2)
                                    cu.set_street(s);
                                if(c==3)
                                    cu.set_postcode(s);
                                if(c==4)
                                    cu.set_city(s);
                                if(c==5)
                                    cu.set_customernumber(s);
                                if(c==6)
                                    cu.set_callnumber(s);
                                if(c==7)
                                    cu.set_faxnumber(s);
                                if(c==8)
                                    cu.set_email(s);
                                if(c==9)
                                    cu.set_comment(s);
                                break;

                            //waregroup
                            case 4:
                                if(c==0)
                                {
                                    //split path -> create parents waregroups
                                    sls=s.split("/");
                                    for(i=-1;sls.count()>1;)
                                    {
                                        if(sls[0]!=QString::fromUtf8(""))
                                        {
                                            id=m_pThread->m_pDbInterface->waregroup_get_id(sls[0],i);
                                            if(id<=0)
                                            {//not found
                                                wg.clear();
                                                wg.set_name(sls[0]);
                                                m_pThread->m_pDbInterface->waregroup_add(wg);//add
                                                i=wg.get_id();
                                            }
                                            else
                                                i=id;
                                        }
                                        sls.removeFirst();
                                    }
                                    //-
                                    s=sls[0];
                                    sls.clear();

                                    //set target waregroup
                                    wg.set_name(s);
                                    wg.set_parent_id(i);
                                    wg.set_comment(s=ui->tableWidgetRecordData->item(r,1)->text());
                                }
                                c++;
                                break;

                            //-
                            default:
                                break;
                        }
                    }
                }

                //add to db
                if(iType==0)//article
                    m_pThread->m_pDbInterface->article_add(ar);
                if(iType==1)//dealer
                    m_pThread->m_pDbInterface->dealer_add(de);
                if(iType==2)//maker
                    m_pThread->m_pDbInterface->maker_add(mk);
                if(iType==3)//customer
                    m_pThread->m_pDbInterface->customer_add(cu);
                if(iType==4)//waregroup
                    m_pThread->m_pDbInterface->waregroup_add(wg);
            }
        }
        //-
        done(1);
    }
    //-
    return true;
}
