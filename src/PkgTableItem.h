// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PkgTableItem.h
 * clip-config single item in pkg table.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef _PKG_TABLE_ITEM_H
#define _PKG_TABLE_ITEM_H

#include <QWidget>

class QString;
class QCheckBox;
class QLabel;

class PkgTableItem : public QWidget
{
  Q_OBJECT
 public:
  PkgTableItem(QWidget *p, const QString &txt, bool b);
  ~PkgTableItem();
  bool pending() const { return changed; };
  bool set(void);
  void saved(void) { changed = false; };
  QString getPkgName(void) const { return txt; }

 public slots:
  void toggle(bool b);

 private:
  QLabel *l;
  QCheckBox *cb;
  QString txt;
  bool changed;
};

#endif

// vi:sw=2:ts=2:et:co=80:
