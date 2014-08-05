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

#include "aboutwindow.h"
#include "ui_aboutwindow.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QPushButton>

#include "version.h"


AboutWindow::AboutWindow(QWidget *parent, const bool debugCmd)
    : QMainWindow(parent),
      ui(new Ui::AboutWindow),
      debug(debugCmd)
{
    ui->setupUi(this);

    createUi();
}


AboutWindow::~AboutWindow()
{
    if (debug) qDebug() << "[AboutWindow]" << "[~AboutWindow]";

    delete uiAbout;
    delete ui;
}


void AboutWindow::createText()
{
    if (debug) qDebug() << "[AboutWindow]" << "[createText]";

    QStringList trdPartyList = QString(TRDPARTY_LICENSE).split(QChar(';'), QString::SkipEmptyParts);
    for (int i=0; i<trdPartyList.count(); i++)
        trdPartyList[i] = QString("<a href=\"%3\">%1</a> (%2 license)")
                .arg(trdPartyList[i].split(QChar(','))[0])
                .arg(trdPartyList[i].split(QChar(','))[1])
                .arg(trdPartyList[i].split(QChar(','))[2]);
    uiAbout->label_name->setText(QString(NAME));
    uiAbout->label_version->setText(QApplication::translate("AboutWindow", "Version %1\n(build date %2)")
                                    .arg(QString(VERSION)).arg(QString(BUILD_DATE)));
    uiAbout->label_description->setText(QApplication::translate("AboutWindow", "Qt-based graphical interface for netctl."));
    uiAbout->label_translators->setText(QApplication::translate("AboutWindow", "Translators: %1").arg(QString(TRANSLATORS)));
    uiAbout->label_links->setText(QApplication::translate("AboutWindow", "Links:") + QString("<br>") +
                                  QString("<a href=\"%1\">%2</a><br>").arg(QString(HOMEPAGE)).arg(QApplication::translate("AboutWindow", "Homepage")) +
                                  QString("<a href=\"%1\">%2</a><br>").arg(QString(REPOSITORY)).arg(QApplication::translate("AboutWindow", "Repository")) +
                                  QString("<a href=\"%1\">%2</a><br>").arg(QString(BUGTRACKER)).arg(QApplication::translate("AboutWindow", "Bugtracker")) +
                                  QString("<a href=\"%1\">%2</a><br>").arg(QString(TRANSLATION)).arg(QApplication::translate("AboutWindow", "Translation issue")) +\
                                  QString("<a href=\"%1\">%2</a>").arg(QString(AUR_PACKAGES)).arg(QApplication::translate("AboutWindow", "AUR packages")));
    uiAbout->label_license->setText(QString("<small>&copy; %1 <a href=\"mailto:%2\">%3</a><br>").arg(QString(DATE)).arg(QString(EMAIL)).arg(QString(AUTHOR)) +
                                    QApplication::translate("AboutWindow", "This software is licensed under %1").arg(QString(LICENSE)) + QString("<br>") +
                                    QApplication::translate("AboutWindow", "This software uses: %1").arg(trdPartyList.join(QString(", "))) + QString("</small>"));
}


void AboutWindow::createUi()
{
    if (debug) qDebug() << "[AboutWindow]" << "[createUi]";

    QWidget *aboutWidget = new QWidget();
    uiAbout = new Ui::About;
    uiAbout->setupUi(aboutWidget);
    ui->verticalLayout->addWidget(aboutWidget);
    createText();

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(QDialogButtonBox::Close);
    ui->verticalLayout->addWidget(buttonBox);
    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked(bool)), this, SLOT(close()));
}


// ESC press event
void AboutWindow::keyPressEvent(QKeyEvent *pressedKey)
{
    if (debug) qDebug() << "[AboutWindow]" << "[keyPressEvent]";

    if (pressedKey->key() == Qt::Key_Escape)
        close();
}
