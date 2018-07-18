// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file clip-config.cpp
 * clip-config main.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include <QTranslator>

#include <QtSingleApplication>

#include <stdlib.h>
#include <unistd.h>

#include "frmNetMain.h"
#include "frmNetConst.h"
#include "frmPkgMain.h"
#include "frmUsersMain.h"
#include "frmDownloadMain.h"
#include "frmDevicesMain.h"
#include "frmDateTime.h"
#include "qtcommon.h"
#include "consts.h"


#define CONFIG_USERS      1
#define CONFIG_NET        2
#define CONFIG_PKGS       3
#define CONFIG_DOWNLOAD   4
#define CONFIG_NETCONST   5
#define CONFIG_DEVICES    6
#define CONFIG_TIME       7

bool hasRMH = false;
bool hasRMB = false;

int g_with_rmh = 0;
int g_with_rmb = 0;

int 
main(int argc, char** argv) 
{
  int mode = CONFIG_NET;
  QString appId = "clip-config-net";
  int numIfs = 0;
  int ifTypes = I_NONE;
  int c;

  while ((c = getopt(argc, argv, "bc:dDhinNs:tuUw")) != -1) {
    switch (c) {
      case 'b':
        hasRMB = true;
        g_with_rmb = 1;
        break;
      case 'c':
        // We define all paths variables, since we may
        // not know yet what mode we will run in.
        netconfdParentDir = optarg;
        netConstPath = optarg;
        devicesPath = optarg;
        ntpPath = optarg;
        dlSourcesPath = optarg;
        break;
      case 'd':
        mode = CONFIG_DOWNLOAD;
        appId = "clip-config-download";
        break;
      case 'D':
        mode = CONFIG_DEVICES;
        appId = "clip-config-devices";
        break;
      case 'h':
        hasRMH = true;
        g_with_rmh = 1;
        break;
      case 'i':
        numIfs += 1;
        break;
      case 'n':
        mode = CONFIG_NET;
        appId = "clip-config-net";
        break;
      case 'N':
        mode = CONFIG_NETCONST;
        appId = "clip-config-netconst";
        break;
      case 's':
        userdSocketPath = optarg;
        netdSocketPath = optarg;
        break;
      case 't':
        mode = CONFIG_TIME;
        appId = "clip-config-time";
        break;
      case 'u':
        mode = CONFIG_USERS;
        appId = "clip-config-users";
        break;
      case 'U':
        mode = CONFIG_PKGS;
        appId = "clip-config-update";
        break;
      case 'w':
        ifTypes |= I_WIFI;
        break;
    }
  }

  if (QFile::exists(rmhFlagFile)) {
    hasRMH = true;
    g_with_rmh = 1;
  }

  if (QFile::exists(rmbFlagFile)) {
    hasRMB = 1;
    g_with_rmb = 1;
  }

  QtSingleApplication app(appId, argc, argv);
  if (app.sendMessage("Hi there") || app.isRunning())
    return 0;

  QTranslator qt(0);
  qt.load ("qt_fr.qm", PREFIX"/share/qt4/translations");
  app.installTranslator(&qt);
    
  QTextCodec *codec = QTextCodec::codecForName("utf8");
  QTextCodec::setCodecForCStrings(codec);

  // Création et affichage de la fenêtre principale
  QWidget* window;

  switch (mode) {
    case CONFIG_USERS:
      window = new frmUsersMain();
      break;
    case CONFIG_NET:
      window = new frmNetMain(numIfs, ifTypes);
      break;
    case CONFIG_PKGS:
      window = new frmPkgMain();
      break;
    case CONFIG_DOWNLOAD:
      window = new frmDownloadMain();
      break;
    case CONFIG_NETCONST:
      window = new frmNetConst();
      break;
    case CONFIG_DEVICES:
      window = new frmDevicesMain();
      break;
    case CONFIG_TIME:
      window = new frmDateTime();
      break;
    default:
      window = new QMessageBox("Erreur sur la ligne de commande",
			     "clip-config doit être appelé avec un argument décrivant\n"
			     "les paramètres que vous souhaitez éditer :\n"
			     "   -u : gestion des utilisateurs\n"
			     "   -n : paramètres réseau\n"
			     "   -U <cage> <destination> : gestion des paquetages optionnels\n",
			     QMessageBox::Critical,
			     QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
  }

  app.setActivationWindow(window);
  QObject::connect(&app, SIGNAL(messageReceived(const QString &)), 
                                  &app, SLOT(activateWindow()));
  window->show();
  exit(app.exec());
}

// vi:sw=2:ts=2:et:co=80:
