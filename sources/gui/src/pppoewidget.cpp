/***************************************************************************
 *   This file is part of netctl-gui                                       *
 *                                                                         *
 *   netctl-gui is free software: you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   netctl-gui is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with netctl-gui. If not, see http://www.gnu.org/licenses/       *
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


void PppoeWidget::createFilter()
{
    // session mac
    ui->lineEdit_session->setInputMask(QString(">00000:hh:hh:hh:hh:hh:hh"));
    // mac
    ui->lineEdit_mac->setInputMask(QString(">hh:hh:hh:hh:hh:hh"));
}


void PppoeWidget::createActions()
{
    connect(ui->pushButton_pppoeAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->comboBox_connection, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeMode(QString)));
    connect(ui->pushButton_options, SIGNAL(clicked(bool)), this, SLOT(selectOptionsFile()));
}


void PppoeWidget::changeMode(const QString currentText)
{
    ui->widget_timeout->setHidden(currentText == QString("persist"));
}


void PppoeWidget::selectOptionsFile()
{
    QString filename = QFileDialog::getOpenFileName(this,
                QApplication::translate("PppoeWidget", "Select options file"),
                QDir::currentPath(),
                QApplication::translate("PppoeWidget", "Configuration files (*.conf)"));
    if (!filename.isEmpty())
        ui->lineEdit_options->setText(filename);
}


void PppoeWidget::showAdvanced()
{
    if (ui->pushButton_pppoeAdvanced->isChecked()) {
        ui->widget_pppoeAdvanced->setHidden(false);
        ui->pushButton_pppoeAdvanced->setArrowType(Qt::UpArrow);
    } else {
        ui->widget_pppoeAdvanced->setHidden(true);
        ui->pushButton_pppoeAdvanced->setArrowType(Qt::DownArrow);
    }
}


QMap<QString, QString> PppoeWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    if (!ui->lineEdit_username->text().isEmpty())
        settings[QString("User")] = QString("'%1'").arg(ui->lineEdit_username->text());
    if (!ui->lineEdit_password->text().isEmpty())
        settings[QString("Password")] = QString("'%1'").arg(ui->lineEdit_password->text());
    settings[QString("ConnectionMode")] = QString("'%1'").arg(ui->comboBox_connection->currentText());
    if (ui->comboBox_connection->currentText() == QString("demand"))
        settings[QString("IdleTimeout")] = QString::number(ui->spinBox_timeout->value());
    if (ui->spinBox_fail->value() != 5)
        settings[QString("MaxFail")] = QString::number(ui->spinBox_fail->value());
    if (ui->checkBox_route->checkState() == Qt::Unchecked)
        settings[QString("DefaultRoute")] = QString("false");
    if (ui->checkBox_dns->checkState() == Qt::Unchecked)
        settings[QString("UsePeerDNS")] = QString("false");
    if (!ui->lineEdit_unit->text().isEmpty())
        settings[QString("PPPUnit")] = ui->lineEdit_unit->text();
    if (ui->spinBox_lcpInterval->value() != 30)
        settings[QString("LCPEchoInterval")] = QString::number(ui->spinBox_lcpInterval->value());
    if (ui->spinBox_lcpFailure->value() != 4)
        settings[QString("LCPEchoFailure")] = QString::number(ui->spinBox_lcpFailure->value());
    if (!ui->lineEdit_options->text().isEmpty())
        settings[QString("OptionsFile")] = QString("'%1'").arg(ui->lineEdit_options->text());
    if (!ui->lineEdit_service->text().isEmpty())
        settings[QString("PPPoEService")] = QString("'%1'").arg(ui->lineEdit_service->text());
    if (!ui->lineEdit_ac->text().isEmpty())
        settings[QString("PPPoEAC")] = QString("'%1'").arg(ui->lineEdit_ac->text());
    if (!ui->lineEdit_session->text().remove(QChar(':')).isEmpty())
        settings[QString("PPPoESession")] = QString("'%1'").arg(ui->lineEdit_session->text());
    if (!ui->lineEdit_mac->text().remove(QChar(':')).isEmpty())
        settings[QString("PPPoEMAC")] = QString("'%1'").arg(ui->lineEdit_mac->text());
    if (ui->checkBox_ipv6->checkState() == Qt::Checked)
        settings[QString("PPPoEIP6")] = QString("yes");

    return settings;
}


int PppoeWidget::isOk()
{
    // config file doesn't exist
    if (!ui->lineEdit_options->text().isEmpty())
        if (!QFile(ui->lineEdit_options->text()).exists())
            return 1;
    // mac address
    if (!ui->lineEdit_mac->text().remove(QChar(':')).isEmpty())
        if (ui->lineEdit_mac->text().length() != (6 * 2 + 5))
            return 2;
    // session id is not set
    if (!ui->lineEdit_session->text().remove(QChar(':')).isEmpty())
        if (ui->lineEdit_session->text().split(QChar(':'))[0].isEmpty())
            return 3;
    // session mac address
    if (!ui->lineEdit_session->text().remove(QChar(':')).isEmpty()) {
        QStringList item = ui->lineEdit_session->text().split(QChar(':'));
        item.removeFirst();
        if (item.join(QChar(':')).length() != (6 * 2 + 5))
            return 4;
    }
    // all fine
    return 0;
}


void PppoeWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("User")))
        ui->lineEdit_username->setText(settings[QString("User")]);
    if (settings.contains(QString("Password")))
        ui->lineEdit_password->setText(settings[QString("Password")]);
    if (settings.contains(QString("ConnectionMode"))) {
        int index = ui->comboBox_connection->findText(settings[QString("ConnectionMode")]);
        ui->comboBox_connection->setCurrentIndex(index);
    }
    if (settings.contains(QString("IdleTimeout")))
        ui->spinBox_timeout->setValue(settings[QString("IdleTimeout")].toInt());
    if (settings.contains(QString("MaxFail")))
        ui->spinBox_fail->setValue(settings[QString("MaxFail")].toInt());
    if (settings.contains(QString("DefaultRoute")))
        if (settings[QString("DefaultRoute")] == QString("false"))
            ui->checkBox_route->setCheckState(Qt::Unchecked);
    if (settings.contains(QString("UsePeerDNS")))
        if (settings[QString("UsePeerDNS")] == QString("false"))
            ui->checkBox_dns->setCheckState(Qt::Unchecked);
    if (settings.contains(QString("PPPUnit")))
        ui->lineEdit_unit->setText(settings[QString("PPPUnit")]);
    if (settings.contains(QString("LCPEchoInterval")))
        ui->spinBox_lcpInterval->setValue(settings[QString("LCPEchoInterval")].toInt());
    if (settings.contains(QString("LCPEchoFailure")))
        ui->spinBox_lcpFailure->setValue(settings[QString("LCPEchoFailure")].toInt());
    if (settings.contains(QString("OptionsFile")))
        ui->lineEdit_options->setText(settings[QString("OptionsFile")]);
    if (settings.contains(QString("PPPoEService")))
        ui->lineEdit_service->setText(settings[QString("PPPoEService")]);
    if (settings.contains(QString("PPPoEAC")))
        ui->lineEdit_ac->setText(settings[QString("PPPoEAC")]);
    if (settings.contains(QString("PPPoESession")))
        ui->lineEdit_session->setText(settings[QString("PPPoESession")]);
    if (settings.contains(QString("PPPoEMAC")))
        ui->lineEdit_mac->setText(settings[QString("PPPoEMAC")]);
    if (settings.contains(QString("PPPoEIP6")))
        if (settings[QString("PPPoEIP6")] == QString("yes"))
            ui->checkBox_dns->setCheckState(Qt::Checked);

    changeMode(ui->comboBox_connection->currentText());
}
