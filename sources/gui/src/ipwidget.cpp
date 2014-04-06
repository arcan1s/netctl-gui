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

#include "ipwidget.h"
#include "ui_ipwidget.h"

#include <QDebug>
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
    ipEnable(ui->checkBox_ip->checkState());
    changeIpMode(ui->comboBox_ip->currentText());
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
    ip6Enable(ui->checkBox_ip6->checkState());
    changeIp6Mode(ui->comboBox_ip6->currentText());
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
    changeDhcpClient(ui->comboBox_dhcp->currentText());
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


void IpWidget::setShown(const bool state)
{
    if (state)
        show();
    else
        hide();
}


void IpWidget::createActions()
{
    connect(ui->pushButton_ipAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    // ip mode
    connect(ui->checkBox_ip, SIGNAL(stateChanged(int)), this, SLOT(ipEnable(int)));
    connect(ui->comboBox_ip, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeIpMode(QString)));
    // ipv6 mode
    connect(ui->checkBox_ip6, SIGNAL(stateChanged(int)), this, SLOT(ip6Enable(int)));
    connect(ui->comboBox_ip6, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeIp6Mode(QString)));
    // dhcp client
    connect(ui->comboBox_dhcp, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeDhcpClient(QString)));
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


QString IpWidget::getIp(const QString rawIp)
{
    QStringList ip = rawIp.split(QString("."));

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

    return ip.join(QString("."));
}


QString IpWidget::getPrefix(const QString rawPrefix)
{
    QString prefix;

    if (rawPrefix.isEmpty())
        prefix = QString("24");
    else if (rawPrefix.toInt() > 32)
        prefix = QString("32");
    else
        prefix = rawPrefix;

    return prefix;
}


QString IpWidget::getIp6(const QString rawIp)
{
    QString ip = rawIp;

    for (int i=0; i<5; i++)
        ip.replace(QString(":::"), QString("::"));

    return ip;
}


QString IpWidget::getPrefix6(const QString rawPrefix)
{
    QString prefix;

    if (rawPrefix.isEmpty())
        prefix = QString("64");
    else if (rawPrefix.toInt() > 128)
        prefix = QString("128");
    else
        prefix = rawPrefix;

    return prefix;
}


void IpWidget::addIp()
{
    QString ip = getIp(ui->lineEdit_ipAddress->text().remove(" ").split(QString("/"))[0]);
    QString prefix = getPrefix(ui->lineEdit_ipAddress->text().remove(" ").split(QString("/"))[1]);

    ui->listWidget_ipAddress->addItem(ip + QString("/") + prefix);
    ui->lineEdit_ipAddress->clear();
}


void IpWidget::addIpRoutes()
{
    QString ip = getIp(ui->lineEdit_ipRoutes->text().remove(" ").split(QString("/"))[0]);
    QString prefix = getPrefix(ui->lineEdit_ipRoutes->text().remove(" ").split(QString("/"))[1]);
    QString ipVia = getIp(ui->lineEdit_ipRoutes2->text().remove(" "));

    ui->listWidget_ipRoutes->addItem(ip + QString("/") + prefix + QString(" via ") + ipVia);
    ui->lineEdit_ipRoutes->clear();
    ui->lineEdit_ipRoutes2->clear();
}


void IpWidget::addIp6()
{
    QString ip = getIp6(ui->lineEdit_ipAddress6->text().remove(" ").split(QString("/"))[0]);
    QString prefix = getPrefix6(ui->lineEdit_ipAddress6->text().remove(" ").split(QString("/"))[1]);

    ui->listWidget_ipAddress6->addItem(ip + QString("/") + prefix);
    ui->lineEdit_ipAddress6->clear();
}


void IpWidget::addIpRoutes6()
{
    QString ip = getIp6(ui->lineEdit_ipRoutes6->text().remove(" ").split(QString("/"))[0]);
    QString prefix = getPrefix6(ui->lineEdit_ipRoutes6->text().remove(" ").split(QString("/"))[1]);
    QString ipVia = getIp6(ui->lineEdit_ipRoutes62->text().remove(" "));

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
    QString ip = getIp(ui->lineEdit_dns->text().remove(" "));

    ui->listWidget_dns->addItem(ip);
    ui->lineEdit_dns->clear();
}


void IpWidget::addDnsOpt()
{
    if (!ui->lineEdit_dnsOptions->text().isEmpty())
        ui->listWidget_dnsOptions->addItem(ui->lineEdit_dnsOptions->text());
    ui->lineEdit_dnsOptions->clear();
}


void IpWidget::changeIpMode(const QString currentText)
{
    if (currentText == QString("dhcp"))
        ui->widget_ip->setHidden(true);
    else if (currentText == QString("static"))
        ui->widget_ip->setShown(true);
}


void IpWidget::ipEnable(const int state)
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


void IpWidget::changeIp6Mode(const QString currentText)
{
    if ((currentText == QString("dhcp")) ||
            (currentText == QString("dhcp-noaddr")))
        ui->widget_ip6->setHidden(true);
    else if ((currentText == QString("static")) ||
             (currentText == QString("stateless")))
        ui->widget_ip6->setShown(true);
}


void IpWidget::ip6Enable(const int state)
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


void IpWidget::changeDhcpClient(const QString currentText)
{
    if (currentText == QString("dhcpcd")) {
        ui->widget_dhcpcdOpt->setShown(true);
        ui->widget_dhclientOpt->setHidden(true);
    }
    else if (currentText == QString("dhclient")) {
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


QMap<QString, QString> IpWidget::getSettings()
{
    QMap<QString, QString> ipSettings;

    if (isOk() != 0)
        return ipSettings;

    if (ui->checkBox_ip->checkState() == Qt::Checked) {
        ipSettings[QString("IP")] = ui->comboBox_ip->currentText();
        if (ui->comboBox_ip->currentText() == QString("static")) {
            QStringList addresses;
            for (int i=0; i<ui->listWidget_ipAddress->count(); i++)
                addresses.append(QString("'") + ui->listWidget_ipAddress->item(i)->text() + QString("'"));
            ipSettings[QString("Address")] = addresses.join(QString(" "));
            ipSettings[QString("Gateway")] = QString("'") + ui->lineEdit_gateway->text() + QString("'");
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
        if ((ui->comboBox_ip6->currentText() == QString("static")) ||
                (ui->comboBox_ip6->currentText() == QString("stateless"))) {
            QStringList addresses;
            for (int i=0; i<ui->listWidget_ipAddress6->count(); i++)
                addresses.append(QString("'") + ui->listWidget_ipAddress6->item(i)->text() + QString("'"));
            ipSettings[QString("Address6")] = addresses.join(QString(" "));
            ipSettings[QString("Gateway6")] = QString("'") + ui->lineEdit_gateway6->text() + QString("'");
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
        ipSettings[QString("TimeoutDAD")] = QString::number(ui->spinBox_timeoutDad->value());
    if (ui->comboBox_dhcp->currentText() == QString("dhcpcd")) {
        if (!ui->lineEdit_dhcpcdOpt->text().isEmpty())
            ipSettings[QString("DhcpcdOptions")] = QString("'") + ui->lineEdit_dhcpcdOpt->text() + QString("'");
    }
    else if (ui->comboBox_dhcp->currentText() == QString("dhclient")) {
        ipSettings[QString("DHCPClient")] = ui->comboBox_dhcp->currentText();
        if (!ui->lineEdit_dhclientOpt->text().isEmpty())
            ipSettings[QString("DhclientOptions")] = QString("'") + ui->lineEdit_dhclientOpt->text() + QString("'");
        if (!ui->lineEdit_dhclientOpt6->text().isEmpty())
            ipSettings[QString("DhclientOptions6")] = QString("'") + ui->lineEdit_dhclientOpt6->text() + QString("'");
    }
    if (ui->spinBox_timeoutDhcp->value() != 30)
        ipSettings[QString("TimeoutDHCP")] = QString::number(ui->spinBox_timeoutDhcp->value());
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
        ipSettings[QString("DNSOptions")] = dnsOpt.join(QString(" "));
    }

    return ipSettings;
}


int IpWidget::isOk()
{
    // ip settings is not set
    if (ui->checkBox_ip->checkState() == Qt::Checked)
        if (ui->comboBox_ip->currentText() == QString("static"))
            if ((ui->listWidget_ipAddress->count() == 0) ||
                    (ui->lineEdit_gateway->text().isEmpty()))
                return 1;
    // ipv6 settings is not set
    if (ui->checkBox_ip6->checkState() == Qt::Checked)
        if ((ui->comboBox_ip6->currentText() == QString("static")) ||
                (ui->comboBox_ip6->currentText() == QString("stateless")))
            if ((ui->listWidget_ipAddress6->count() == 0) ||
                    (ui->lineEdit_gateway6->text().isEmpty()))
                return 2;
    // all fine
    return 0;
}


void IpWidget::setSettings(const QMap<QString, QString> settings)
{
    QMap<QString, QString> ipSettings = settings;

    if (ipSettings.contains(QString("IP"))) {
        if (ipSettings[QString("IP")].remove(QString("'")) == QString("no"))
            ui->checkBox_ip->setCheckState(Qt::Unchecked);
        else {
            ui->checkBox_ip->setCheckState(Qt::Checked);
            for (int i=0; i<ui->comboBox_ip->count(); i++)
                if (ipSettings[QString("IP")].remove(QString("'")) == ui->comboBox_ip->itemText(i))
                    ui->comboBox_ip->setCurrentIndex(i);
        }
    }
    if (ipSettings.contains(QString("Address")))
        ui->listWidget_ipAddress->addItems(ipSettings[QString("Address")].remove(QString("'")).split(QString(" ")));
    if (ipSettings.contains(QString("Gateway")))
        ui->lineEdit_gateway->setText(ipSettings[QString("Gateway")].remove(QString("'")));
    if (ipSettings.contains(QString("Routes")))
        ui->listWidget_ipRoutes->addItems(ipSettings[QString("Routes")].remove(QString("'")).split(QString(" ")));
    if (ipSettings.contains(QString("IP6"))) {
        if (ipSettings[QString("IP6")].remove(QString("'")) == QString("no"))
            ui->checkBox_ip6->setCheckState(Qt::Unchecked);
        else {
            ui->checkBox_ip6->setCheckState(Qt::Checked);
            for (int i=0; i<ui->comboBox_ip6->count(); i++)
                if (ipSettings[QString("IP6")].remove(QString("'")) == ui->comboBox_ip6->itemText(i))
                    ui->comboBox_ip6->setCurrentIndex(i);
        }
    }
    if (ipSettings.contains(QString("Address6")))
        ui->listWidget_ipAddress6->addItems(ipSettings[QString("Address6")].remove(QString("'")).split(QString(" ")));
    if (ipSettings.contains(QString("Gateway6")))
        ui->lineEdit_gateway6->setText(ipSettings[QString("Gateway6")].remove(QString("'")));
    if (ipSettings.contains(QString("Routes6")))
        ui->listWidget_ipRoutes6->addItems(ipSettings[QString("Routes6")].remove(QString("'")).split(QString(" ")));
    if (ipSettings.contains(QString("IPCustom"))) {
        QStringList custom;
        if (ipSettings[QString("IPCustom")].contains(QString("\n")))
            custom = ipSettings[QString("IPCustom")].split(QString("'\n'"));
        else
            custom = ipSettings[QString("IPCustom")].split(QString("' '"));
        for (int i=0; i<custom.count(); i++)
            ui->listWidget_custom->addItem(custom[i].remove(QString("'")));
    }
    if (ipSettings.contains(QString("Hostname")))
        ui->lineEdit_hostname->setText(ipSettings[QString("Hostname")].remove(QString("'")));
    if (ipSettings.contains(QString("TimeoutDAD")))
        ui->spinBox_timeoutDad->setValue(ipSettings[QString("TimeoutDAD")].toInt());
    if (ipSettings.contains(QString("DHCPClient")))
        for (int i=0; i<ui->comboBox_dhcp->count(); i++)
            if (ipSettings[QString("DHCPClient")].remove(QString("'")) == ui->comboBox_dhcp->itemText(i))
                ui->comboBox_dhcp->setCurrentIndex(i);
    if (ipSettings.contains(QString("DhcpcdOptions")))
        ui->lineEdit_dhcpcdOpt->setText(ipSettings[QString("DhcpcdOptions")].remove(QString("'")));
    if (ipSettings.contains(QString("DhclientOptions")))
        ui->lineEdit_dhclientOpt->setText(ipSettings[QString("DhclientOptions")].remove(QString("'")));
    if (ipSettings.contains(QString("DhclientOptions6")))
        ui->lineEdit_dhclientOpt6->setText(ipSettings[QString("DhclientOptions6")].remove(QString("'")));
    if (ipSettings.contains(QString("TimeoutDHCP")))
        ui->spinBox_timeoutDhcp->setValue(ipSettings[QString("TimeoutDHCP")].toInt());
    if (ipSettings.contains(QString("DHCPReleaseOnStop")))
        if (ipSettings[QString("DHCPReleaseOnStop")].remove(QString("'")) == QString("yes"))
            ui->checkBox_dhcp->setCheckState(Qt::Checked);
    if (ipSettings.contains(QString("DNS")))
        ui->listWidget_dns->addItems(ipSettings[QString("DNS")].remove(QString("'")).split(QString(" ")));
    if (ipSettings.contains(QString("DNSDomain")))
        ui->lineEdit_dnsDomain->setText(ipSettings[QString("DNSDomain")].remove(QString("'")));
    if (ipSettings.contains(QString("DNSSearch")))
        ui->lineEdit_dnsSearch->setText(ipSettings[QString("DNSSearch")].remove(QString("'")));
    if (ipSettings.contains(QString("DNSOptions")))
        ui->listWidget_dnsOptions->addItems(ipSettings[QString("DNSOptions")].remove(QString("'")).split(QString(" ")));

    ipEnable(ui->checkBox_ip->checkState());
    changeIpMode(ui->comboBox_ip->currentText());
    ip6Enable(ui->checkBox_ip6->checkState());
    changeIp6Mode(ui->comboBox_ip6->currentText());
    changeDhcpClient(ui->comboBox_dhcp->currentText());
}
