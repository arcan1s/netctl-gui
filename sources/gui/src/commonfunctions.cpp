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

#include "commonfunctions.h"

#include <QApplication>
#include <QDebug>

#include <pdebug/pdebug.h>
#include <task/taskadds.h>


bool checkExternalApps(const QString apps, const QMap<QString, QString> configuration,
                       const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Applications" << apps;

    if (configuration[QString("SKIPCOMPONENTS")] == QString("true")) return true;
    QStringList cmd;
    cmd.append("which");
    // avoid null-lines arguments
    cmd.append("true");
    cmd.append(externalApps(apps, configuration));

    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd.join(QChar(' '));
    TaskResult process = runTask(cmd.join(QChar(' ')), false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return (process.exitCode == 0);
}


QString checkStatus(const bool statusBool, const bool nullFalse)
{
    if (statusBool) return QApplication::translate("MainWindow", "yes");
    if (!nullFalse) return QApplication::translate("MainWindow", "no");

    return QString("");
}


QStringList externalApps(const QString apps, const QMap<QString, QString> configuration)
{
    QStringList app;
    // editor works always over cmd
    if ((apps == QString("editor")) || (apps == QString("all"))) {
        app.append(configuration[QString("EDITOR_PATH")]);
        app.append(configuration[QString("SUDO_PATH")]);
    }
    if ((apps == QString("helper")) || (apps == QString("all"))) {
        app.append(configuration[QString("HELPER_PATH")]);
    }
    if ((apps == QString("netctl")) || (apps == QString("all"))) {
        app.append(configuration[QString("NETCTL_PATH")]);
        app.append(configuration[QString("NETCTLAUTO_PATH")]);
    }
    if ((apps == QString("systemctl")) || (apps == QString("all"))) {
        app.append(configuration[QString("SYSTEMCTL_PATH")]);
    }
    if ((apps == QString("wpasup")) || (apps == QString("wpasup-only")) || (apps == QString("all"))) {
        app.append(configuration[QString("WPACLI_PATH")]);
        app.append(configuration[QString("WPASUP_PATH")]);
    }
    // append sudo
    // FORCE_SUDO is always true if helper is not running
    if ((apps.contains(QRegExp(QString("(^all$|^editor$|^netctl$|^sudo$|^systemctl$|^wpasup$)")))) &&
        (configuration[QString("FORCE_SUDO")] == QString("true")))
        app.append(configuration[QString("SUDO_PATH")]);

    return app;
}
