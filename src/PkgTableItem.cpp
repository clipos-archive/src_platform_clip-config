// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PkgTableItem.cpp
 * clip-config single item in pkg table.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010-2012 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "PkgTableItem.h"
#include <QLabel>
#include <QLayout>
#include <QCheckBox>

PkgTableItem::PkgTableItem(QWidget *parent, const QString &txt, bool b)
  : QWidget(), txt(txt), changed(false)
{
  QHBoxLayout *h = new QHBoxLayout(this);
  h->setSpacing(6);
  l = new QLabel(parent);
  cb = new QCheckBox(parent);
  cb->setChecked(b);
  l->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  if (b)
    l->setText(QString("<b><font color=blue>") 
        + txt + "</font></b>");
  else
    l->setText(QString("<b>") + txt + "</b>");
  connect(cb, SIGNAL(toggled(bool)), this, SLOT(toggle(bool)));
  
  h->addWidget(cb, 0, Qt::AlignCenter|Qt::AlignVCenter);
  h->addSpacing(8);
  h->addWidget(l, 1, Qt::AlignLeft|Qt::AlignVCenter);
}

PkgTableItem::~PkgTableItem(void)
{
  delete l;
  delete cb;
}

void PkgTableItem::toggle(bool b)
{
  changed = !changed;
  if (b)
    l->setText(QString("<b><font color=blue>") 
        + txt + "</font></b>");
  else
    l->setText(QString("<b>") + txt + "</b>");
}

bool PkgTableItem::set(void)
{
  return cb->isChecked();
}

// vi:sw=2:ts=2:et:co=80:
