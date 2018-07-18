// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetGwTab.h
 * clip-config jail gateway config tab widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETGWTAB_H
#define NETGWTAB_H


#include "NetTab.h"

class AddrsField;
class QCheckBox;
class InheritSection;

class NetGwTab : public NetTab
{
  Q_OBJECT
 
 public:
  NetGwTab(QWidget *parent, ClipNetProfile *profile, 
                                  bool has_rmh, bool has_rmb);

 public slots:
  bool saveCurrentProfile();
  bool loadCurrentProfile();

 private slots:
  void enableAdmin(int);
  void enableAudit(int);
  void enableRMB(int);

 private:
  QCheckBox *chkAdmin;
  QCheckBox *chkAudit;
  QCheckBox *chkRMB;

  AddrsField *update;
  AddrsField *rmh;
  AddrsField *admin;
  AddrsField *audit;

  AddrsField *rmbGw;
  AddrsField *rmbNet;

  InheritSection *sectionGw;
};

#endif // NETGWTAB_H

// vi:sw=2:ts=2:et:co=80:
