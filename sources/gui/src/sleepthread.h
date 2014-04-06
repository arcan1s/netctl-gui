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

#ifndef SLEEPTHREAD_H
#define SLEEPTHREAD_H

#include <QThread>


class SleepThread : public QThread
{
    Q_OBJECT

public:
    static void usleep(long iSleepTime)
    {
        QThread::usleep(iSleepTime);
    }

    static void sleep(long iSleepTime)
    {
        QThread::sleep(iSleepTime);
    }

    static void msleep(long iSleepTime)
    {
        QThread::msleep(iSleepTime);
    }
};


#endif /* SLEEPTHREAD_H */
