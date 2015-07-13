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

#include "cinputdialogexport.h"
#include "ui_cinputdialogexport.h"

CInputDialogExport::CInputDialogExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogExport)
{
    ui->setupUi(this);
    setWindowTitle(QString::fromUtf8("Export-Einstellung"));
    //-
    settings(false);//load & set settings
    //-
    update_data_examble();
    //-
    connect(ui->radioButtonCom,SIGNAL(clicked()),this,SLOT(update_data_examble()));
    connect(ui->radioButtonOther,SIGNAL(clicked()),this,SLOT(update_data_examble()));
    connect(ui->radioButtonSemi,SIGNAL(clicked()),this,SLOT(update_data_examble()));
    connect(ui->radioButtonSpace,SIGNAL(clicked()),this,SLOT(update_data_examble()));
    connect(ui->radioButtonTab,SIGNAL(clicked()),this,SLOT(update_data_examble()));
    connect(ui->lineEditSplitChar,SIGNAL(textChanged(QString)),this,SLOT(update_data_examble()));
    connect(ui->comboBoxTextSpliter,SIGNAL(currentIndexChanged(QString)),this,SLOT(update_data_examble()));
}

CInputDialogExport::~CInputDialogExport()
{
    settings(true);//save settings if update
    delete ui;
}

bool CInputDialogExport::settings(bool bUpdate)
{
    bool b;
    QString s;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;

    lsSType.push_back(QString::fromUtf8("DLG_EXPORT_CSV_DIALOG_CODEC"));
    lsSType.push_back(QString::fromUtf8("DLG_EXPORT_CSV_DIALOG_SPLIT_TEXT_CHAR"));
    lsSType.push_back(QString::fromUtf8("DLG_EXPORT_CSV_DIALOG_SPLIT_CHAR"));
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
            //radio button's split char
            if(lsSValue.count()>2)
            {
                if(lsSValue[2]==QString::fromUtf8(";"))
                    ui->radioButtonSemi->setChecked(true);
                else if(lsSValue[2]==QString::fromUtf8(","))
                    ui->radioButtonCom->setChecked(true);
                else if(lsSValue[2]==QString::fromUtf8("\t"))
                    ui->radioButtonTab->setChecked(true);
                else if(lsSValue[2]==QString::fromUtf8(" "))
                    ui->radioButtonSpace->setChecked(true);
                else
                {
                    ui->radioButtonOther->setChecked(true);
                    ui->lineEditSplitChar->setText(lsSValue[2]);
                }
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
            //radio button's split char
            if(lsSValue.length()>2)
            {
                if(ui->radioButtonSemi->isChecked())
                    s=QString::fromUtf8(";");
                else if(ui->radioButtonCom->isChecked())
                    s=QString::fromUtf8(",");
                else if(ui->radioButtonTab->isChecked())
                    s=QString::fromUtf8("\t");
                else if(ui->radioButtonSpace->isChecked())
                    s=QString::fromUtf8(" ");
                else
                    s=ui->lineEditSplitChar->text();
                if(s!=lsSValue[2])
                {
                    lsSUpdateType.push_back(lsSType[2]);
                    lsSUpdateValue.push_back(s);
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

bool CInputDialogExport::get_data(QString & sSplitChar, QString & sSplitText, int & iCodecType)
{
    //text splitter
    if(ui->comboBoxTextSpliter->currentIndex()!=2)//not nothing text splitter
        sSplitText=ui->comboBoxTextSpliter->currentText();

    //split char
    if(ui->radioButtonSemi->isChecked())
        sSplitChar=QString::fromUtf8(";");
    if(ui->radioButtonCom->isChecked())
        sSplitChar=QString::fromUtf8(",");
    if(ui->radioButtonTab->isChecked())
        sSplitChar=QString::fromUtf8("\t");
    if(ui->radioButtonSpace->isChecked())
        sSplitChar+=QString::fromUtf8(" ");
    if(ui->radioButtonOther->isChecked())
        sSplitChar=ui->lineEditSplitChar->text();

    //codec
    iCodecType=ui->comboBoxCodec->currentIndex(); //0=UTF-8  1=Windows 1252
    //-
    return true;
}

bool CInputDialogExport::update_data_examble(void)
{
    int i;
    QString s,sTextSplit,sCharSplit;

    //get data
    get_data(sCharSplit,sTextSplit,i);

    //examble text
    s=QString::fromUtf8("%1Beispiel_Tabelle_1%2%3").arg(sTextSplit,sTextSplit,sCharSplit);
    s+=QString::fromUtf8("%1Beispiel_Tabelle_2%2%3").arg(sTextSplit,sTextSplit,sCharSplit);
    s+=QString::fromUtf8("%1...%2%3").arg(sTextSplit,sTextSplit,sCharSplit);
    s+=QString::fromUtf8("%1Beispiel_Tabelle_n%2").arg(sTextSplit,sTextSplit);

    //update line
    ui->lineEditExamble->setText(s);
    //-
    return true;
}
