// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetAdminTab.h
 * clip-config net filter config tab widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETADMINTAB_H
#define NETADMINTAB_H

#include "NetTab.h"

class AddrsField;

class NetAdminTab : public NetTab
{
  Q_OBJECT
 
 public:
  NetAdminTab(QWidget *parent, ClipNetProfile *profile, 
          const QString &lblAdmTargets, const QString &admTgtVar,
          const QString &lblAdmSources, const QString &admSrcVar);

 public slots:
  bool saveCurrentProfile();
  bool loadCurrentProfile();

 private:
  QString admSrcVar;
  QString admTgtVar;

  AddrsField  *adminTargets;
  AddrsField  *adminSources;
};

#endif // NETADMINTAB_H

// vi:sw=2:ts=2:et:co=80:
