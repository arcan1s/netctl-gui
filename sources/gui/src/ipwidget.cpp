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

#include <QKeyEvent>

#include "errorwindow.h"
#include "ipregexp.h"


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
    connect(ui->lineEdit_ipAddress, SIGNAL(returnPressed()), this, SLOT(addIp()));
    connect(ui->pushButton_ipRoutes, SIGNAL(clicked(bool)), this, SLOT(addIpRoutes()));
    connect(ui->pushButton_ipAddress6, SIGNAL(clicked(bool)), this, SLOT(addIp6()));
    connect(ui->lineEdit_ipAddress6, SIGNAL(returnPressed()), this, SLOT(addIp6()));
    connect(ui->pushButton_ipRoutes6, SIGNAL(clicked(bool)), this, SLOT(addIpRoutes6()));
    connect(ui->pushButton_custom, SIGNAL(clicked(bool)), this, SLOT(addCustom()));
    connect(ui->lineEdit_custom, SIGNAL(returnPressed()), this, SLOT(addCustom()));
    connect(ui->pushButton_dns, SIGNAL(clicked(bool)), this, SLOT(addDns()));
    connect(ui->lineEdit_dns, SIGNAL(returnPressed()), this, SLOT(addDns()));
    connect(ui->pushButton_dnsOptions, SIGNAL(clicked(bool)), this, SLOT(addDnsOpt()));
    connect(ui->lineEdit_dnsOptions, SIGNAL(returnPressed()), this, SLOT(addDnsOpt()));
}


void IpWidget::createFilter()
{
    // ipv4
    ui->lineEdit_ipAddress->setValidator(IpRegExp::ipv4CidrValidator());
    ui->lineEdit_gateway->setValidator(IpRegExp::ipv4Validator());
    ui->lineEdit_ipRoutes->setValidator(IpRegExp::ipv4CidrValidator());
    ui->lineEdit_ipRoutes2->setValidator(IpRegExp::ipv4Validator());

    // ipv6
    ui->lineEdit_ipAddress6->setValidator(IpRegExp::ipv6CidrValidator());
    ui->lineEdit_gateway6->setValidator(IpRegExp::ipv6Validator());
    ui->lineEdit_ipRoutes6->setValidator(IpRegExp::ipv6CidrValidator());
    ui->lineEdit_ipRoutes62->setValidator(IpRegExp::ipv6Validator());

    // dns
    ui->lineEdit_dns->setValidator(IpRegExp::ipv4Validator());
}


void IpWidget::keyPressEvent(QKeyEvent *pressedKey)
{
    if (pressedKey->key() == Qt::Key_Delete) {
        if (ui->listWidget_ipAddress->hasFocus() &&
            (ui->listWidget_ipAddress->currentItem() != nullptr))
            delete ui->listWidget_ipAddress->currentItem();
        else if (ui->listWidget_ipRoutes->hasFocus() &&
                 (ui->listWidget_ipRoutes->currentItem() != nullptr))
            delete ui->listWidget_ipRoutes->currentItem();
        else if (ui->listWidget_ipAddress6->hasFocus() &&
                 (ui->listWidget_ipAddress6->currentItem() != nullptr))
            delete ui->listWidget_ipAddress6->currentItem();
        else if (ui->listWidget_ipRoutes6->hasFocus() &&
                 (ui->listWidget_ipRoutes6->currentItem() != nullptr))
            delete ui->listWidget_ipRoutes6->currentItem();
        else if (ui->listWidget_custom->hasFocus() &&
                 (ui->listWidget_custom->currentItem() != nullptr))
            delete ui->listWidget_custom->currentItem();
        else if (ui->listWidget_dns->hasFocus() &&
                 (ui->listWidget_dns->currentItem() != nullptr))
            delete ui->listWidget_dns->currentItem();
        else if (ui->listWidget_dnsOptions->hasFocus() &&
                 (ui->listWidget_dnsOptions->currentItem() != nullptr))
            delete ui->listWidget_dnsOptions->currentItem();
    }
}


void IpWidget::addIp()
{
    if (!IpRegExp::checkString(ui->lineEdit_ipAddress->text(), IpRegExp::ip4CidrRegex()))
        return ErrorWindow::showWindow(20, QString("[IpWidget::addIp]"));

    ui->listWidget_ipAddress->addItem(ui->lineEdit_ipAddress->text());
    ui->lineEdit_ipAddress->clear();
}


void IpWidget::addIpRoutes()
{
    if ((!IpRegExp::checkString(ui->lineEdit_ipRoutes->text(), IpRegExp::ip4CidrRegex())) ||
        (!IpRegExp::checkString(ui->lineEdit_ipRoutes2->text(), IpRegExp::ip4Regex())))
        return ErrorWindow::showWindow(20, QString("[IpWidget::addIpRoutes]"));

    ui->listWidget_ipRoutes->addItem(ui->lineEdit_ipRoutes->text() + QString(" via ") + ui->lineEdit_ipRoutes2->text());
    ui->lineEdit_ipRoutes->clear();
    ui->lineEdit_ipRoutes2->clear();
}


void IpWidget::addIp6()
{
    if (!IpRegExp::checkString(ui->lineEdit_ipAddress6->text(), IpRegExp::ip6CidrRegex()))
        return ErrorWindow::showWindow(20, QString("[IpWidget::addIp6]"));

    ui->listWidget_ipAddress6->addItem(ui->lineEdit_ipAddress6->text());
    ui->lineEdit_ipAddress6->clear();
}


void IpWidget::addIpRoutes6()
{
    if ((!IpRegExp::checkString(ui->lineEdit_ipRoutes6->text(), IpRegExp::ip6CidrRegex())) ||
        (!IpRegExp::checkString(ui->lineEdit_ipRoutes62->text(), IpRegExp::ip6Regex())))
        return ErrorWindow::showWindow(20, QString("[IpWidget::addIp6Routes]"));

    ui->listWidget_ipRoutes6->addItem(ui->lineEdit_ipRoutes6->text() + QString(" via ") + ui->lineEdit_ipRoutes62->text());
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
    if (!IpRegExp::checkString(ui->lineEdit_dns->text(), IpRegExp::ip4Regex()))
        return ErrorWindow::showWindow(20, QString("[IpWidget::addDns]"));

    ui->listWidget_dns->addItem(ui->lineEdit_dns->text());
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
    ui->widget_ip->setHidden(currentText == QString("dhcp"));
}


void IpWidget::ipEnable(const int state)
{
    ui->comboBox_ip->setDisabled(state == 0);
    ui->widget_ip->setDisabled(state == 0);
    ui->widget_ipRoutes->setDisabled(state == 0);
}


void IpWidget::changeIp6Mode(const QString currentText)
{
    ui->widget_ip6->setHidden(currentText.startsWith(QString("dhcp")));
}


void IpWidget::ip6Enable(const int state)
{
    ui->comboBox_ip6->setDisabled(state == 0);
    ui->widget_ip6->setDisabled(state == 0);
    ui->widget_ipRoutes6->setDisabled(state == 0);
}


void IpWidget::changeDhcpClient(const QString currentText)
{
    ui->widget_dhcpcdOpt->setHidden(currentText != QString("dhcpcd"));
    ui->widget_dhclientOpt->setHidden(currentText == QString("dhcpcd"));
}


void IpWidget::showAdvanced()
{
    if (ui->pushButton_ipAdvanced->isChecked()) {
        ui->widget_ipAdvanced->setHidden(false);
        ui->pushButton_ipAdvanced->setArrowType(Qt::UpArrow);
    } else {
        ui->widget_ipAdvanced->setHidden(true);
        ui->pushButton_ipAdvanced->setArrowType(Qt::DownArrow);
    }
}


QMap<QString, QString> IpWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    if (ui->checkBox_ip->checkState() == Qt::Checked) {
        settings[QString("IP")] = ui->comboBox_ip->currentText();
        if (ui->comboBox_ip->currentText() == QString("static")) {
            QStringList addresses;
            for (int i=0; i<ui->listWidget_ipAddress->count(); i++)
                addresses.append(QString("'%1'").arg(ui->listWidget_ipAddress->item(i)->text()));
            settings[QString("Address")] = addresses.join(QChar(' '));
            settings[QString("Gateway")] = QString("'%1'").arg(ui->lineEdit_gateway->text());
        }
        if (ui->listWidget_ipRoutes->count() > 0) {
            QStringList routes;
            for (int i=0; i<ui->listWidget_ipRoutes->count(); i++)
                routes.append(QString("'%1'").arg(ui->listWidget_ipRoutes->item(i)->text()));
            settings[QString("Routes")] = routes.join(QChar(' '));
        }
    } else
        settings[QString("IP")] = QString("no");
    if (ui->checkBox_ip6->checkState() == Qt::Checked) {
        settings[QString("IP6")] = ui->comboBox_ip6->currentText();
        if ((ui->comboBox_ip6->currentText() == QString("static")) ||
            (ui->comboBox_ip6->currentText() == QString("stateless"))) {
            QStringList addresses;
            for (int i=0; i<ui->listWidget_ipAddress6->count(); i++)
                addresses.append(QString("'%1'").arg(ui->listWidget_ipAddress6->item(i)->text()));
            settings[QString("Address6")] = addresses.join(QChar(' '));
            settings[QString("Gateway6")] = QString("'%1'").arg(ui->lineEdit_gateway6->text());
        }
        if (ui->listWidget_ipRoutes6->count() > 0) {
            QStringList routes;
            for (int i=0; i<ui->listWidget_ipRoutes6->count(); i++)
                routes.append(QString("'%1'").arg(ui->listWidget_ipRoutes6->item(i)->text()));
            settings[QString("Routes6")] = routes.join(QChar(' '));
        }
    } else
        settings[QString("IP6")] = QString("no");
    if (ui->listWidget_custom->count() > 0) {
        QStringList custom;
        for (int i=0; i<ui->listWidget_custom->count(); i++)
            custom.append(QString("'%1'").arg(ui->listWidget_custom->item(i)->text()));
        settings[QString("IPCustom")] = custom.join(QChar(' '));
    }
    if (!ui->lineEdit_hostname->text().isEmpty())
        settings[QString("Hostname")] = QString("'%1'").arg(ui->lineEdit_hostname->text());
    if (ui->spinBox_timeoutDad->value() != 3)
        settings[QString("TimeoutDAD")] = QString::number(ui->spinBox_timeoutDad->value());
    if (ui->comboBox_dhcp->currentText() == QString("dhcpcd")) {
        if (!ui->lineEdit_dhcpcdOpt->text().isEmpty())
            settings[QString("DhcpcdOptions")] = QString("'%1'").arg(ui->lineEdit_dhcpcdOpt->text());
    } else if (ui->comboBox_dhcp->currentText() == QString("dhclient")) {
        settings[QString("DHCPClient")] = ui->comboBox_dhcp->currentText();
        if (!ui->lineEdit_dhclientOpt->text().isEmpty())
            settings[QString("DhclientOptions")] = QString("'%1'").arg(ui->lineEdit_dhclientOpt->text());
        if (!ui->lineEdit_dhclientOpt6->text().isEmpty())
            settings[QString("DhclientOptions6")] = QString("'%1'").arg(ui->lineEdit_dhclientOpt6->text());
    }
    if (ui->spinBox_timeoutDhcp->value() != 30)
        settings[QString("TimeoutDHCP")] = QString::number(ui->spinBox_timeoutDhcp->value());
    if (ui->checkBox_dhcp->checkState() == Qt::Checked)
        settings[QString("DHCPReleaseOnStop")] = QString("yes");
    if (ui->listWidget_dns->count() > 0) {
        QStringList dns;
        for (int i=0; i<ui->listWidget_dns->count(); i++)
            dns.append(QString("'%1'").arg(ui->listWidget_dns->item(i)->text()));
        settings[QString("DNS")] = dns.join(QChar(' '));
    }
    if (!ui->lineEdit_dnsDomain->text().isEmpty())
        settings[QString("DNSDomain")] = QString("'%1'").arg(ui->lineEdit_dnsDomain->text());
    if (!ui->lineEdit_dnsSearch->text().isEmpty())
        settings[QString("DNSSearch")] = QString("'%1'").arg(ui->lineEdit_dnsSearch->text());
    if (ui->listWidget_dnsOptions->count() > 0) {
        QStringList dnsOpt;
        for (int i=0; i<ui->listWidget_dnsOptions->count(); i++)
            dnsOpt.append(QString("'%1'").arg(ui->listWidget_dnsOptions->item(i)->text()));
        settings[QString("DNSOptions")] = dnsOpt.join(QChar(' '));
    }

    return settings;
}


int IpWidget::isOk()
{
    // ip settings is not set
    if ((ui->checkBox_ip->checkState() == Qt::Checked) &&
        (ui->comboBox_ip->currentText() == QString("static")))
        if ((ui->listWidget_ipAddress->count() == 0) ||
            (ui->lineEdit_gateway->text().isEmpty())) return 1;
    // ipv6 settings is not set
    if ((ui->checkBox_ip6->checkState() == Qt::Checked) &&
        ((ui->comboBox_ip6->currentText() == QString("static")) ||
         (ui->comboBox_ip6->currentText() == QString("stateless"))))
        if ((ui->listWidget_ipAddress6->count() == 0) ||
            (ui->lineEdit_gateway6->text().isEmpty())) return 2;
    // all fine
    return 0;
}


void IpWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("IP"))) {
        if (settings[QString("IP")] == QString("no"))
            ui->checkBox_ip->setCheckState(Qt::Unchecked);
        else {
            ui->checkBox_ip->setCheckState(Qt::Checked);
            int index = ui->comboBox_ip->findText(settings[QString("IP")]);
            ui->comboBox_ip->setCurrentIndex(index);
        }
    }
    if (settings.contains(QString("Address")))
        ui->listWidget_ipAddress->addItems(settings[QString("Address")].split(QChar('\n')));
    if (settings.contains(QString("Gateway")))
        ui->lineEdit_gateway->setText(settings[QString("Gateway")]);
    if (settings.contains(QString("Routes")))
        ui->listWidget_ipRoutes->addItems(settings[QString("Routes")].split(QChar('\n')));
    if (settings.contains(QString("IP6"))) {
        if (settings[QString("IP6")] == QString("no"))
            ui->checkBox_ip6->setCheckState(Qt::Unchecked);
        else {
            ui->checkBox_ip6->setCheckState(Qt::Checked);
            int index = ui->comboBox_ip6->findText(settings[QString("IP6")]);
            ui->comboBox_ip6->setCurrentIndex(index);
        }
    }
    if (settings.contains(QString("Address6")))
        ui->listWidget_ipAddress6->addItems(settings[QString("Address6")].split(QChar('\n')));
    if (settings.contains(QString("Gateway6")))
        ui->lineEdit_gateway6->setText(settings[QString("Gateway6")]);
    if (settings.contains(QString("Routes6")))
        ui->listWidget_ipRoutes6->addItems(settings[QString("Routes6")].split(QChar('\n')));
    if (settings.contains(QString("IPCustom")))
        ui->listWidget_custom->addItems(settings[QString("IPCustom")].split(QChar('\n')));
    if (settings.contains(QString("Hostname")))
        ui->lineEdit_hostname->setText(settings[QString("Hostname")]);
    if (settings.contains(QString("TimeoutDAD")))
        ui->spinBox_timeoutDad->setValue(settings[QString("TimeoutDAD")].toInt());
    if (settings.contains(QString("DHCPClient"))) {
        int index = ui->comboBox_dhcp->findText(settings[QString("DHCPClient")]);
        ui->comboBox_dhcp->setCurrentIndex(index);
    }
    if (settings.contains(QString("DhcpcdOptions")))
        ui->lineEdit_dhcpcdOpt->setText(settings[QString("DhcpcdOptions")]);
    if (settings.contains(QString("DhclientOptions")))
        ui->lineEdit_dhclientOpt->setText(settings[QString("DhclientOptions")]);
    if (settings.contains(QString("DhclientOptions6")))
        ui->lineEdit_dhclientOpt6->setText(settings[QString("DhclientOptions6")]);
    if (settings.contains(QString("TimeoutDHCP")))
        ui->spinBox_timeoutDhcp->setValue(settings[QString("TimeoutDHCP")].toInt());
    if (settings.contains(QString("DHCPReleaseOnStop")))
        if (settings[QString("DHCPReleaseOnStop")] == QString("yes"))
            ui->checkBox_dhcp->setCheckState(Qt::Checked);
    if (settings.contains(QString("DNS")))
        ui->listWidget_dns->addItems(settings[QString("DNS")].split(QChar('\n')));
    if (settings.contains(QString("DNSDomain")))
        ui->lineEdit_dnsDomain->setText(settings[QString("DNSDomain")]);
    if (settings.contains(QString("DNSSearch")))
        ui->lineEdit_dnsSearch->setText(settings[QString("DNSSearch")]);
    if (settings.contains(QString("DNSOptions")))
        ui->listWidget_dnsOptions->addItems(settings[QString("DNSOptions")].split(QChar('\n')));

    ipEnable(ui->checkBox_ip->checkState());
    changeIpMode(ui->comboBox_ip->currentText());
    ip6Enable(ui->checkBox_ip6->checkState());
    changeIp6Mode(ui->comboBox_ip6->currentText());
    changeDhcpClient(ui->comboBox_dhcp->currentText());
}
