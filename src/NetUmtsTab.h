// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetUmtsTab.h
 * clip-config net UMTS config tab widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETUMTSTAB_H
#define NETUMTSTAB_H

#include "NetTab.h"

class QLineEdit;
class QCheckBox;
class QLabel;

class NetUmtsTab : public NetTab
{
  Q_OBJECT
 
 public:
  NetUmtsTab(QWidget *parent, ClipNetProfile *profile);

 public slots:
  bool saveCurrentProfile();
  bool loadCurrentProfile();

 private slots:
  void showPin(bool);
  void showPassword(bool);

 private:
  QLineEdit *edtPin;
  QLineEdit *edtApn;
  QLineEdit *edtIdentity;
  QLineEdit *edtPassword;

  QCheckBox *chkPin;
  QCheckBox *chkPassword;
};

#endif // NETUMTSTAB_H

// vi:sw=2:ts=2:et:co=80:
