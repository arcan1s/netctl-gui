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

#ifndef CONFIG_H
#define CONFIG_H

#include <inconfig.h>


/*
 * Main configuration
 */
// the owner of the source repository
#define OWNER "arcan1s"
// project name
#define PROJECT "netctl-gui"
// tags defaults
#define TAG_TITLE "A new bug"
#define TAG_BODY "Some error occurs"
// the following tags will work only if user has push access
// if they will be empty, they will be ignored
#define TAG_ASSIGNEE "$OWNER"
#define TAG_MILESTONE ""
// comma separated
#define TAG_LABELS ""

/*
 * Configuration of creating an issue using GitHub API
 */
// combobox text
#define GITHUB_COMBOBOX "I want to report a bug using my GitHub account"
// issues url; in the most cases do not touch it
// available tags are $PROJECT, $OWNER
#define ISSUES_URL "https://api.github.com/repos/$OWNER/$PROJECT/issues"

/*
 * Configuration of creating an issue using GitHub API and own token
 *
 * This module will be used instead of GitReport module.
 * To create a token please visit https://github.com/settings/applications
 * and generate new one. Needed scopes are public_repo
 * (or repo if you will use it for a private repository).
 * Please keep in mind that passing the token in the clear,
 * you may discredit your account.
 */

/*
 * Configuration of creating an issue using GitReports
 *
 * Please, visit https://gitreports.com/
 * and set up it for your repository.
 */
// combobox text
#define GITREPORT_COMBOBOX "GitHub? I don't understand what do you want from me!"
// public link; in the most cases do not touch it
// available tags are $PROJECT, $OWNER
#define PUBLIC_URL "https://gitreports.com/issue/$OWNER/$PROJECT"
// captcha url; in the most cases do not touch it
#define CAPTCHA_URL "https://gitreports.com/simple_captcha?code="


#endif /* CONFIG_H */
