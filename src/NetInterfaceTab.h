// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetInterfaceTab.h
 * clip-config net interface config tab widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETINTERFACETAB_H
#define NETINTERFACETAB_H

#include "NetTab.h"

class QRadioButton;
class QButtonGroup;
class QCheckBox;
class QSpinBox;
class QLabel;

class NetInterfaceTab : public NetTab
{
  Q_OBJECT
 
 public:
  NetInterfaceTab(QWidget *parent, ClipNetProfile *profile, int ifs);

 public slots:
  bool saveCurrentProfile();
  bool loadCurrentProfile();
  void showAdvanced(bool on);
  void initProfile();

 private slots:
  void interfaceChanged(int newtype);
  void interfaceSet(int newtype);
  void downloadLocked(bool on);

 private:
  bool hasNone;
  bool hasWired;
  bool hasWifi;
  bool hasUmts;
  bool loading;

  QButtonGroup *interfaces;

  QRadioButton *btnWired;
  QRadioButton *btnWifi;
  QRadioButton *btnUmts;
  QRadioButton *btnNone;

  QLabel    *lblMTU;
  QSpinBox  *spbMTU;

  QLabel    *lblDownload;
  QCheckBox *chkDownload;

 signals:
  void netInterfaceChanged(int newtype); // Changed by user or new profile
  void netInterfaceSet(int newtype); // Changed explicitly by user
};

#endif // NETINTERFACETAB_H

// vi:sw=2:ts=2:et:co=80:
