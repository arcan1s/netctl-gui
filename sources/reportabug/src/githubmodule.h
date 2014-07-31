/***************************************************************************
 * This file is part of reportabug                                         *
 *                                                                         *
 * This library is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU Lesser General Public              *
 * License as published by the Free Software Foundation; either            *
 * version 3.0 of the License, or (at your option) any later version.      *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU        *
 * Lesser General Public License for more details.                         *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public        *
 * License along with this library.                                        *
 ***************************************************************************/
/**
 * @file githubmodule.h
 * Source code of reportabug
 * @author Evgeniy Alekseev
 * @copyright LGPLv3
 * @bug https://github.com/arcan1s/reportabug/issues
 */

#ifndef GITHUBMODULE_H
#define GITHUBMODULE_H

#include <QMap>
#include <QObject>


class Reportabug;
class QNetworkReply;

/**
 * @brief The GithubModule class creates issue over GitHub.
 * @details The GitHub API is used for creating issue. Please visit
 * [the API page](https://developer.github.com/v3/issues/) for more details.
 * This module requires an users authentication. The typical POST request is:
 * @code
 * curl -X POST -u user:pass -d '{"title":"A new bug","body":"Some error occurs"}' \
 *      https://api.github.com/repos/owner/repo/issues
 * @endcode
 *
 * The module also may send request using given token. In this case request is:
 * @code
 * curl -X POST -H "Authorization: token token" -d '{"title":"A new bug","body":"Some error occurs"}' \
 *      https://api.github.com/repos/owner/repo/issues
 * @endcode
 *
 * This module depends on QtNetwork module.
 */
class GithubModule : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GithubModule class constructor
     * @param parent         parent object
     * @param debugCmd       show debug messages
     */
    explicit GithubModule(QWidget *parent = 0,
                          bool debugCmd = false);
    /**
     * @brief GithubModule class destructor
     */
    ~GithubModule();

public slots:
    /**
     * @brief method which creates an issue
     * @param info           issue details. Needed keys are:
     *                       body (body of an issue),
     *                       password (GitHub password),
     *                       title (title of an issue),
     *                       userdata (given GitHub token, it is optional key),
     *                       username (GitHub user name),
     */
    void sendReportUsingGithub(const QMap<QString, QString> info);

private slots:
    /**
     * @brief method which will be called after sending POST request
     * @param reply          reply of the request
     */
    void githubFinished(QNetworkReply *reply);

private:
    /**
     * @brief show debug messages
     */
    bool debug;
    /**
     * @brief Reportabug class
     */
    Reportabug *mainWindow;
    /**
     * @brief method which parses string and replace known tags
     * @param line           source line
     * @return replaced line
     */
    QString parseString(QString line);
    /**
     * @brief method which prepares raw issue details for sending request
     * @param title          title of an issue
     * @param body           body of an issue
     * @return JSON string
     */
    QByteArray prepareRequest(const QString title, const QString body);
};


#endif /* GITHUBMODULE_H */
