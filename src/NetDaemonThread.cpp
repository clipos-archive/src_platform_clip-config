// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetDaemonThread.cpp
 * clip-config net daemon communication thread.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */


#include <QDir>
#include <QFile>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#include "NetDaemonThread.h"
#include "consts.h"

static const QString NetErrorPath(PREFIX"/var/net_error");

static inline QString
get_activation_errmsg()
{
  if (!QFile::exists(NetErrorPath))
    return "";
  QFile f(NetErrorPath);
  if (!f.open(QIODevice::ReadOnly)) 
    return "";
  QString msg = f.readAll();
  f.close();
  return msg;
}

void 
activation_warning(char c, const QString &pname)
{
  QString msg;
  switch (c) {
      case 'Y':
        return;
      case 'N':
        msg = get_activation_errmsg();
        if (msg.isEmpty()) {
          QMessageBox::warning(0, "Erreur lors de l'activation", 
                    "Le profil " + pname + " n'a pas pu être activé du "
                    "fait d'une erreur réseau indéterminée.",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
        } else {
          QMessageBox::warning(0, "Erreur lors de l'activation", 
                    "Le profil " + pname + " n'a pas pu être activé du "
                    "fait de l'erreur suivante:\n" + msg + ".",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
        }
        break;

      case 'E':
        QMessageBox::warning(0, "Erreur lors de l'activation", 
                    "Le profil " + pname + 
                    " n'a pas pu être activé du fait d'une erreur interne "
                    "du démon netd.",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
        break;

      case 'S':
        QMessageBox::warning(0, "Erreur lors de l'activation", 
                    "Le démon netd n'a pu être contacté.",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
        break;

      case 'P':
        QMessageBox::warning(0, "Erreur lors de l'activation", 
                    "Le lien symbolique n'a pas pu être créé.",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
        break;

      default:
        QMessageBox::warning(0, "Erreur lors de l'activation", 
                    QString("Erreur d'activation inconue (") + c + ").",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
        break;
  }
}

void 
scan_warning(char c)
{
  switch (c) {
    case 'Y':
      break;

    case 'N':
      QMessageBox::warning(0, "Erreur lors de l'analyse wifi", 
                    "L'analyse des réseaux actifs n'a pas pu être "
                    "réalisée du fait d'une erreur réseau.",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
      break;

    case 'E':
      QMessageBox::warning(0, "Erreur lors de l'analyse wifi", 
                    "L'analyse des réseaux actifs a échoué en raison "
                    "d'une erreur interne du démon netd.",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
      break;

    case 'S':
      QMessageBox::warning(0, "Erreur lors de l'analyse wifi", 
                    "Impossible d'analyser les réseaux actifs: le démon "
                    "netd n'a pas pu être contacté.",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
      break;

    default:
      QMessageBox::warning(0, "Erreur lors de l'analyse wifi", 
                    QString("Erreur inconue (") + c + ") lors de "
                    "l'analyse des réseaux actifs.",
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
      break;
 }
}

NetDaemonThread::NetDaemonThread(char action, const QString& profileName, 
                                      char& ptrResult, bool& ptrFinished)
  : action(action), profileName(profileName), ptrResult(ptrResult), 
    ptrFinished(ptrFinished) 
{}


void 
NetDaemonThread::run() 
{
  int s;

  if (action == 'R') { // Reload profile
    ptrResult = 'P';
    if (unlink(NETCONF_LINK))
      qDebug("unlink %s failed: %s", NETCONF_LINK, strerror(errno));

    QString profilePath = netconfdParentDir + QDir::separator() + profileName;
    if (symlink(profilePath.toStdString().c_str(), NETCONF_LINK)) {
      qDebug("symlink %s %s failed: %s", 
              profilePath.toStdString().c_str(), NETCONF_LINK,
              strerror(errno));
      goto RT_end;
    }
  } // Else list wifi.

  struct sockaddr_un sau;
  sau.sun_family = AF_UNIX;
  snprintf(sau.sun_path, sizeof(sau.sun_path), "%s", 
                            netdSocketPath.toStdString().c_str());
      
  ptrResult = 'S';
  s = socket(PF_UNIX, SOCK_STREAM, 0);
  if (s < 0)
    goto RT_end;
  
  if (::connect(s, (struct sockaddr *)&sau, 
              (socklen_t)sizeof(struct sockaddr_un)) < 0)
    goto RT_close;

  if (write(s, (const void *)&action, 1) != 1) {
    ptrResult = 'W';
    goto RT_close;
  }

  if (read(s, &ptrResult, 1) != 1)
    ptrResult = 'S';

  RT_close:
  ::close (s);
    
  RT_end: 
    ptrFinished = true;
}


// vi:sw=2:ts=2:et:co=80:
