/***************************************************************************
 *   This file is part of netctl-plasmoid                                  *
 *                                                                         *
 *   netctl-plasmoid is free software: you can redistribute it and/or      *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   netctl-plasmoid is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with netctl-plasmoid. If not, see http://www.gnu.org/licenses/  *
 ***************************************************************************/

#include "pppoewidget.h"
#include "ui_pppoewidget.h"

#include <QDir>
#include <QFileDialog>
#include <QFile>


PppoeWidget::PppoeWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PppoeWidget)
{
    ui->setupUi(this);
    createActions();
    createFilter();
    clear();
}


PppoeWidget::~PppoeWidget()
{
    delete ui;
}


void PppoeWidget::clear()
{
    ui->lineEdit_username->clear();
    ui->lineEdit_password->clear();
    ui->comboBox_connection->setCurrentIndex(0);
    changeMode(ui->comboBox_connection->currentText());
    ui->spinBox_timeout->setValue(300);
    ui->spinBox_fail->setValue(5);
    ui->checkBox_route->setCheckState(Qt::Checked);
    ui->checkBox_dns->setCheckState(Qt::Checked);
    ui->lineEdit_unit->clear();
    ui->spinBox_lcpInterval->setValue(30);
    ui->spinBox_lcpFailure->setValue(4);
    ui->lineEdit_options->clear();
    ui->lineEdit_service->clear();
    ui->lineEdit_ac->clear();
    ui->lineEdit_session->clear();
    ui->lineEdit_mac->clear();
    ui->checkBox_ipv6->setCheckState(Qt::Unchecked);

    ui->pushButton_pppoeAdvanced->setChecked(false);;
    showAdvanced();
}


void PppoeWidget::setShown(const bool state)
{
    if (state)
        show();
    else
        hide();
}


void PppoeWidget::createFilter()
{
    // session mac
    ui->lineEdit_session->setInputMask(QString(">99999:HH:HH:HH:HH:HH:HH"));
    // mac
    ui->lineEdit_mac->setInputMask(QString(">HH:HH:HH:HH:HH:HH"));
}


void PppoeWidget::createActions()
{
    connect(ui->pushButton_pppoeAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->comboBox_connection, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeMode(QString)));
    connect(ui->pushButton_options, SIGNAL(clicked(bool)), this, SLOT(selectOptionsFile()));
}


void PppoeWidget::changeMode(const QString currentText)
{
    if (currentText == QString("persist"))
        ui->widget_timeout->setHidden(true);
    else if (currentText == QString("demand"))
        ui->widget_timeout->setShown(true);
}


void PppoeWidget::selectOptionsFile()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("PppoeWidget", "Select options file"),
                QDir::currentPath(),
                QApplication::translate("PppoeWidget", "Configuration files (*.conf)"));
    if (!filename.isEmpty())
        ui->lineEdit_options->setText(filename);
}


void PppoeWidget::showAdvanced()
{
    if (ui->pushButton_pppoeAdvanced->isChecked()) {
        ui->widget_pppoeAdvanced->setShown(true);
        ui->pushButton_pppoeAdvanced->setText(QApplication::translate("PppoeWidget", "Hide advanced"));
    }
    else {
        ui->widget_pppoeAdvanced->setHidden(true);
        ui->pushButton_pppoeAdvanced->setText(QApplication::translate("PppoeWidget", "Show advanced"));
    }
}


QMap<QString, QString> PppoeWidget::getSettings()
{
    QMap<QString, QString> pppoeSettings;

    if (isOk() == 0) {
        if (!ui->lineEdit_username->text().isEmpty())
            pppoeSettings[QString("User")] = QString("'") + ui->lineEdit_username->text() + QString("'");
        if (!ui->lineEdit_password->text().isEmpty())
            pppoeSettings[QString("Password")] = QString("'") + ui->lineEdit_password->text() + QString("'");
        pppoeSettings[QString("ConnectionMode")] = QString("'") + ui->comboBox_connection->currentText() + QString("'");
        if (ui->comboBox_connection->currentText() == QString("demand"))
            pppoeSettings[QString("IdleTimeout")] = QString(ui->spinBox_timeout->value());
        if (ui->spinBox_fail->value() != 5)
            pppoeSettings[QString("MaxFail")] = QString(ui->spinBox_fail->value());
        if (ui->checkBox_route->checkState() == Qt::Unchecked)
            pppoeSettings[QString("DefaultRoute")] = QString("false");
        if (ui->checkBox_dns->checkState() == Qt::Unchecked)
            pppoeSettings[QString("UsePeerDNS")] = QString("false");
        if (!ui->lineEdit_unit->text().isEmpty())
            pppoeSettings[QString("PPPUnit")] = ui->lineEdit_unit->text();
        if (ui->spinBox_lcpInterval->value() != 30)
            pppoeSettings[QString("LCPEchoInterval")] = QString(ui->spinBox_lcpInterval->value());
        if (ui->spinBox_lcpFailure->value() != 4)
            pppoeSettings[QString("LCPEchoFailure")] = QString(ui->spinBox_lcpFailure->value());
        if (!ui->lineEdit_options->text().isEmpty())
            pppoeSettings[QString("OptionsFile")] = QString("'") + ui->lineEdit_options->text() + QString("'");
        if (!ui->lineEdit_service->text().isEmpty())
            pppoeSettings[QString("PPPoEService")] = QString("'") + ui->lineEdit_service->text() + QString("'");
        if (!ui->lineEdit_ac->text().isEmpty())
            pppoeSettings[QString("PPPoEAC")] = QString("'") + ui->lineEdit_ac->text() + QString("'");
        if (!ui->lineEdit_session->text().split(QString(":")).join(QString("")).remove(QString(" ")).isEmpty())
            pppoeSettings[QString("PPPoESession")] = QString("'") + ui->lineEdit_session->text().remove(QString(" ")) + QString("'");
        if (!ui->lineEdit_mac->text().split(QString(":")).join(QString("")).remove(QString(" ")).isEmpty())
            pppoeSettings[QString("PPPoEMAC")] = QString("'") + ui->lineEdit_mac->text() + QString("'");
        if (ui->checkBox_ipv6->checkState() == Qt::Checked)
            pppoeSettings[QString("PPPoEIP6")] = QString("yes");
    }

    return pppoeSettings;
}


