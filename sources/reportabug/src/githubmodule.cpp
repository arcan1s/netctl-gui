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
 * @file githubmodule.cpp
 * Source code of reportabug
 * @author Evgeniy Alekseev
 * @copyright LGPLv3
 * @bug https://github.com/arcan1s/reportabug/issues
 */

#include "githubmodule.h"
#include "reportabug.h"

#include <QApplication>
#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include "config.h"


/**
 * @class GithubModule
 */
/**
 * @fn GithubModule
 */
GithubModule::GithubModule(QWidget *parent, bool debugCmd)
    : QObject(parent),
      debug(debugCmd),
      mainWindow((Reportabug *)parent)
{
}


/**
 * @fn ~GithubModule
 */
GithubModule::~GithubModule()
{
    if (debug) qDebug() << "[GithubModule]" << "[~GithubModule]";
}


/**
 * @fn prepareRequest
 */
QByteArray GithubModule::prepareRequest(const QString title, const QString body)
{
    if (debug) qDebug() << "[GithubModule]" << "[prepareRequest]";
    if (debug) qDebug() << "[GithubModule]" << "[prepareRequest]" << ":" << "Title" << title;
    if (debug) qDebug() << "[GithubModule]" << "[prepareRequest]" << ":" << "Title" << body;

    QStringList requestList;
    requestList.append(QString("\"title\":\"") + title + QString("\""));
    QString fixBody = body;
    fixBody.replace(QString("\n"), QString("<br>"));
    requestList.append(QString("\"body\":\"") + fixBody + QString("\""));
    if (!QString(TAG_ASSIGNEE).isEmpty())
        requestList.append(QString("\"assignee\":\"") + parseString(QString(TAG_ASSIGNEE)) + QString("\""));
    if (!QString(TAG_MILESTONE).isEmpty())
        requestList.append(QString("\"milestone\":") + QString(TAG_MILESTONE));
    if (!QString(TAG_LABELS).isEmpty()) {
        QStringList labels = QString(TAG_LABELS).split(QChar(','));
        for (int i=0; i<labels.count(); i++)
            labels[i] = QString("\"") + labels[i] + QString("\"");
        requestList.append(QString("\"labels\":[") + labels.join(QChar(',')) + QString("]"));
    }

    QString request;
    request += QString("{");
    request += requestList.join(QChar(','));
    request += QString("}");

    return request.toLocal8Bit();
}


/**
 * @fn parseString
 */
QString GithubModule::parseString(QString line)
{
    if (debug) qDebug() << "[GithubModule]" << "[parseString]";
    if (debug) qDebug() << "[GithubModule]" << "[parseString]" << ":" << "Parse line" << line;

    if (line.contains(QString("$OWNER")))
        line = line.split(QString("$OWNER"))[0] +
                QString(OWNER) +
                line.split(QString("$OWNER"))[1];
    if (line.contains(QString("$PROJECT")))
        line = line.split(QString("$PROJECT"))[0] +
                QString(PROJECT) +
                line.split(QString("$PROJECT"))[1];

    return line;
}


/**
 * @fn sendReportUsingGithub
 */
void GithubModule::sendReportUsingGithub(const QMap<QString, QString> info)
{
    if (debug) qDebug() << "[GithubModule]" << "[sendReportUsingGithub]";

    // authentication
    QString headerData;
    if (info.contains(QString("userdata")))
        headerData = QString("token ") + info[QString("userdata")];
    else {
        QString concatenated = info[QString("username")] + QString(":") + info[QString("password")];
        QByteArray userData = concatenated.toLocal8Bit().toBase64();
        headerData = QString("Basic ") + userData;
    }
    // text
    QByteArray text = prepareRequest(info[QString("title")], info[QString("body")]);
    QByteArray textSize = QByteArray::number(text.size());
    // sending request
    QNetworkRequest request = QNetworkRequest(parseString(QString(ISSUES_URL)));
    request.setRawHeader("Authorization", headerData.toLocal8Bit());
    request.setRawHeader("User-Agent", "reportabug");
    request.setRawHeader("Host", "api.github.com");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Content-Type", "application/vnd.github.VERSION.raw+json");
    request.setRawHeader("Content-Length", textSize);
    QNetworkAccessManager *manager = new QNetworkAccessManager;
    manager->post(request, text);
    disconnect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(githubFinished(QNetworkReply *)));
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(githubFinished(QNetworkReply *)));
}


/**
 * @fn githubFinished
 */
void GithubModule::githubFinished(QNetworkReply *reply)
{
    if (debug) qDebug() << "[GithubModule]" << "[githubFinished]";
    if (debug) qDebug() << "[GithubModule]" << "[githubFinished]" << ":" << "Error state" << reply->error();
    if (debug) qDebug() << "[GithubModule]" << "[githubFinished]" << ":" << "Reply size" << reply->readBufferSize();

    bool state = true;
    QString answer = reply->readAll();
    if (debug) qDebug() << "[GithubModule]" << "[replyFinished]" << ":" << answer;
    QString messageBody, messageTitle;
    QMessageBox::Icon icon = QMessageBox::NoIcon;
    if (answer.contains(QString("\"html_url\":"))) {
        QString url;
        for (int i=0; i<answer.split(QChar(',')).count(); i++)
            if (answer.split(QChar(','))[i].split(QChar(':'))[0] == QString("\"html_url\"")) {
                url = answer.split(QChar(','))[i].split(QString("\"html_url\":"))[1].remove(QChar('"'));
                break;
            }
        messageBody += QString("%1\n").arg(QApplication::translate("Reportabug", "Message has been sended"));
        messageBody += QString("Url: %2").arg(url);
        messageTitle = QApplication::translate("Reportabug", "Done!");
        icon = QMessageBox::Information;
        state = true;
    }
    else if (answer.contains(QString("\"Bad credentials\""))) {
        messageBody += QApplication::translate("Reportabug", "Incorrect username or password");
        messageTitle = QApplication::translate("Reportabug", "Error!");
        icon = QMessageBox::Critical;
        state = false;
    }
    else {
        messageBody += QApplication::translate("Reportabug", "An error occurs");
        messageTitle = QApplication::translate("Reportabug", "Error!");
        icon = QMessageBox::Critical;
        state = false;
    }
    reply->deleteLater();

    QMessageBox messageBox;
    messageBox.setText(messageTitle);
    messageBox.setInformativeText(messageBody);
    messageBox.setIcon(icon);
    messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Retry);
    messageBox.setDefaultButton(QMessageBox::Ok);
    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = (QGridLayout *)messageBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    int ret = messageBox.exec();

    switch (ret) {
    case QMessageBox::Ok:
        if (state) mainWindow->close();
        break;
    case QMessageBox::Retry:
        if (state) mainWindow->externalUpdateTab();
        break;
    default:
        break;
    }
}
