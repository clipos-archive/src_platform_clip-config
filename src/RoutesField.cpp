// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file RoutesField.cpp
 * clip-config multiple routes field.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QListWidget>

#include "RoutesField.h"
#include <ClipConfig/CCException.h>
#include "consts.h"

RoutesField::RoutesField(QWidget* parent, const QString& lbl)
  : MultiField(parent, lbl, route_rx, " ", 300, 60, true, 
      QString("Saisir sous la forme <réseau>:<adresse passerelle>"))
{ }

void 
RoutesField::addValue(const QString& a)
{
  if (!regexp.exactMatch(a))
    return;
  QString toAdd = a;

  values->insertItem(0, toAdd.replace(':', "  =>  "));

  values->setCurrentRow(0);
  modified = true;
}

const QString 
RoutesField::returnValues() const
{
  QString res = MultiField::returnValues();
  return res.replace("  =>  ", ":");
}

// vi:sw=2:ts=2:et:co=80:
