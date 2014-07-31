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
 * @file reportabug.cpp
 * Source code of reportabug
 * @author Evgeniy Alekseev
 * @copyright LGPLv3
 * @bug https://github.com/arcan1s/reportabug/issues
 */

#include "reportabug.h"
#include "ui_reportabug.h"

#include <QDebug>
#include <QPushButton>

#include "config.h"
#if defined(ENABLE_GITHUB) || defined(OWN_GITHUB_TOKEN)
#include "githubmodule.h"
#endif /* defined(ENABLE_GITHUB) || defined(OWN_GITHUB_TOKEN) */
#ifdef ENABLE_GITREPORT
#include "gitreportmodule.h"
#endif /* ENABLE_GITREPORT */
#include "version.h"


/**
 * @class Reportabug
 */
/**
 * @fn Reportabug
 */
Reportabug::Reportabug(QWidget *parent, bool debugCmd)
    : QMainWindow(parent),
      debug(debugCmd),
      ui(new Ui::Reportabug)
{
    ui->setupUi(this);
    initModules();
    createComboBox();
    createActions();
}


/**
 * @fn ~Reportabug
 */
Reportabug::~Reportabug()
{
    if (debug) qDebug() << "[Reportabug]" << "[~Reportabug]";

#if defined(ENABLE_GITHUB) || defined(OWN_GITHUB_TOKEN)
    delete github;
#endif /* defined(ENABLE_GITHUB) || defined(OWN_GITHUB_TOKEN) */
#ifdef ENABLE_GITREPORT
    delete gitreport;
#endif /* ENABLE_GITREPORT */
    delete ui;
}


/**
 * @fn externalUpdateTab
 */
void Reportabug::externalUpdateTab()
{
    if (debug) qDebug() << "[Reportabug]" << "[externalUpdateTab]";

    return updateTabs(ui->comboBox->currentIndex());
}


/**
 * @fn setCaptchaImage
 */
void Reportabug::setCaptchaImage(QPixmap captcha)
{
    if (debug) qDebug() << "[Reportabug]" << "[setCaptchaImage]";

    ui->label_captcha->setPixmap(captcha);
}


/**
 * @fn createActions
 */
void Reportabug::createActions()
{
    if (debug) qDebug() << "[Reportabug]" << "[createActions]";

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTabs(int)));
    connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(sendReport()));
}


/**
 * @fn createComboBox
 */
void Reportabug::createComboBox()
{
    if (debug) qDebug() << "[Reportabug]" << "[createComboBox]";

    ui->comboBox->clear();
    if (modules[0])
        ui->comboBox->addItem(GITHUB_COMBOBOX);
    if (modules[1] || modules[2])
        ui->comboBox->addItem(GITREPORT_COMBOBOX);
}


/**
 * @fn getNumberByIndex
 */
int Reportabug::getNumberByIndex(const int index)
{
    if (debug) qDebug() << "[Reportabug]" << "[getNumberByIndex]";
    if (debug) qDebug() << "[Reportabug]" << "[getNumberByIndex]" << ":" << "Index" << index;

    if ((modules[0]) && (modules[1])) {
        if (index == 0)
            return 0;
        else if (index == 1)
            return 1;
    }
    else if ((modules[0]) && (modules[2])) {
        if (index == 0)
            return 0;
        else if (index == 1)
            return 2;
    }
    else if ((modules[1]) && (modules[2]))
        // wtf??
        return -1;
    else if (modules[0])
        return 0;
    else if (modules[1])
        return 1;
    else if (modules[2])
        return 2;

    return -1;
}


/**
 * @fn initModules
 */
void Reportabug::initModules()
{
    if (debug) qDebug() << "[Reportabug]" << "[initModules]";

    modules[0] = false;
    modules[1] = false;
    modules[2] = false;

#if defined(ENABLE_GITHUB) || defined(OWN_GITHUB_TOKEN)
    github = new GithubModule(this, debug);
#endif /* defined(ENABLE_GITHUB) || defined(OWN_GITHUB_TOKEN) */
#ifdef ENABLE_GITHUB
    modules[0] = true;
#endif /* ENABLE_GITHUB */
#ifdef ENABLE_GITREPORT
    modules[1] = true;
    gitreport = new GitreportModule(this, debug);
    // 4 is a magic number. Seriously
    ui->verticalLayout->insertWidget(5, gitreport->webView);
#endif /* ENABLE_GITREPORT */
#ifdef OWN_GITHUB_TOKEN
    modules[2] = true;
#endif /* OWN_GITHUB_TOKEN */
}


/**
 * @fn keyPressEvent
 */
void Reportabug::keyPressEvent(QKeyEvent *pressedKey)
{
    if (debug) qDebug() << "[Reportabug]" << "[keyPressEvent]";

    if (pressedKey->key() == Qt::Key_Escape)
        close();
}


/**
 * @fn parseString
 */
QString Reportabug::parseString(QString line)
{
    if (debug) qDebug() << "[Reportabug]" << "[parseString]";
    if (debug) qDebug() << "[Reportabug]" << "[parseString]" << ":" << "Parse line" << line;

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
 * @fn sendReport
 */
void Reportabug::sendReport()
{
    if (debug) qDebug() << "[Reportabug]" << "[sendReport]";

    int number = getNumberByIndex(ui->comboBox->currentIndex());
    QMap<QString, QString> info;
    info[QString("body")] = ui->textEdit->toPlainText();
    info[QString("captcha")] = ui->lineEdit_captcha->text();
    info[QString("password")] = ui->lineEdit_password->text();
    info[QString("title")] = ui->lineEdit_title->text();
    info[QString("username")] = ui->lineEdit_username->text();

    if (number == -1)
        return;
#ifdef ENABLE_GITHUB
    else if (number == 0)
        github->sendReportUsingGithub(info);
#endif /* ENABLE_GITHUB */
#ifdef ENABLE_GITREPORT
    else if (number == 1)
        gitreport->sendReportUsingGitreport(info);
#endif /* ENABLE_GITREPORT */
#ifdef OWN_GITHUB_TOKEN
    else if (number == 2) {
        info[QString("userdata")] = QString(OWN_GITHUB_TOKEN);
        github->sendReportUsingGithub(info);
    }
#endif /* OWN_GITHUB_TOKEN */
}


/**
 * @fn showWindow
 */
void Reportabug::showWindow()
{
    externalUpdateTab();
    show();
}


/**
 * @fn updateTabs
 */
void Reportabug::updateTabs(const int index)
{
    if (debug) qDebug() << "[Reportabug]" << "[updateTabs]";
    if (debug) qDebug() << "[Reportabug]" << "[updateTabs]" << ":" << "Index" << index;

    int number = getNumberByIndex(index);
    ui->lineEdit_username->clear();
    ui->lineEdit_password->clear();
    ui->lineEdit_title->setText(QString(TAG_TITLE));
    ui->textEdit->setPlainText(QString(TAG_BODY));
    ui->lineEdit_captcha->clear();

    // it is out of conditional because I don't want a lot of ifdef/endif
#ifdef ENABLE_GITREPORT
    gitreport->webView->setHidden(true);
#endif /* ENABLE_GITREPORT */
    if (number == -1) {
        ui->widget_auth->setHidden(true);
        ui->widget_title->setHidden(true);
        ui->textEdit->setHidden(true);
        ui->widget_captcha->setHidden(true);
    }
#ifdef ENABLE_GITHUB
    else if (number == 0) {
        ui->widget_auth->setHidden(false);
        ui->widget_title->setHidden(false);
        ui->textEdit->setHidden(false);
        ui->widget_captcha->setHidden(true);
        ui->label_password->setText(QApplication::translate("Reportabug", "Password"));
        ui->label_password->setToolTip(QApplication::translate("Reportabug", "GitHub account password"));
        ui->lineEdit_password->setPlaceholderText(QApplication::translate("Reportabug", "password"));
        ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    }
#endif /* ENABLE_GITHUB */
#ifdef ENABLE_GITREPORT
    else if (number == 1) {
        ui->widget_auth->setHidden(false);
        ui->widget_title->setHidden(true);
        ui->textEdit->setHidden(false);
        ui->widget_captcha->setHidden(false);
        ui->label_password->setText(QApplication::translate("Reportabug", "Email"));
        ui->label_password->setToolTip(QApplication::translate("Reportabug", "Your email"));
        ui->lineEdit_password->setPlaceholderText(QApplication::translate("Reportabug", "email"));
        ui->lineEdit_password->setEchoMode(QLineEdit::Normal);

        gitreport->webView->load(QUrl(parseString(QString(PUBLIC_URL))));
        disconnect(gitreport->webView, SIGNAL(loadFinished(bool)), gitreport, SLOT(gitreportLoaded(bool)));
        disconnect(gitreport->webView, SIGNAL(loadFinished(bool)), gitreport, SLOT(gitreportFinished(bool)));
        connect(gitreport->webView, SIGNAL(loadFinished(bool)), gitreport, SLOT(gitreportLoaded(bool)));
    }
#endif /* ENABLE_GITREPORT */
#ifdef OWN_GITHUB_TOKEN
    else if (number == 2) {
        ui->widget_auth->setHidden(true);
        ui->widget_title->setHidden(false);
        ui->textEdit->setHidden(false);
    }
#endif /* OWN_GITHUB_TOKEN */
}
