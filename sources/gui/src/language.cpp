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


#include "language.h"

#include <QFile>
#include <QLocale>


Language::Language(const QString configPath)
{
}


QString Language::defineLanguage(const QString configPath)
{
    QString language;
    language = defineLanguageFromFile(configPath);
    if (language.isEmpty())
        language = defineLanguageFromLocale();
    language = checkLanguage(language, QString("en"));
    return language;
}


QString Language::defineLanguageFromFile(const QString configPath)
{
    QString language;
    if (configPath.isEmpty())
        return language;
    QFile configFile(configPath);
    QString fileStr;
    if (configFile.open(QIODevice::ReadOnly))
        while (true) {
            fileStr = QString(configFile.readLine());
            if (fileStr[0] != '#') {
                if (fileStr.contains(QString("LANGUAGE=")))
                    language = fileStr.split(QString("="))[1]
                            .remove(QString(" "))
                            .trimmed();
            }
            if (configFile.atEnd())
                break;
        }
    configFile.close();
    return language;
}


QString Language::defineLanguageFromLocale()
{
    return QLocale::system().name();
}
\

QString Language::checkLanguage(const QString language, const QString defaultLanguage)
{
    QStringList availableLanguages = getAvailableLanguages();
    for (int i=0; i<availableLanguages.count(); i++)
        if (language == availableLanguages[i])
            return availableLanguages[i];
    for (int i=0; i<availableLanguages.count(); i++)
        if (language.contains(availableLanguages[i] + QChar('_')))
            return availableLanguages[i];
    return defaultLanguage;
}


QStringList Language::getAvailableLanguages()
{
    QStringList languages;
    languages.append(QString("en"));
    languages.append(QString("ru"));
    return languages;
}
