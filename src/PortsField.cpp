// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PortsField.cpp
 * clip-config multiple ports field.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "PortsField.h"
#include <ClipConfig/CCException.h>
#include "consts.h"

#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QLayout>
#include <QValidator>
#include <QPushButton>


PortsField::PortsField(QWidget* parent, const QString& lbl)
  : MultiField(parent, lbl, port_rx, ",", 100, 90)
{ 
  valueToAdd->setToolTip("Saisissez un port, ou un segment de ports\n"
                         "(<début>:<fin>), puis cliquez sur :\n"
                         "+ pour ajouter à la liste\n"
                         " - pour supprimer de la liste");
}

static inline bool 
get_min_max(const QString& s, unsigned short& min, unsigned short& max)
{
  int pos = s.indexOf(":");
  bool ok;

  if (pos == -1) {
    min = max = s.toUShort (&ok);
    return ok;
  } else {
    if (pos > 0) {
      min = s.mid(0, pos).toUShort(&ok);
      if (!ok)
        return false;
    } else 
      min = 1;
    if (pos != s.length() -1) {
      max = s.mid(pos+1).toUShort(&ok);
      if (!ok)
        return false;
    } else
      max = 65535;
    return (min <= max);
  }
}

void 
PortsField::addValue(const QString& p)
{
  if (!regexp.exactMatch(p))
    return;

  QListWidgetItem *it;
  int firstToDel = -1 , lastToDel = -2, where = 0, n;
  unsigned short min, max, curMin, curMax = 0;
  if (!get_min_max(p, min, max))
    return;
  
  for (it = values->item(0), n = 0; it != 0; n++, it = values->item(n)) {
    // TODO: Catch somewhere, even if this should NEVER happen !!!!
    if (!get_min_max(it->text(), curMin, curMax))
      throw CCException("PortsField::addPort: inconsistency detected !!!");

    if (firstToDel > lastToDel) {
      if (curMax < min - 1)
        continue;
      if (curMin > max + 1) {
        where = n;
        break;
      }
      firstToDel = where = lastToDel = n;
      if (curMin < min)
        min = curMin;
      if (curMax > max)
        max = curMax;
    } else {
      // Merging has begun, so "where" is set
      if (curMin > max + 1)
        break;
      if (curMax > max) 
        max = curMax;
      lastToDel = n;
    }
  }

  if (!it)
    where = n;

  for (n = lastToDel; n >= firstToDel; n--)
    delete values->item(n);
  
  if (min == max)
    values->insertItem(where, QString::number(min));
  else
    values->insertItem(where, QString::number(min) 
                              + ':' + QString::number(max));

  values->setCurrentRow(n);
  modified = true;
}

// vi:sw=2:ts=2:et:co=80:
