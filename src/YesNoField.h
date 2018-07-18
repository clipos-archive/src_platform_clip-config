// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file YesNoField.h
 * clip-config simple config var yes/no checkbox widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef YESNOFIELD_H
#define YESNOFIELD_H

#include <QCheckBox>
#include "consts.h"

class ClipNetProfile;

class YesNoField : public QCheckBox
{
  Q_OBJECT
 
 public:
  YesNoField(const QString &text, QWidget *parent,
              netfile_t id, const QString &name, bool advanced = false);

 public slots:
  bool saveProfile(ClipNetProfile *);
  bool loadProfile(ClipNetProfile *);

  void showAdvanced(bool on);

 private:
  netfile_t fileId;
  QString varname;
  bool advanced;
};

#endif // YESNOFIELD_H

// vi:sw=2:ts=2:et:co=80:
