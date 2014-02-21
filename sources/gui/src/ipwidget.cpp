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

#include "ipwidget.h"
#include "ui_ipwidget.h"

#include <QKeyEvent>


IpWidget::IpWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::IpWidget)
{
    ui->setupUi(this);
    createActions();
    createFilter();
    clear();
}


IpWidget::~IpWidget()
{
    delete ui;
}


void IpWidget::clear()
{
    ui->checkBox_ip->setCheckState(Qt::Checked);
    ui->comboBox_ip->setCurrentIndex(0);
    changeIpMode(ui->comboBox_ip->currentIndex());
    ui->lineEdit_ipAddress->clear();
    ui->listWidget_ipAddress->setCurrentRow(-1);
    ui->listWidget_ipAddress->clear();
    ui->lineEdit_gateway->clear();
    ui->lineEdit_ipRoutes->clear();
    ui->lineEdit_ipRoutes2->clear();
    ui->listWidget_ipRoutes->setCurrentRow(-1);
    ui->listWidget_ipRoutes->clear();
    ui->checkBox_ip6->setCheckState(Qt::Unchecked);
    ui->comboBox_ip6->setCurrentIndex(0);
    changeIp6Mode(ui->comboBox_ip6->currentIndex());
    ui->lineEdit_ipAddress6->clear();
    ui->listWidget_ipAddress6->setCurrentRow(-1);
    ui->listWidget_ipAddress6->clear();
    ui->lineEdit_gateway6->clear();
    ui->lineEdit_ipRoutes6->clear();
    ui->lineEdit_ipRoutes62->clear();
    ui->listWidget_ipRoutes6->setCurrentRow(-1);
    ui->listWidget_ipRoutes6->clear();
    ui->lineEdit_custom->clear();
    ui->listWidget_custom->clear();
    ui->lineEdit_hostname->clear();
    ui->spinBox_timeoutDad->setValue(3);
    ui->comboBox_dhcp->setCurrentIndex(0);
    changeDhcpClient(ui->comboBox_dhcp->currentIndex());
    ui->lineEdit_dhcpcdOpt->clear();
    ui->lineEdit_dhclientOpt->clear();
    ui->lineEdit_dhclientOpt6->clear();
    ui->spinBox_timeoutDhcp->setValue(30);
    ui->checkBox_dhcp->setCheckState(Qt::Unchecked);
    ui->lineEdit_dns->clear();
    ui->listWidget_dns->setCurrentRow(-1);
    ui->listWidget_dns->clear();
    ui->lineEdit_dnsDomain->clear();
    ui->lineEdit_dnsSearch->clear();
    ui->lineEdit_dnsOptions->clear();
    ui->listWidget_dnsOptions->setCurrentRow(-1);
    ui->listWidget_dnsOptions->clear();

    ui->pushButton_ipAdvanced->setChecked(false);;
    showAdvanced();
}


void IpWidget::createActions()
{
    connect(ui->pushButton_ipAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    // ip mode
    connect(ui->checkBox_ip, SIGNAL(stateChanged(int)), this, SLOT(ipEnable(int)));
    connect(ui->comboBox_ip, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIpMode(int)));
    // ipv6 mode
    connect(ui->checkBox_ip6, SIGNAL(stateChanged(int)), this, SLOT(ip6Enable(int)));
    connect(ui->comboBox_ip6, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIp6Mode(int)));
    // dhcp client
    connect(ui->comboBox_dhcp, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDhcpClient(int)));
    // buttons
    connect(ui->pushButton_ipAddress, SIGNAL(clicked(bool)), this, SLOT(addIp()));
    connect(ui->pushButton_ipRoutes, SIGNAL(clicked(bool)), this, SLOT(addIpRoutes()));
    connect(ui->pushButton_ipAddress6, SIGNAL(clicked(bool)), this, SLOT(addIp6()));
    connect(ui->pushButton_ipRoutes6, SIGNAL(clicked(bool)), this, SLOT(addIpRoutes6()));
    connect(ui->pushButton_custom, SIGNAL(clicked(bool)), this, SLOT(addCustom()));
    connect(ui->pushButton_dns, SIGNAL(clicked(bool)), this, SLOT(addDns()));
    connect(ui->pushButton_dnsOptions, SIGNAL(clicked(bool)), this, SLOT(addDnsOpt()));
}


