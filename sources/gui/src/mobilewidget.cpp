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

#include "mobilewidget.h"
#include "ui_mobilewidget.h"

#include <QDir>
#include <QFileDialog>
#include <QFile>


MobileWidget::MobileWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MobileWidget)
{
    ui->setupUi(this);
    createActions();
    clear();
}


MobileWidget::~MobileWidget()
{
    delete ui;
}


void MobileWidget::clear()
{
    ui->lineEdit_username->clear();
    ui->lineEdit_password->clear();
    ui->lineEdit_apn->clear();
    ui->lineEdit_pin->clear();
    ui->comboBox_mode->setCurrentIndex(0);
    ui->spinBox_fail->setValue(5);
    ui->checkBox_route->setCheckState(Qt::Checked);
    ui->checkBox_dns->setCheckState(Qt::Checked);
    ui->lineEdit_options->clear();

    ui->pushButton_mobileAdvanced->setChecked(false);;
    showAdvanced();
}


void MobileWidget::createActions()
{
    connect(ui->pushButton_mobileAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->pushButton_options, SIGNAL(clicked(bool)), this, SLOT(selectOptionsFile()));
}


void MobileWidget::selectOptionsFile()
{
    QString filename = QFileDialog::getOpenFileName(this,
                QApplication::translate("MobileWidget", "Select options file"),
                QDir::currentPath(),
                QApplication::translate("MobileWidget", "Configuration files (*.conf)"));
    if (!filename.isEmpty())
        ui->lineEdit_options->setText(filename);
}


void MobileWidget::showAdvanced()
{
    if (ui->pushButton_mobileAdvanced->isChecked()) {
        ui->widget_mobileAdvanced->setHidden(false);
        ui->pushButton_mobileAdvanced->setArrowType(Qt::UpArrow);
    } else {
        ui->widget_mobileAdvanced->setHidden(true);
        ui->pushButton_mobileAdvanced->setArrowType(Qt::DownArrow);
    }
}


QMap<QString, QString> MobileWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    if (!ui->lineEdit_username->text().isEmpty())
        settings[QString("User")] = QString("'%1'").arg(ui->lineEdit_username->text());
    if (!ui->lineEdit_password->text().isEmpty())
        settings[QString("Password")] = QString("'%1'").arg(ui->lineEdit_password->text());
    settings[QString("AccessPointName")] = ui->lineEdit_apn->text();
    if (!ui->lineEdit_pin->text().isEmpty())
        settings[QString("PIN")] = QString("'%1'").arg(ui->lineEdit_pin->text());
    else
        settings[QString("PIN")] = QString("None");
    settings[QString("Mode")] = ui->comboBox_mode->currentText();
    if (ui->spinBox_fail->value() != 5)
        settings[QString("MaxFail")] = QString::number(ui->spinBox_fail->value());
    if (ui->checkBox_route->checkState() == Qt::Unchecked)
        settings[QString("DefaultRoute")] = QString("false");
    if (ui->checkBox_dns->checkState() == Qt::Unchecked)
        settings[QString("UsePeerDNS")] = QString("false");
    if (!ui->lineEdit_options->text().isEmpty())
        settings[QString("OptionsFile")] = QString("'%1'").arg(ui->lineEdit_options->text());

    return settings;
}


int MobileWidget::isOk()
{
    // APN is not set
    if (ui->lineEdit_apn->text().isEmpty())
        return 1;
    // config file doesn't exist
    if (!ui->lineEdit_options->text().isEmpty())
        if (!QFile(ui->lineEdit_options->text()).exists())
            return 2;
    // all fine
    return 0;
}


void MobileWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("User")))
        ui->lineEdit_username->setText(settings[QString("User")]);
    if (settings.contains(QString("Password")))
        ui->lineEdit_password->setText(settings[QString("Password")]);
    if (settings.contains(QString("AccessPointName")))
        ui->lineEdit_apn->setText(settings[QString("AccessPointName")]);
    if (settings.contains(QString("PIN")))
        ui->lineEdit_pin->setText(settings[QString("PIN")]);
    if (settings.contains(QString("Mode"))) {
        int index = ui->comboBox_mode->findText(settings[QString("Mode")]);
        ui->comboBox_mode->setCurrentIndex(index);
    }
    if (settings.contains(QString("MaxFail")))
        ui->spinBox_fail->setValue(settings[QString("MaxFail")].toInt());
    if (settings.contains(QString("DefaultRoute")))
        if (settings[QString("DefaultRoute")] == QString("false"))
            ui->checkBox_route->setCheckState(Qt::Unchecked);
    if (settings.contains(QString("UsePeerDNS")))
        if (settings[QString("UsePeerDNS")] == QString("false"))
            ui->checkBox_dns->setCheckState(Qt::Unchecked);
    if (settings.contains(QString("OptionsFile")))
        ui->lineEdit_options->setText(settings[QString("OptionsFile")]);
}
