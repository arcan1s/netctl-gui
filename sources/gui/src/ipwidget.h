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

#ifndef IPWIDGET_H
#define IPWIDGET_H

#include <QWidget>


namespace Ui {
class IpWidget;
}

class IpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IpWidget(QWidget *parent = 0);
    ~IpWidget();
    QMap<QString, QString> getSettings();
    int isOk();
    void setSettings(const QMap<QString, QString> settings);

public slots:
    void clear();
    void setShown(const bool state);

private slots:
    // buttons
    void addIp();
    void addIpRoutes();
    void addIp6();
    void addIpRoutes6();
    void addCustom();
    void addDns();
    void addDnsOpt();
    // ip mode
    void changeIpMode(const QString currentText);
    void ipEnable(const int state);
    // ipv6 mode
    void changeIp6Mode(const QString currentText);
    void ip6Enable(int state);
    // dhcp client
    void changeDhcpClient(const QString currentText);
    void showAdvanced();

private:
    Ui::IpWidget *ui;
    void createActions();
    void createFilter();
    void keyPressEvent(const QKeyEvent *pressedKey);
    QString getIp(const QString rawIp);
    QString getPrefix(const QString rawPrefix);
    QString getIp6(const QString rawIp);
    QString getPrefix6(const QString rawPrefix);
};


#endif /* IPWIDGET_H */
