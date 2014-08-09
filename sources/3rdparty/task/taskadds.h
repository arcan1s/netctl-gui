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


#ifndef TASKADDS_H
#define TASKADDS_H

#include <QProcess>
#include <unistd.h>

#include "task.h"


class RootProcess : public QProcess
{
protected:
    void setupChildProcess()
    {
        ::setuid(0);
    };
};


struct TaskResult
{
    int exitCode;
    QByteArray output;
};
TaskResult runTask(const QString cmd, const bool useSuid = true);


#endif /* TASKADDS_H */
