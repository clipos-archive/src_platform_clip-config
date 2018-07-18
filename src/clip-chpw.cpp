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
#include <QMessageBox>
#include <QFile>
#include <QTextCodec>
#include <QTranslator>
#include <stdlib.h>
#include "qtcommon.h"
#include "consts.h"

#include "frmUsersChpw.h"

int g_with_rmh = 0;
int g_with_rmb = 0;

bool hasRMH = false;
bool hasRMB = false;

int 
main(int argc, char** argv) 
{
  QApplication app(argc, argv);

  if (QFile::exists(rmhFlagFile)) {
    hasRMH = true;
    g_with_rmh = 1;
  }

  if (QFile::exists(rmbFlagFile)) {
    hasRMB = true;
    g_with_rmb = 1;
  }

  QTranslator qt(0);
  qt.load ("qt_fr.qm", PREFIX"/share/qt4/translations");
  app.installTranslator(&qt);
    
  QTextCodec *codec = QTextCodec::codecForName("utf8");
  QTextCodec::setCodecForCStrings(codec);

  frmUsersChpw *window = new frmUsersChpw(0);
  window->show();
  
  exit(app.exec());
}

// vi:sw=2:ts=2:et:co=80:
