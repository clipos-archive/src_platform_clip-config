// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file clip-audit-config.cpp
 * clip-audit-config main.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include <QTranslator>

#include <unistd.h>

#include <QtSingleApplication>

#include "frmAuditMain.h"
#include "qtcommon.h"
#include "consts.h"

bool hasRMH = false;
bool hasRMB = false;

int 
main(int argc, char** argv) 
{
  char c;
  QString path = "/etc/audit";
  while ((c = getopt(argc, argv, "bc:h")) != -1) {
    switch (c) {
      case 'b':
        hasRMB = true;
        break;
      case 'c':
        path = optarg;
        break;
      case 'h':
        hasRMH = true;
        break;
    }
  }

  if (QFile::exists(rmhFlagFile)) {
    hasRMH = true;
  }

  if (QFile::exists(rmbFlagFile)) {
    hasRMB = 1;
  }

  QtSingleApplication app("clip-audit-config", argc, argv);
  if (app.sendMessage("Hi there") || app.isRunning())
    return 0;

  QTranslator qt(0);
  qt.load ("qt_fr.qm", PREFIX"/share/qt4/translations");
  app.installTranslator(&qt);
    
  QTextCodec *codec = QTextCodec::codecForName("utf8");
  QTextCodec::setCodecForCStrings(codec);

  // Création et affichage de la fenêtre principale
  QWidget* window = new frmAuditMain(0, path);

  app.setActivationWindow(window);
  QObject::connect(&app, SIGNAL(messageReceived(const QString &)), 
                                  &app, SLOT(activateWindow()));
  window->show();
  exit(app.exec());
}

// vi:sw=2:ts=2:et:co=80:
