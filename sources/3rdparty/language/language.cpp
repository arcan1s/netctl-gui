/***************************************************************************
 *  Copyright (C) 2014  Evgeniy Alekseev                                      *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 3.0 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library.                                       *
 ***************************************************************************/


#include "language.h"

#include <QFile>
#include <QLocale>

#include "config.h"


Language::Language()
{
}


QString Language::checkLanguage(const QString language)
{
    QStringList availableLanguages = getAvailableLanguages();
    if (availableLanguages.count() == 0) return QString();
    for (int i=0; i<availableLanguages.count(); i++)
        if (language == availableLanguages[i])
            return availableLanguages[i];
    for (int i=0; i<availableLanguages.count(); i++)
        if (language.contains(availableLanguages[i] + QChar('_')))
            return availableLanguages[i];

    return availableLanguages[0];
}


QString Language::defineLanguage(const QString configPath, const QString options)
{
    QMap<QString, QString> optionsDict = parseOptions(options);
    if (optionsDict.contains(QString(LANGUAGE_KEY)))
        if (getAvailableLanguages().contains(optionsDict[QString(LANGUAGE_KEY)]))
            return optionsDict[QString(LANGUAGE_KEY)];

    QString language;
    language = defineLanguageFromFile(configPath);
    if (language.isEmpty())
        language = defineLanguageFromLocale();
    language = checkLanguage(language);

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
        if (fileStr.contains(QChar('=')))
            settings[fileStr.split(QChar('='))[0]] = fileStr.split(QChar('='))[1];
        if (configFile.atEnd()) break;
    }
    configFile.close();

    if (settings.contains(QString(LANGUAGE_KEY)))
        return settings[QString(LANGUAGE_KEY)];
    else
        return QString("");
}


QString Language::defineLanguageFromLocale()
{
    return QLocale::system().name();
}


QStringList Language::getAvailableLanguages()
{
    return QString(LANGUAGES).split(QChar(','));
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
