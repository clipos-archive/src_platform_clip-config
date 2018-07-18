// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetFilterTab.h
 * clip-config net filter config tab widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETFILTERTAB_H
#define NETFILTERTAB_H

#include <QLinkedList>
#include "NetTab.h"

class PortsWidget;
class AddrsField;
class YesNoField;
class QGroupBox;
class InheritSection;

class NetFilterTab : public NetTab
{
  Q_OBJECT
 
 public:
  NetFilterTab(QWidget *parent, ClipNetProfile *profile, 
          const QString &lbl, const QString &prefix, int protos = 0);

  NetFilterTab(QWidget *parent, ClipNetProfile *profile, 
          const QString &lbl, const QString &prefix,
          const QString &lblClients, const QString &clientsVar, 
          int protos = 0);

 public slots:
  bool saveCurrentProfile();
  bool loadCurrentProfile();

  void showAdvanced(bool on);

 private:
  QString prefix;
  QString clientsVar;

  PortsWidget *ports;
  AddrsField  *clients;
  YesNoField  *dynamicIPs;

  QGroupBox *protoBox;
  QLinkedList<YesNoField *>protoFields;

  InheritSection *sectionFilter;
};

#endif // NETFILTERTAB_H

// vi:sw=2:ts=2:et:co=80:
