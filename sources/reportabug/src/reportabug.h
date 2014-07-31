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
 * @file reportabug.h
 * Source code of reportabug
 * @author Evgeniy Alekseev
 * @copyright LGPLv3
 * @bug https://github.com/arcan1s/reportabug/issues
 */

#ifndef REPORTABUG_H
#define REPORTABUG_H

#include <QKeyEvent>
#include <QLocale>
#include <QMainWindow>


class GithubModule;
class GitreportModule;

namespace Ui {
class Reportabug;
}

/**
 * @brief The Reportabug class is a main class of the application
 */
class Reportabug : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Reportabug class constructor
     * @param parent         parent object
     * @param debugCmd       show debug messages
     */
    explicit Reportabug(QWidget *parent = 0,
                        bool debugCmd = false);
    /**
     * @brief Reportabug class destructor
     */
    ~Reportabug();
    /**
     * @brief method which is used for update tabs from external classes
     */
    void externalUpdateTab();
    /**
     * @brief method which sets captcha image
     * @param captcha        captcha image
     */
    void setCaptchaImage(QPixmap captcha);

public slots:
    /**
     * @brief method which creates an issue
     */
    void sendReport();
    /**
     * @brief method which updates text and shows window
     */
    void showWindow();
    /**
     * @brief method which updates text
     * @param index          the current combobox index
     */
    void updateTabs(const int index);

private:
    /**
     * @brief show debug messages
     */
    bool debug;
    /**
     * @brief contains information about enabled modules
     */
    bool modules[3];
    /**
     * @brief class user interface
     */
    Ui::Reportabug *ui;
    // modules
    /**
     * @brief GitHub module
     */
    GithubModule *github;
    /**
     * @brief GitReports module
     */
    GitreportModule *gitreport;
    /**
     * @brief method which creates the window actions
     */
    void createActions();
    /**
     * @brief method which adds a text into combobox
     */
    void createComboBox();
    /**
     * @brief method which parses combobox content
     * @param index          the current combobox index
     * @return -1 if there is no any selected modules
     * @return 0 if the GitHub module is selected
     * @return 1 if the GitReports module is selected
     */
    int getNumberByIndex(const int index);
    /**
     * @brief method which initializates modules
     */
    void initModules();
    /**
     * @brief method which monitors on pressed keys
     * @param pressedKey     the current pressed key
     */
    void keyPressEvent(QKeyEvent *pressedKey);
    /**
     * @brief method which parses string and replace known tags
     * @param line           source line
     * @return replaced line
     */
    QString parseString(QString line);
};


#endif /* REPORTABUG_H */
