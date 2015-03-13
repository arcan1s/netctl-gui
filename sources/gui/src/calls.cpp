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

#include "calls.h"

#include <QDebug>

#include <pdebug/pdebug.h>

#include "dbusoperation.h"


bool enableProfileSlot(const QString profile, Netctl *netctlCommand,
                       const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool current;
    if (useHelper) {
        // enable
        QList<QVariant> args;
        args.append(profile);
        sendRequestToCtrlWithArgs(QString("Enable"), args, debug);
        // check
        QList<QVariant> responce = sendRequestToLibWithArgs(QString("isProfileEnabled"), args, debug);
        if (responce.isEmpty())
            current = netctlCommand->isProfileEnabled(profile);
        else
            current = responce[0].toBool();
    } else {
        // enable
        netctlCommand->enableProfile(profile);
        // check
        current = netctlCommand->isProfileEnabled(profile);
    }

    return current;
}


bool restartProfileSlot(const QString profile, Netctl *netctlCommand,
                        const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool current;
    if (useHelper) {
        // restart
        QList<QVariant> args;
        args.append(profile);
        sendRequestToCtrlWithArgs(QString("Restart"), args, debug);
        // check
        QList<QVariant> responce = sendRequestToLibWithArgs(QString("isProfileActive"), args, debug);
        if (responce.isEmpty())
            current = netctlCommand->isProfileActive(profile);
        else
            current = responce[0].toBool();
    } else {
        // restart
        netctlCommand->restartProfile(profile);
        // check
        current = netctlCommand->isProfileActive(profile);
    }

    return current;
}


bool startProfileSlot(const QString profile, Netctl *netctlCommand,
                      const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool current;
    if (useHelper) {
        // get current
        QList<QVariant> args;
        args.append(profile);
        QList<QVariant> responce = sendRequestToLib(QString("ActiveProfile"), debug);
        QStringList currentProfile;
        if (!responce.isEmpty()) currentProfile = responce[0].toString().split(QChar('|'));
        // start or switch
        if ((currentProfile.isEmpty()) || (currentProfile.contains(profile)))
            sendRequestToCtrlWithArgs(QString("Start"), args, debug);
        else
            sendRequestToCtrlWithArgs(QString("SwitchTo"), args, debug);
        // check
        responce = sendRequestToLibWithArgs(QString("isProfileActive"), args, debug);
        if (responce.isEmpty())
            current = netctlCommand->isProfileActive(profile);
        else
            current = responce[0].toBool();
    } else {
        // get current
        QStringList currentProfile = netctlCommand->getActiveProfile();
        // start or switch
        if ((currentProfile.isEmpty()) || (currentProfile.contains(profile)))
            netctlCommand->startProfile(profile);
        else
            netctlCommand->switchToProfile(profile);
        // check
        current = netctlCommand->isProfileActive(profile);
    }

    return current;
}


bool MainWindow::stopAllProfilesSlot()
{
    if (debug) qDebug() << PDEBUG;

    if (useHelper)
        sendRequestToCtrl(QString("StolAll"), debug);
    else
        netctlCommand->stopAllProfiles();

    return true;
}


bool MainWindow::switchToProfileSlot(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool netctlAutoStatus = false;
    if (useHelper) {
        QList<QVariant> responce = sendRequestToLib(QString("isNetctlAutoActive"), debug);
        if (!responce.isEmpty()) netctlAutoStatus = responce[0].toBool();
    } else
        netctlAutoStatus = netctlCommand->isNetctlAutoRunning();

    bool current;
    if (netctlAutoStatus) {
        if (useHelper) {
            QList<QVariant> args;
            args.append(profile);
            sendRequestToCtrlWithArgs(QString("autoStart"), args, debug);
            QList<QVariant> responce = sendRequestToLibWithArgs(QString("autoIsProfileActive"), args, debug);
            if (responce.isEmpty())
                current = netctlCommand->autoIsProfileActive(profile);
            else
                current = responce[0].toBool();
        } else {
            netctlCommand->autoStartProfile(profile);
            current = netctlCommand->autoIsProfileActive(profile);
        }
    } else {
        if (useHelper) {
            QList<QVariant> args;
            args.append(profile);
            sendRequestToCtrlWithArgs(QString("SwitchTo"), args, debug);
            QList<QVariant> responce = sendRequestToLibWithArgs(QString("isProfileActive"), args, debug);
            if (responce.isEmpty())
                current = netctlCommand->isProfileActive(profile);
            else
                current = responce[0].toBool();
        } else {
            netctlCommand->switchToProfile(profile);
            current = netctlCommand->isProfileActive(profile);
        }
    }

    return current;
}
