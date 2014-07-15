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
/**
 * @file sleepthread.h
 * Header of netctlgui library
 * @author Evgeniy Alekseev
 * @copyright GPLv3
 * @bug https://github.com/arcan1s/netctl-gui/issues
 */


#ifndef SLEEPTHREAD_H
#define SLEEPTHREAD_H

#include <QThread>


/**
 * @brief The SleepThread class is used for sleep current thread in WpaSup class
 */
class SleepThread : public QThread
{
    Q_OBJECT

public:
    /**
     * @brief method which forces the current thread to sleep for usecs microseconds
     * @param iSleepTime     time in microseconds
     */
    static void usleep(long iSleepTime)
    {
        QThread::usleep(iSleepTime);
    }
    /**
     * @brief method which forces the current thread to sleep for usecs seconds
     * @param iSleepTime     time in seconds
     */
    static void sleep(long iSleepTime)
    {
        QThread::sleep(iSleepTime);
    }
    /**
     * @brief method which forces the current thread to sleep for usecs milliseconds
     * @param iSleepTime     time in milliseconds
     */
    static void msleep(long iSleepTime)
    {
        QThread::msleep(iSleepTime);
    }
};


#endif /* SLEEPTHREAD_H */
