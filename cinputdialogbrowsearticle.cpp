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

#include "cinputdialogbrowsearticle.h"
#include "ui_cinputdialogbrowsearticle.h"

CInputDialogBrowseArticle::CInputDialogBrowseArticle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogBrowseArticle)
{
    ui->setupUi(this);
    m_pThread=NULL;

    ui->buttonBox->setEnabled(false);
    settings(false);//load & set settings

    setMaximumSize(width(),height());
    setMinimumSize(width(),height());

    //connects
    connect(ui->pushButtonMaskSet,SIGNAL(clicked()),this,SLOT(update_table()));
    connect(ui->checkBoxAuto,SIGNAL(clicked()),this,SLOT(checkbox_auto_clicked()));
    connect(ui->lineEditMask,SIGNAL(textChanged(QString)),this,SLOT(mask_edit()));
    connect(ui->comboBoxMask,SIGNAL(currentIndexChanged(QString)),this,SLOT(mask_edit()));
    connect(ui->tableWidgetArticle,SIGNAL(itemSelectionChanged()),this,SLOT(check_user_input()));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(close()));
}

CInputDialogBrowseArticle::~CInputDialogBrowseArticle()
{
    settings(true);//save settings if update
    delete ui;
}

void CInputDialogBrowseArticle::keyPressEvent(QKeyEvent * event)
{
    if(event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return)
    {
        if(ui->lineEditMask->hasFocus())
            update_table();
    }
}

bool CInputDialogBrowseArticle::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;
    lsSType.push_back(QString("DLG_ARTICLE_BROWSE_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_ARTICLE_BROWSE"));
    lsSType.push_back(QString("AUTO_CHECKBOX_ARTICLE_BROWSE"));
    lsSType.push_back(QString("MASK_SELECTION_DLG_ARTICLE_BROWSE"));
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //table width
            if(lsSValue.count()>0)
                settings.set_table_columns_width(ui->tableWidgetArticle,lsSValue[0],200);
            //table sort
            if(lsSValue.count()>1)
                settings.set_table_sort(ui->tableWidgetArticle,lsSValue[1],0);
            //auto checkbox
            if(lsSValue.count()>2)
                settings.set_checkbox(ui->checkBoxAuto,lsSValue[2],false);
            //mask selection
            if(lsSValue.count()>3)
                settings.set_combobox(ui->comboBoxMask,lsSValue[3],0);
        }
        if(bUpdate)//write
        {
            //table width
            if(lsSValue.count()>0)
            {
                if(settings.get_table_columns_width(ui->tableWidgetArticle,lsSValue[0]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[0]);
                    lsSUpdateValue.push_back(lsSValue[0]);
                }
            }
            //table sort
            if(lsSValue.count()>1)
            {
                if(settings.get_table_sort(ui->tableWidgetArticle,lsSValue[1]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[1]);
                    lsSUpdateValue.push_back(lsSValue[1]);
                }
            }
            //checkbox
            if(lsSValue.count()>2)
            {
                if(settings.get_checkbox(ui->checkBoxAuto,lsSValue[2]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[2]);
                    lsSUpdateValue.push_back(lsSValue[2]);
                }
            }
            //combobox
            if(lsSValue.count()>3)
            {
                if(settings.get_combobox(ui->comboBoxMask,lsSValue[3]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[3]);
                    lsSUpdateValue.push_back(lsSValue[3]);
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

void CInputDialogBrowseArticle::checkbox_auto_clicked(void)
{
    if(ui->checkBoxAuto->isChecked())
        ui->pushButtonMaskSet->setEnabled(false);
    else
        ui->pushButtonMaskSet->setEnabled(true);
}

void CInputDialogBrowseArticle::mask_edit(void)
{
    if(ui->checkBoxAuto->isChecked())
        update_table();
}

bool CInputDialogBrowseArticle::check_user_input(void)
{
    QList <QTableWidgetItem*> ls=ui->tableWidgetArticle->selectedItems();
    if(ls.count()<=0)
        ui->buttonBox->setEnabled(false);
    else
        ui->buttonBox->setEnabled(true);
    return true;
}

bool CInputDialogBrowseArticle::update_table(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    bool b;
    QString sCondition;
    QString sMask=ui->lineEditMask->text();
    int iMask=ui->comboBoxMask->currentIndex();
    int iFormatType=FORMAT_THREE;
    QList<int> lsIds;
    CPointerMemory memory;
    memory.set_int(&iFormatType);
    memory.set_string(&sCondition);
    memory.set_int_list(&lsIds);

    //-condition-
    if(sMask.length()<=0)//mask not set?
        sCondition=QString("");
    else
    {
        if(iMask==0)//name?
            sCondition=QString("name like '%%1%'").arg(sMask);
        else if(iMask==1)//number
            sCondition=QString("articlenumber like '%%1%'").arg(sMask);
        else if(iMask==2)//2.number
            sCondition=QString("articlenumber2 like '%%1%'").arg(sMask);
        else if(iMask==5)//location
            sCondition=QString("location like '%%1%'").arg(sMask);
        else
            sCondition=QString(sMask);
    }

    //-get article by mask-
    if(iMask==0 || iMask==1 || iMask==2 || iMask==5 || sMask.length()<=0)//name or numbers or location or no mask
        b=m_pThread->set_work(WORK_ARTICLE_GET_ALL,&memory);
    else if(iMask==3)//maker?
        b=m_pThread->set_work(WORK_ARTICLE_GET_ALL_BY_MAKER,&memory);
    else//waregroup?
        b=m_pThread->set_work(WORK_ARTICLE_GET_ALL_BY_WAREGROUP,&memory);

    //-fill table-
    if(b)
    {
        memory.set_tablewidget(ui->tableWidgetArticle);
        b=m_pThread->set_work(WORK_ARTICLE_UPDATE_TABLEWIDGET,&memory);
    }

    if(!b)//db error
    {
    }
    //-
    ui->buttonBox->setEnabled(false);
    //-
    return b;
}

bool CInputDialogBrowseArticle::get_data(int & iArticleId)
{
    bool b=false;

    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pWidgets!=NULL)
        {
            if(m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetArticle,iArticleId))
                b=true;
        }
    }
    return b;
}

bool CInputDialogBrowseArticle::set_data(int iArticleId)
{
    bool b=false;
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pWidgets!=NULL)
        {
            update_table();
            if(iArticleId>=0)
                b=m_pThread->m_pWidgets->find_and_select_row(ui->tableWidgetArticle,iArticleId);
        }
    }
    return b;
}
