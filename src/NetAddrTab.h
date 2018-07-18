// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetAddrTab.h
 * clip-config net core addresses config tab widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETADDRTAB_H
#define NETADDRTAB_H

#include "NetTab.h"

class QRadioButton;
class QButtonGroup;
class QSpinBox;
class QLabel;
class IPField;
class RoutesField;
class HostsWidget;
class InheritSection;

class NetAddrTab : public NetTab
{
  Q_OBJECT
 
 public:
  NetAddrTab(QWidget *parent, ClipNetProfile *profile, int numIfs);

 public slots:
  bool saveCurrentProfile();
  bool loadCurrentProfile();
  void showAdvanced(bool on);
  void setDhcp(bool on);
  void setInterface(int type);

 private slots:
  void dhcpToggled(int num);

 private:
  int numIfs;
  QButtonGroup *staticOrDhcp;

  QRadioButton *btnStatic;
  QRadioButton *btnDhcp;

  IPField *addr;
  IPField *addr1;
  IPField *addr2;
  IPField *gateway;
  RoutesField *extraRoutes;

  HostsWidget *hostsWidget;
  InheritSection *sectionHosts;

 signals:
  void dhcpOn(bool on);
};

#endif // NETADDRTAB_H

// vi:sw=2:ts=2:et:co=80:
