// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file qtcommon.cpp
 * clip-config common functions.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "qtcommon.h"

#include <QApplication>
#include <QMessageBox>

void 
fatalError(const QString& err) 
{
  QMessageBox::critical(0, "Erreur interne", err,
			QMessageBox::Ok | QMessageBox::Default, 0, 0);
  QApplication::exit(1);
}

QString
jailName(const QString& jail)
{
  if (jail == "clip")
    return QString("socle CLIP");
  else if (jail == "rm_h")
    return QString("niveau Haut");
  else if (jail == "rm_b")
    return QString("niveau Bas");
  else
    return jail.toUpper();
}


// vi:sw=2:ts=2:et:co=80:
