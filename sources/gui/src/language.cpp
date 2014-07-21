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


Language::Language()
{
}


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


QString Language::defineLanguage(const QString configPath, const QString options)
{
    QMap<QString, QString> optionsDict = parseOptions(options);
    if (optionsDict.contains(QString("LANGUAGE")))
        if (getAvailableLanguages().contains(optionsDict[QString("LANGUAGE")]))
            return optionsDict[QString("LANGUAGE")];

    QString language;
    language = defineLanguageFromFile(configPath);
    if (language.isEmpty())
        language = defineLanguageFromLocale();
    language = checkLanguage(language, QString("en"));

    return language;
}


QString Language::defineLanguageFromFile(const QString configPath)
{
    QMap<QString, QString> settings;
    if (configPath.isEmpty())
        return QString("");
    QFile configFile(configPath);
    QString fileStr;
    if (!configFile.open(QIODevice::ReadOnly))
        return QString("");
    while (true) {
        fileStr = QString(configFile.readLine()).trimmed();
        if ((fileStr.isEmpty()) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar('#')) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar(';')) && (!configFile.atEnd())) continue;
        if ((!fileStr.contains(QChar('='))) && (!configFile.atEnd())) continue;
        settings[fileStr.split(QChar('='))[0]] = fileStr.split(QChar('='))[1];
        if (configFile.atEnd()) break;
    }
    configFile.close();

    if (settings.contains(QString("LANGUAGE")))
        return settings[QString("LANGUAGE")];
    else
        return QString("");
}


QString Language::defineLanguageFromLocale()
{
    return QLocale::system().name();
}


QStringList Language::getAvailableLanguages()
{
    QStringList languages;
    languages.append(QString("en"));
    languages.append(QString("ru"));

    return languages;
}


QMap<QString, QString> Language::parseOptions(const QString options)
{
    QMap<QString, QString> optionsDict;
    for (int i=0; i<options.split(QChar(',')).count(); i++) {
        if (options.split(QChar(','))[i].split(QChar('=')).count() < 2)
            continue;
        optionsDict[options.split(QChar(','))[i].split(QChar('='))[0]] =
                options.split(QChar(','))[i].split(QChar('='))[1];
    }

    return optionsDict;
}
