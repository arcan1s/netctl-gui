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

#ifndef TESTNETCTLPROFILE_H
#define TESTNETCTLPROFILE_H

#include <QObject>
#include <QVariant>


class NetctlProfile;

class TestNetctlProfile : public QObject
{
    Q_OBJECT

private slots:
    // initialization
    void initTestCase();
    void cleanupTestCase();
    // netctl profile
    void test_getRecommendedConfiguration();
    void test_copyProfile();
    void test_createProfile();
    void test_getSettingsFromProfile();
    void test_getValueFromProfile();
    void test_removeProfile();

private:
    bool helper;
    NetctlProfile *createNetctlProfileObj();
    bool createTestProfile();
    bool removeTestProfile();
    QList<QVariant> sendDBusRequest(const QString path,
                                    const QString cmd,
                                    const QList<QVariant> args = QList<QVariant>());
};


#endif /* TESTNETCTLPROFILE_H */
