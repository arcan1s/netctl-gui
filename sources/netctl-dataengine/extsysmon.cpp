/***************************************************************************
 *   Copyright (C) 2013 by Evgeniy Alekseev <esalekseev@gmail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "extsysmon.h"

#include <Plasma/DataContainer>
#include <QFile>
#include <QProcess>

#include <stdio.h>
#include <stdlib.h>


ExtendedSysMon::ExtendedSysMon(QObject* parent, const QVariantList& args)
  : Plasma::DataEngine(parent, args)
{
  Q_UNUSED(args)

  setMinimumPollingInterval(333);
  readConfiguration();
}

QStringList ExtendedSysMon::sources() const
{
  QStringList source;
  source.append(QString("gpu"));
  source.append(QString("gputemp"));
  source.append(QString("hddtemp"));
  source.append(QString("player"));
  return source;
}

bool ExtendedSysMon::readConfiguration()
{
  // pre-setup
  FILE *f_out;
  f_out = popen("lspci 2> /dev/null", "r");
  char device[256];
  QString dev;
  while (fgets(device, 256, f_out) != NULL)
  {
    dev = QString(device);
  if (dev.toLower().contains("nvidia"))
    gpudev = QString("nvidia");
  else if (dev.toLower().contains("radeon"))
    gpudev = QString("ati");
  }
  pclose(f_out);
  
  f_out = popen("ls -1 /dev/sd[a-z] 2> /dev/null ; ls -1 /dev/hd[a-z] 2> /dev/null", "r");
  while (fgets(device, 256, f_out) != NULL)
  {
    dev = QString(device).split("\n")[0];
    if (dev[0] == '/')
      hdddev.append(dev);
  }
  pclose(f_out);
  
  mpdAddress = QString("localhost");
  mpdPort = QString("6600");
  
  QString fileStr;
  // FIXME: define configuration file
  QString confFileName = QString(getenv("HOME")) + QString("/.kde4/share/config/extsysmon.conf");
  QFile confFile(confFileName);
  bool exists = confFile.open(QIODevice::ReadOnly);
  if (!exists)
  {
    confFileName = QString("/usr/share/config/extsysmon.conf");
    confFile.setFileName(confFileName);
    exists = confFile.open(QIODevice::ReadOnly);
    if (!exists)
      return false;
  }
  
  while (true)
  {
    fileStr = QString(confFile.readLine());
    if (confFile.atEnd())
      break;
    else if (fileStr[0] != '#')
    {
      if (fileStr.split(QString("="), QString::SkipEmptyParts).count() == 2)
      {
        if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("GPUDEV"))
        {
          if (fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0] == QString("ati"))
            gpudev = fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
          else if (fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0] == QString("nvidia"))
            gpudev = fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
          else if (fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0] != QString("auto"))
            gpudev = QString("ignore");
        }
        else if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("HDDDEV"))
        {
          if (fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0] != QString("all"))
          {
            hdddev.clear();
            for (int i=0; i<fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), \
                            QString::SkipEmptyParts)[0].split(QString(","), QString::SkipEmptyParts).count(); i++)
              hdddev.append(fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), \
                            QString::SkipEmptyParts)[0].split(QString(","), QString::SkipEmptyParts)[i]);
          }
        }
        else if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("MPDADDRESS"))
          mpdAddress = fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
        else if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("MPDPORT"))
          mpdPort = fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
      }
    }
  }
  
  confFile.close();
  return true;
}

bool ExtendedSysMon::sourceRequestEvent(const QString &name)
{
  return updateSourceEvent(name);
}

bool ExtendedSysMon::updateSourceEvent(const QString &source)
{
  FILE *f_out;
  QString key, out, tmp_out, value;
  bool ok = false;
  char output[256], val[5];
  if (source == QString("gpu"))
  {
    key = QString("GPU");
    if (gpudev == QString("nvidia"))
    {
      f_out = popen("nvidia-smi -q -d UTILIZATION 2> /dev/null | grep Gpu | tail -n1", "r");
      fgets (output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(" "), QString::SkipEmptyParts).count() < 2))
        value = QString("  N\\A");
      else
      {
        out = QString(output).split(QString(" "), QString::SkipEmptyParts)[2];
        sprintf (val, "%5.1f", out.left(out.count()-2).toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
    }
    else if (gpudev == QString("ati"))
    {
      f_out = popen("aticonfig --od-getclocks 2> /dev/null | grep load | tail -n1", "r");
      fgets (output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(" "), QString::SkipEmptyParts).count() < 3))
        value = QString("  N\\A");
      else
      {
        out = QString(output).split(QString(" "), QString::SkipEmptyParts)[3];
        sprintf (val, "%5.1f", out.left(out.count()-2).toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
    }
    else
    {
      value = QString("  N\\A");
    }
    if (ok == false)
      value = QString("  N\\A");
    value = value.split(QString(","), QString::SkipEmptyParts).join(QString("."));
    setData(source, key, value);
  }
  else if (source == QString("gputemp"))
  {
    key = QString("GPUTemp");
    if (gpudev == QString("nvidia"))
    {
      f_out = popen("nvidia-smi -q -d TEMPERATURE 2> /dev/null | grep Gpu | tail -n1", "r");
      fgets (output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(" "), QString::SkipEmptyParts).count() < 2))
        value = QString(" N\\A");
      else
      {
        out = QString(output).split(QString(" "), QString::SkipEmptyParts)[2];
        sprintf (val, "%4.1f", out.toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
    }
    else if (gpudev == QString("ati"))
    {
      f_out = popen("aticonfig --od-gettemperature 2> /dev/null | grep Temperature | tail -n1", "r");
      fgets (output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(" "), QString::SkipEmptyParts).count() < 4))
        value = QString(" N\\A");
      else
      {
        out = QString(output).split(QString(" "), QString::SkipEmptyParts)[4];
        sprintf (val, "%4.1f", out.toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
    }
    else
    {
      value = QString(" N\\A");
    }
    if (ok == false)
      value = QString(" N\\A");
    value = value.split(QString(","), QString::SkipEmptyParts).join(QString("."));
    setData(source, key, value);
  }
  else if (source == QString("hddtemp"))
  {
    char command[256], *dev;
    QByteArray qb;
    for (int i=0; i<hdddev.count(); i++)
    {
      qb = hdddev[i].toUtf8();
      dev = qb.data();
      sprintf(command, "hddtemp %s 2> /dev/null", dev);
      f_out = popen(command, "r");
      fgets(output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(":"), QString::SkipEmptyParts).count() < 3))
        value = QString(" N\\A");
      else
      {
        out = QString(output).split(QString(":"), QString::SkipEmptyParts)[2];
        sprintf (val, "%4.1f", out.left(out.count()-4).toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
      if (ok == false)
        value = QString(" N\\A");
      value = value.split(QString(","), QString::SkipEmptyParts).join(QString("."));
      setData(source, hdddev[i], value);
    }
  }
  else if (source == QString("player"))
  {
    QProcess player;
    QString qoutput;
    // qmmp
    qoutput = QString("");
    key = QString("qmmp_artist");
    player.start("qmmp --nowplaying %if(%p,%p,Unknown)");
    player.waitForFinished(-1);
    qoutput = player.readAllStandardOutput();
    if (qoutput == QString(""))
      value = QString("N\\A");
    else
      value = qoutput.split(QString("\n"), QString::SkipEmptyParts)[0];
    setData(source, key, value);
    qoutput = QString("");
    key = QString("qmmp_title");
    player.start("qmmp --nowplaying %if(%t,%t,Unknown)");
    player.waitForFinished(-1);
    qoutput = player.readAllStandardOutput();
    if (qoutput == QString(""))
      value = QString("N\\A");
    else
      value = qoutput.split(QString("\n"), QString::SkipEmptyParts)[0];
    setData(source, key, value);
    // amarok
    QString value_artist, qstr;
    qoutput = QString("");
    value = QString("N\\A");
    value_artist = QString("N\\A");
    player.start("qdbus org.kde.amarok /Player GetMetadata");
    player.waitForFinished(-1);
    qoutput = player.readAllStandardOutput();
    for (int i=0; i<qoutput.split(QString("\n"), QString::SkipEmptyParts).count(); i++)
    {
      qstr = qoutput.split(QString("\n"), QString::SkipEmptyParts)[i];
      if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("artist"))
        value_artist = qstr.split(QString(": "), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
      else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("title"))
        value = qstr.split(QString(": "), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
    }
    key = QString("amarok_artist");
    setData(source, key, value_artist);
    key = QString("amarok_title");
    setData(source, key, value);
    // mpd
    value = QString("N\\A");
    value_artist = QString("N\\A");
    char commandStr[512];
    sprintf (commandStr, "echo 'currentsong\nclose' | curl --connect-timeout 1 -fsm 3 telnet://%s:%s 2> /dev/null", \
             mpdAddress.toUtf8().data(), mpdPort.toUtf8().data());
    qoutput = QString("");
    player.start(QString(commandStr));
    player.waitForFinished(-1);
    qoutput = player.readAllStandardOutput();
    for (int i=0; i<qoutput.split(QString("\n"), QString::SkipEmptyParts).count(); i++)
    {
      qstr = qoutput.split(QString("\n"), QString::SkipEmptyParts)[i];
      if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("Artist"))
        value_artist = qstr.split(QString(": "), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
      else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("Title"))
        value = qstr.split(QString(": "), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
    }
    key = QString("mpd_artist");
    setData(source, key, value_artist);
    key = QString("mpd_title");
    setData(source, key, value);
  }

  return true;
}

K_EXPORT_PLASMA_DATAENGINE(extsysmon, ExtendedSysMon)

#include "extsysmon.moc"
