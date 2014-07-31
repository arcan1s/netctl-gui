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
 * @file gitreportmodule.cpp
 * Source code of reportabug
 * @author Evgeniy Alekseev
 * @copyright LGPLv3
 * @bug https://github.com/arcan1s/reportabug/issues
 */

#include "gitreportmodule.h"
#include "reportabug.h"

#include <QApplication>
#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QUrl>
#include <QWebElement>
#include <QWebFrame>

#include "config.h"


/**
 * @class GitreportModule
 */
/**
 * @fn GitreportModule
 */
GitreportModule::GitreportModule(QWidget *parent, bool debugCmd)
    : QObject(parent),
      debug(debugCmd),
      mainWindow((Reportabug *)parent)
{
    webView = new QWebView();
}


/**
 * @fn ~GitreportModule
 */
GitreportModule::~GitreportModule()
{
    if (debug) qDebug() << "[GitreportModule]" << "[~GitreportModule]";

    delete webView;
}


/**
 * @fn sendReportUsingGitreport
 */
void GitreportModule::sendReportUsingGitreport(const QMap<QString, QString> info)
{
    if (debug) qDebug() << "[GitreportModule]" << "[sendReportUsingGitreport]";

    QWebElement document = webView->page()->mainFrame()->documentElement();
    QWebElement captchaKey = document.findFirst(QString("input#captcha"));
    QWebElement emailInput = document.findFirst(QString("input#email"));
    QWebElement textArea = document.findFirst(QString("textarea#details"));
    QWebElement usernameInput = document.findFirst(QString("input#name"));

    // input
    usernameInput.setAttribute(QString("value"), info[QString("username")]);
    emailInput.setAttribute(QString("value"), info[QString("password")]);
    textArea.setPlainText(info[QString("body")]);
    captchaKey.setAttribute(QString("value"), info[QString("captcha")]);

    // send request
    document.findFirst(QString("input[name=commit]")).evaluateJavaScript("this.click()");
    disconnect(webView, SIGNAL(loadFinished(bool)), this, SLOT(gitreportLoaded(bool)));
    disconnect(webView, SIGNAL(loadFinished(bool)), this, SLOT(gitreportFinished(bool)));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(gitreportFinished(bool)));
}


/**
 * @fn gitreportLoaded
 */
void GitreportModule::gitreportLoaded(const bool state)
{
    if (debug) qDebug() << "[GitreportModule]" << "[gitreportLoaded]";
    if (debug) qDebug() << "[GitreportModule]" << "[gitreportLoaded]" << ":" << "State" << state;

    if (state) {
        webView->setHidden(!debug);
        // captcha
        QWebElement document = webView->page()->mainFrame()->documentElement();
        QWebElement captchaImg = document.findFirst(QString("input#captcha_key"));
        QString captchaUrl = QString(CAPTCHA_URL) + captchaImg.attribute(QString("value"));
        QNetworkRequest request(captchaUrl);
        manager.get(request);
        disconnect(&manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(setCaptcha(QNetworkReply *)));
        connect(&manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(setCaptcha(QNetworkReply *)));
    }
    else {
        QMessageBox messageBox;
        messageBox.setText(QApplication::translate("Reportabug", "Error!"));
        messageBox.setInformativeText(QApplication::translate("Reportabug", "An error occurs"));
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Retry);
        messageBox.setDefaultButton(QMessageBox::Ok);
        QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = (QGridLayout *)messageBox.layout();
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
        messageBox.exec();
    }
}


/**
 * @fn gitreportFinished
 */
void GitreportModule::gitreportFinished(const bool state)
{
    if (debug) qDebug() << "[GitreportModule]" << "[gitreportFinished]";
    if (debug) qDebug() << "[GitreportModule]" << "[gitreportFinished]" << ":" << "State" << state;

    QString messageBody, messageTitle;
    QMessageBox::Icon icon = QMessageBox::NoIcon;
    if (state) {
        messageBody += QApplication::translate("Reportabug", "Message has been sended");
        messageTitle = QApplication::translate("Reportabug", "Done!");
        icon = QMessageBox::Information;
    }
    else {
        messageBody += QApplication::translate("Reportabug", "An error occurs");
        messageTitle = QApplication::translate("Reportabug", "Error!");
        icon = QMessageBox::Critical;
    }

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


/**
 * @fn setCaptcha
 */
void GitreportModule::setCaptcha(QNetworkReply *reply)
{
    if (debug) qDebug() << "[setCaptcha]" << "[setCaptcha]";
    if (debug) qDebug() << "[setCaptcha]" << "[setCaptcha]" << ":" << "Error state" << reply->error();
    if (debug) qDebug() << "[setCaptcha]" << "[setCaptcha]" << ":" << "Reply size" << reply->readBufferSize();

    QByteArray answer = reply->readAll();
    QPixmap captcha;
    captcha.loadFromData(answer);
    mainWindow->setCaptchaImage(captcha);
    reply->deleteLater();
}