int PppoeWidget::isOk()
{
    // config file doesn't exist
    if (!ui->lineEdit_options->text().isEmpty())
        if (!QFile(ui->lineEdit_options->text()).exists())
            return 1;
    // mac address
    if (!ui->lineEdit_mac->text().split(QString(":")).join(QString("")).remove(QString(" ")).isEmpty())
        if (ui->lineEdit_mac->text().indexOf(QString(" ")) > -1)
            return 2;
    // session id is not set
    if (!ui->lineEdit_session->text().split(QString(":")).join(QString("")).remove(QString(" ")).isEmpty())
        if (ui->lineEdit_session->text().split(QString(":"))[0].remove(QString(" ")).isEmpty())
            return 3;
    // session mac address
    if (!ui->lineEdit_session->text().split(QString(":")).join(QString("")).remove(QString(" ")).isEmpty()) {
        QStringList item = ui->lineEdit_session->text().split(QString(":"));
        for (int i=1; i<7; i++)
            if (item[i].indexOf(QString(" ")) > -1)
                return 4;
    }
    // all fine
    return 0;
}


void PppoeWidget::setSettings(QMap<QString, QString> settings)
{
    if (settings.contains(QString("User")))
        ui->lineEdit_username->setText(settings[QString("User")].remove(QString("'")));
    if (settings.contains(QString("Password")))
        ui->lineEdit_password->setText(settings[QString("Password")].remove(QString("'")));
    if (settings.contains(QString("ConnectionMode")))
        for (int i=0; i<ui->comboBox_connection->count(); i++)
            if (settings[QString("ConnectionMode")].remove(QString("'")) == ui->comboBox_connection->itemText(i))
                ui->comboBox_connection->setCurrentIndex(i);
    if (settings.contains(QString("IdleTimeout")))
        ui->spinBox_timeout->setValue(settings[QString("IdleTimeout")].toInt());
    if (settings.contains(QString("MaxFail")))
        ui->spinBox_fail->setValue(settings[QString("MaxFail")].toInt());
    if (settings.contains(QString("DefaultRoute")))
        if (settings[QString("DefaultRoute")].remove(QString("'")) == QString("false"))
            ui->checkBox_route->setCheckState(Qt::Unchecked);
    if (settings.contains(QString("UsePeerDNS")))
        if (settings[QString("UsePeerDNS")].remove(QString("'")) == QString("false"))
            ui->checkBox_dns->setCheckState(Qt::Unchecked);
    if (settings.contains(QString("PPPUnit")))
        ui->lineEdit_unit->setText(settings[QString("PPPUnit")]);
    if (settings.contains(QString("LCPEchoInterval")))
        ui->spinBox_lcpInterval->setValue(settings[QString("LCPEchoInterval")].toInt());
    if (settings.contains(QString("LCPEchoFailure")))
        ui->spinBox_lcpFailure->setValue(settings[QString("LCPEchoFailure")].toInt());
    if (settings.contains(QString("OptionsFile")))
        ui->lineEdit_options->setText(settings[QString("OptionsFile")].remove(QString("'")));
    if (settings.contains(QString("PPPoEService")))
        ui->lineEdit_service->setText(settings[QString("PPPoEService")].remove(QString("'")));
    if (settings.contains(QString("PPPoEAC")))
        ui->lineEdit_ac->setText(settings[QString("PPPoEAC")].remove(QString("'")));
    if (settings.contains(QString("PPPoESession")))
        ui->lineEdit_session->setText(settings[QString("PPPoESession")].remove(QString("'")));
    if (settings.contains(QString("PPPoEMAC")))
        ui->lineEdit_mac->setText(settings[QString("PPPoEMAC")].remove(QString("'")));
    if (settings.contains(QString("PPPoEIP6")))
        if (settings[QString("PPPoEIP6")] == QString("yes"))
            ui->checkBox_dns->setCheckState(Qt::Checked);

    changeMode(ui->comboBox_connection->currentText());
}