void IpWidget::createFilter()
{
    // using input mask because validators is not comfortable
    // ipv4
    ui->lineEdit_ipAddress->setInputMask(QString("999.999.999.999/99"));
    ui->lineEdit_gateway->setInputMask(QString("999.999.999.999"));
    ui->lineEdit_ipRoutes->setInputMask(QString("999.999.999.999/99"));
    ui->lineEdit_ipRoutes2->setInputMask(QString("999.999.999.999"));

    // ipv6
    ui->lineEdit_ipAddress6->setInputMask(QString("<hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh/999"));
    ui->lineEdit_gateway6->setInputMask(QString("<hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh"));
    ui->lineEdit_ipRoutes6->setInputMask(QString("<hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh/999"));
    ui->lineEdit_ipRoutes62->setInputMask(QString("<hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh"));

    // dns
    ui->lineEdit_dns->setInputMask(QString("999.999.999.999"));
}


void IpWidget::keyPressEvent(QKeyEvent *pressedKey)
{
    if (pressedKey->key() == Qt::Key_Delete) {
        if (ui->listWidget_ipAddress->hasFocus() &&
                (ui->listWidget_ipAddress->currentItem() != 0))
            delete ui->listWidget_ipAddress->currentItem();
        else if (ui->listWidget_ipRoutes->hasFocus() &&
                 (ui->listWidget_ipRoutes->currentItem() != 0))
            delete ui->listWidget_ipRoutes->currentItem();
        else if (ui->listWidget_ipAddress6->hasFocus() &&
                 (ui->listWidget_ipAddress6->currentItem() != 0))
            delete ui->listWidget_ipAddress6->currentItem();
        else if (ui->listWidget_ipRoutes6->hasFocus() &&
                 (ui->listWidget_ipRoutes6->currentItem() != 0))
            delete ui->listWidget_ipRoutes6->currentItem();
        else if (ui->listWidget_custom->hasFocus() &&
                 (ui->listWidget_custom->currentItem() != 0))
            delete ui->listWidget_custom->currentItem();
        else if (ui->listWidget_dns->hasFocus() &&
                 (ui->listWidget_dns->currentItem() != 0))
            delete ui->listWidget_dns->currentItem();
        else if (ui->listWidget_dnsOptions->hasFocus() &&
                 (ui->listWidget_dnsOptions->currentItem() != 0))
            delete ui->listWidget_dnsOptions->currentItem();
    }
}


void IpWidget::addIp()
{
    QStringList ip = ui->lineEdit_ipAddress->text().remove(" ").split(QString("/"))[0].split(QString("."));
    QString prefix = ui->lineEdit_ipAddress->text().remove(" ").split(QString("/"))[1];
    // fix empty fields
    if (ip[0].isEmpty())
        ip[0] = QString("127");
    if (ip[1].isEmpty())
        ip[1] = QString("0");
    if (ip[2].isEmpty())
        ip[2] = QString("0");
    if (ip[3].isEmpty())
        ip[3] = QString("1");
    // fix numbers
    for (int i=0; i<4; i++)
        if (ip[i].toInt() > 255)
            ip[i] = QString("255");
    if (prefix.isEmpty())
        prefix = QString("24");
    if (prefix.toInt() > 32)
        prefix = QString("32");
    ui->listWidget_ipAddress->addItem(ip.join(QString(".")) + QString("/") + prefix);
    ui->lineEdit_ipAddress->clear();
}


void IpWidget::addIpRoutes()
{
    QStringList ip = ui->lineEdit_ipRoutes->text().remove(" ").split(QString("/"))[0].split(QString("."));
    QString prefix = ui->lineEdit_ipRoutes->text().remove(" ").split(QString("/"))[1];
    QStringList ipVia = ui->lineEdit_ipRoutes2->text().remove(" ").split(QString("."));
    // fix empty fields
    if (ip[0].isEmpty())
        ip[0] = QString("127");
    if (ip[1].isEmpty())
        ip[1] = QString("0");
    if (ip[2].isEmpty())
        ip[2] = QString("0");
    if (ip[3].isEmpty())
        ip[3] = QString("1");
    if (ipVia[0].isEmpty())
        ipVia[0] = QString("127");
    if (ipVia[1].isEmpty())
        ipVia[1] = QString("0");
    if (ipVia[2].isEmpty())
        ipVia[2] = QString("0");
    if (ipVia[3].isEmpty())
        ipVia[3] = QString("1");
    // fix numbers
    for (int i=0; i<4; i++)
        if (ip[i].toInt() > 255)
            ip[i] = QString("255");
    if (prefix.isEmpty())
        prefix = QString("24");
    if (prefix.toInt() > 32)
        prefix = QString("32");
    for (int i=0; i<4; i++)
        if (ipVia[i].toInt() > 255)
            ipVia[i] = QString("255");
    ui->listWidget_ipRoutes->addItem(ip.join(QString(".")) + QString("/") + prefix + QString(" via ") + ipVia.join(QString(".")));
    ui->lineEdit_ipRoutes->clear();
    ui->lineEdit_ipRoutes2->clear();
}


void IpWidget::addIp6()
{
    QString ip = ui->lineEdit_ipAddress6->text().remove(" ").split(QString("/"))[0];
    QString prefix = ui->lineEdit_ipAddress6->text().remove(" ").split(QString("/"))[1];
    // fix empty fields
    for (int i=0; i<5; i++)
        ip.replace(QString(":::"), QString("::"));
    // fix numbers
    if (prefix.isEmpty())
        prefix = QString("64");
    if (prefix.toInt() > 128)
        prefix = QString("128");
    ui->listWidget_ipAddress6->addItem(ip + QString("/") + prefix);
    ui->lineEdit_ipAddress6->clear();
}


void IpWidget::addIpRoutes6()
{
    QString ip = ui->lineEdit_ipRoutes6->text().remove(" ").split(QString("/"))[0];
    QString prefix = ui->lineEdit_ipRoutes6->text().remove(" ").split(QString("/"))[1];
    QString ipVia = ui->lineEdit_ipRoutes62->text().remove(" ");
    // fix empty fields
    for (int i=0; i<5; i++)
        ip.replace(QString(":::"), QString("::"));
    for (int i=0; i<5; i++)
        ipVia.replace(QString(":::"), QString("::"));
    // fix numbers
    if (prefix.isEmpty())
        prefix = QString("64");
    if (prefix.toInt() > 128)
        prefix = QString("128");
    ui->listWidget_ipRoutes6->addItem(ip + QString("/") + prefix + QString(" via ") + ipVia);
    ui->lineEdit_ipRoutes6->clear();
    ui->lineEdit_ipRoutes62->clear();
}


void IpWidget::addCustom()
{
    if (!ui->lineEdit_custom->text().isEmpty())
        ui->listWidget_custom->addItem(ui->lineEdit_custom->text());
    ui->lineEdit_custom->clear();
}


void IpWidget::addDns()
{
    QStringList ip = ui->lineEdit_dns->text().remove(" ").split(QString("."));
    // fix empty fields
    if (ip[0].isEmpty())
        ip[0] = QString("127");
    if (ip[1].isEmpty())
        ip[1] = QString("0");
    if (ip[2].isEmpty())
        ip[2] = QString("0");
    if (ip[3].isEmpty())
        ip[3] = QString("1");
    // fix numbers
    for (int i=0; i<4; i++)
        if (ip[i].toInt() > 255)
            ip[i] = QString("255");
    ui->listWidget_dns->addItem(ip.join(QString(".")));
    ui->lineEdit_dns->clear();
}


void IpWidget::addDnsOpt()
{
    if (!ui->lineEdit_dnsOptions->text().isEmpty())
        ui->listWidget_dnsOptions->addItem(ui->lineEdit_dnsOptions->text());
    ui->lineEdit_dnsOptions->clear();
}


void IpWidget::changeIpMode(int index)
{
    if (index == 0)
        // dhcp
        ui->widget_ip->setHidden(true);
    else if (index == 1)
        // static
        ui->widget_ip->setShown(true);
}


void IpWidget::ipEnable(int state)
{
    if (state == 0) {
        ui->comboBox_ip->setDisabled(true);
        ui->widget_ip->setDisabled(true);
        ui->widget_ipRoutes->setDisabled(true);
    }
    else {
        ui->comboBox_ip->setEnabled(true);
        ui->widget_ip->setEnabled(true);
        ui->widget_ipRoutes->setEnabled(true);
    }
}


void IpWidget::changeIp6Mode(int index)
{
    if ((index == 0) || (index == 1))
        // dhcp
        ui->widget_ip6->setHidden(true);
    else if ((index == 2) || (index == 3))
        // static
        ui->widget_ip6->setShown(true);
}


void IpWidget::ip6Enable(int state)
{
    if (state == 0) {
        ui->comboBox_ip6->setDisabled(true);
        ui->widget_ip6->setDisabled(true);
        ui->widget_ipRoutes6->setDisabled(true);
    }
    else {
        ui->comboBox_ip6->setEnabled(true);
        ui->widget_ip6->setEnabled(true);
        ui->widget_ipRoutes6->setEnabled(true);
    }
}


void IpWidget::changeDhcpClient(int index)
{
    if (index == 0) {
        // dhcp
        ui->widget_dhcpcdOpt->setShown(true);
        ui->widget_dhclientOpt->setHidden(true);
    }
    else if (index == 1) {
        // dhclient
        ui->widget_dhcpcdOpt->setHidden(true);
        ui->widget_dhclientOpt->setShown(true);
    }
}


void IpWidget::showAdvanced()
{
    if (ui->pushButton_ipAdvanced->isChecked()) {
        ui->widget_ipAdvanced->setShown(true);
        ui->pushButton_ipAdvanced->setText(QApplication::translate("IpWidget", "Hide advanced"));
    }
    else {
        ui->widget_ipAdvanced->setHidden(true);
        ui->pushButton_ipAdvanced->setText(QApplication::translate("IpWidget", "Show advanced"));
    }
}


QHash<QString, QString> IpWidget::getSettings()
{
    QHash<QString, QString> ipSettings;

    if (isOk() == 0) {
        if (ui->checkBox_ip->checkState() == Qt::Checked) {
            ipSettings[QString("IP")] = ui->comboBox_ip->currentText();
            if (ui->comboBox_ip->currentIndex() == 1) {
                QStringList addresses;
                for (int i=0; i<ui->listWidget_ipAddress->count(); i++)
                    addresses.append(QString("'") + ui->listWidget_ipAddress->item(i)->text() + QString("'"));
                ipSettings[QString("Address")] = addresses.join(QString(" "));
                ipSettings[QString("Gateway")] = ui->lineEdit_gateway->text();
            }
            if (ui->listWidget_ipRoutes->count() > 0) {
                QStringList routes;
                for (int i=0; i<ui->listWidget_ipRoutes->count(); i++)
                    routes.append(QString("'") + ui->listWidget_ipRoutes->item(i)->text() + QString("'"));
                ipSettings[QString("Routes")] = routes.join(QString(" "));
            }
        }
        else
            ipSettings[QString("IP")] = QString("no");
        if (ui->checkBox_ip6->checkState() == Qt::Checked) {
            ipSettings[QString("IP6")] = ui->comboBox_ip6->currentText();
            if ((ui->comboBox_ip6->currentIndex() == 2) ||
                    (ui->comboBox_ip6->currentIndex() == 3)) {
                QStringList addresses;
                for (int i=0; i<ui->listWidget_ipAddress6->count(); i++)
                    addresses.append(QString("'") + ui->listWidget_ipAddress6->item(i)->text() + QString("'"));
                ipSettings[QString("Address6")] = addresses.join(QString(" "));
                ipSettings[QString("Gateway6")] = ui->lineEdit_gateway6->text();
            }
            if (ui->listWidget_ipRoutes6->count() > 0) {
                QStringList routes;
                for (int i=0; i<ui->listWidget_ipRoutes6->count(); i++)
                    routes.append(QString("'") + ui->listWidget_ipRoutes6->item(i)->text() + QString("'"));
                ipSettings[QString("Routes6")] = routes.join(QString(" "));
            }
        }
        else
            ipSettings[QString("IP6")] = QString("no");
        if (ui->listWidget_custom->count() > 0) {
            QStringList custom;
            for (int i=0; i<ui->listWidget_custom->count(); i++)
                custom.append(QString("'") + ui->listWidget_custom->item(i)->text() + QString("'"));
            ipSettings[QString("IPCustom")] = custom.join(QString(" "));
        }
        if (!ui->lineEdit_hostname->text().isEmpty())
            ipSettings[QString("Hostname")] = QString("'") + ui->lineEdit_hostname->text() + QString("'");
        if (ui->spinBox_timeoutDad->value() != 3)
            ipSettings[QString("TimeoutDAD")] = QString(ui->spinBox_timeoutDad->value());
        if (ui->comboBox_dhcp->currentIndex() == 0) {
            if (!ui->lineEdit_dhcpcdOpt->text().isEmpty())
                ipSettings[QString("DhcpcdOptions")] = QString("'") + ui->lineEdit_dhcpcdOpt->text() + QString("'");
        }
        else if (ui->comboBox_dhcp->currentIndex() == 1) {
            ipSettings[QString("DHCPClient")] = QString(ui->spinBox_timeoutDad->value());
            if (!ui->lineEdit_dhclientOpt->text().isEmpty())
                ipSettings[QString("DhclientOptions")] = QString("'") + ui->lineEdit_dhclientOpt->text() + QString("'");
            if (!ui->lineEdit_dhclientOpt6->text().isEmpty())
                ipSettings[QString("DhclientOptions")] = QString("'") + ui->lineEdit_dhclientOpt6->text() + QString("'");
        }
        if (ui->spinBox_timeoutDhcp->value() != 30)
            ipSettings[QString("TimeoutDHCP")] = QString(ui->spinBox_timeoutDhcp->value());
        if (ui->checkBox_dhcp->checkState() == Qt::Checked)
            ipSettings[QString("DHCPReleaseOnStop")] = QString("yes");
        if (ui->listWidget_dns->count() > 0) {
            QStringList dns;
            for (int i=0; i<ui->listWidget_dns->count(); i++)
                dns.append(QString("'") + ui->listWidget_dns->item(i)->text() + QString("'"));
            ipSettings[QString("DNS")] = dns.join(QString(" "));
        }
        if (!ui->lineEdit_dnsDomain->text().isEmpty())
            ipSettings[QString("DNSDomain")] = QString("'") + ui->lineEdit_dnsDomain->text() + QString("'");
        if (!ui->lineEdit_dnsSearch->text().isEmpty())
            ipSettings[QString("DNSSearch")] = QString("'") + ui->lineEdit_dnsSearch->text() + QString("'");
        if (ui->listWidget_dnsOptions->count() > 0) {
            QStringList dnsOpt;
            for (int i=0; i<ui->listWidget_dnsOptions->count(); i++)
                dnsOpt.append(QString("'") + ui->listWidget_dnsOptions->item(i)->text() + QString("'"));
            ipSettings[QString("DNS")] = dnsOpt.join(QString("\n"));
        }
        clear();
    }

    return ipSettings;
}


int IpWidget::isOk()
{
    // ip settings is not set
    if (ui->checkBox_ip->checkState() == Qt::Checked)
        if (ui->comboBox_ip->currentIndex() == 1)
            if ((ui->listWidget_ipAddress->count() == 0) ||
                    (ui->lineEdit_gateway->text().isEmpty()))
                return 1;
    // ipv6 settings is not set
    if (ui->checkBox_ip6->checkState() == Qt::Checked)
        if ((ui->comboBox_ip6->currentIndex() == 2) || (ui->comboBox_ip6->currentIndex() == 3))
            if ((ui->listWidget_ipAddress6->count() == 0) ||
                    (ui->lineEdit_gateway6->text().isEmpty()))
                return 2;
    // all fine
    return 0;
}


void IpWidget::setSettings(QHash<QString, QString> settings)
{

}
