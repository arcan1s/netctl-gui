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

#include "openvswitchwidget.h"
#include "ui_openvswitchwidget.h"


OpenvswitchWidget::OpenvswitchWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::OpenvswitchWidget)
{
    ui->setupUi(this);
    clear();
}


OpenvswitchWidget::~OpenvswitchWidget()
{
    delete ui;
}


void OpenvswitchWidget::clear()
{
}


void OpenvswitchWidget::setShown(const bool state)
{
    if (state)
        show();
    else
        hide();
}


QMap<QString, QString> OpenvswitchWidget::getSettings()
{
    QMap<QString, QString> openvswitchSettings;

    if (isOk() != 0)
        return openvswitchSettings;

    return openvswitchSettings;
}


int OpenvswitchWidget::isOk()
{
    // all fine
    return 0;
}


void OpenvswitchWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();
    QMap<QString, QString> OpenvswitchWidget = settings;
}
