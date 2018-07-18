// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetTab.h
 * clip-config net config tab widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETTAB_H
#define NETTAB_H

#include <QFrame>

class ClipNetProfile;

class NetTab : public QFrame
{
  Q_OBJECT
 
 public:
  NetTab(QWidget *parent = NULL, ClipNetProfile *profile = NULL);
  virtual ~NetTab() { };

 public slots:
  virtual bool changesPending();
  virtual bool saveCurrentProfile() = 0;
  virtual bool loadCurrentProfile() = 0;

  virtual void showAdvanced(bool on);
  virtual void enable(bool on) { };
  virtual void initProfile() { };
  virtual void setInterface(int) { };

  void setProfile(ClipNetProfile *prof);
 
 protected:
  bool pending;
  bool advanced;
  ClipNetProfile *profile;
};

#endif // NETTAB_H

// vi:sw=2:ts=2:et:co=80:
