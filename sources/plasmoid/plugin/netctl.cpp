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

#include <QtQml>

#include <pdebug/pdebug.h>

#include "netctl.h"
#include "netctladds.h"
#include "version.h"


void NetctlPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.private.netctl"));

    qmlRegisterType<NetctlAdds>(uri, 1, 0, "NetctlAdds");
//    qmlRegisterType<TimeZoneModel>(uri, 1, 0, "TimeZoneModel");
//    qmlRegisterSingletonType<TimezonesI18n>(uri, 1, 0, "TimezonesI18n", timezonesi18n_singletontype_provider);
}
