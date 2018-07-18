// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetPeerTab.h
 * clip-config peer gateways config tab widget header.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2012 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETPEERTAB_H
#define NETPEERTAB_H


#include "NetTab.h"

class AddrsField;
class QCheckBox;

class NetPeerTab : public NetTab
{
  Q_OBJECT
 
 public:
  NetPeerTab(QWidget *parent, ClipNetProfile *profile);

 public slots:
  bool saveCurrentProfile();
  bool loadCurrentProfile();

 private slots:
  void enablePeer(int);

 private:
  QCheckBox *chkPeer;

  AddrsField *peer;
};

#endif // NETPEERTAB_H

// vi:sw=2:ts=2:et:co=80:
