// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PkgTable.h
 * clip-config package table.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef _PKG_TABLE_H
#define _PKG_TABLE_H

#include <QLayout>
#include <QLinkedList>

class QString;
class PkgTableItem;

class PkgTable : public QWidget
{
public:
  PkgTable(QWidget *parent, const QString &jail, 
                      const QString &out, const QString &in);
  bool save(void);
  bool pending(void);
  bool empty(void) { return (nrows == 0); };

private:
  QGridLayout *gl;
  QString outpath;
  QString inpath;
  QString jail;
  int nrows;
  QLinkedList<PkgTableItem *> pkgs;

private slots:
  bool runHelper();
  bool readList(const QString &plist);

public slots:
  void init();
};

#endif

// vi:sw=2:ts=2:et:co=80:
