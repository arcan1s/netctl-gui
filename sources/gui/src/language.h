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

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QStringList>


class Language : public QObject
{
    Q_OBJECT

public:
    explicit Language();
    static QString checkLanguage(const QString language,
                                 const QString defaultLanguage = QString("en"));
    static QString defineLanguage(const QString configPath,
                                  const QString options = QString("OPTIONS"));
    static QString defineLanguageFromFile(const QString configPath);
    static QString defineLanguageFromLocale();
    static QStringList getAvailableLanguages();

private:
    static QMap<QString, QString> parseOptions(const QString options);
};


#endif /* LANGUAGE_H */
