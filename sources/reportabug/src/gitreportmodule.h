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
 * @file gitreportmodule.h
 * Source code of reportabug
 * @author Evgeniy Alekseev
 * @copyright LGPLv3
 * @bug https://github.com/arcan1s/reportabug/issues
 */

#ifndef GITREPORTMODULE_H
#define GITREPORTMODULE_H

#include <QMap>
#include <QObject>
#include <QWebView>

class QNetworkAccessManager;
class Reportabug;

/**
 * @brief The GitreportModule class creates issue over GitReports
 * @details This module uses [GitReport](https://gitreports.com/about) for
 * creating an issue. Please visit [this page](https://gitreports.com/)
 * and set up it for your repository.
 *
 * This module depends on QtWebKit and QtNetwork modules.
 */
class GitreportModule : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GitreportModule class constructor
     * @param parent         parent object
     * @param debugCmd       show debug messages
     */
    explicit GitreportModule(QWidget *parent = 0,
                             bool debugCmd = false);
    /**
     * @brief GitreportModule class destructor
     */
    ~GitreportModule();
    /**
     * @brief Webkit item which is used for loading web page
     */
    QWebView *webView;

public slots:
    /**
     * @brief method which creates an issue
     * @param info           issue details. Needed keys are:
     *                       body (body of an issue)
     *                       password (user email),
     *                       username (user name),
     */
    void sendReportUsingGitreport(const QMap<QString, QString> info);

private slots:
    /**
     * @brief method which will be called after sending request
     * @param bool           state of web page loading
     */
    void gitreportFinished(const bool state);
    /**
     * @brief method which will be called after loading of web page
     * @param bool           state of web page loading
     */
    void gitreportLoaded(const bool state);
    /**
     * @brief method which will be called after loading captcha image
     * @param reply          reply of the request
     */
    void setCaptcha(QNetworkReply *reply);

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
     * @brief manager for getting captcha
     */
    QNetworkAccessManager manager;
};


#endif /* GITREPORTMODULE_H */
