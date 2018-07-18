// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetPeerTab.cpp
 * clip-config peer gateways config tab widget.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2012 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QCheckBox>
#include <QLabel>
#include <QLayout>

#include "AddrsField.h"
#include "NetPeerTab.h"
#include "ClipNetProfile.h"
#include "consts.h"

NetPeerTab::NetPeerTab(QWidget *parent, ClipNetProfile *profile)
  : NetTab(parent, profile)
{
  QVBoxLayout *vb = new QVBoxLayout(this);

  chkPeer = new QCheckBox("Etablir un tunnel avec une "
                          "passerelle homologue", this);
  connect(chkPeer, SIGNAL(stateChanged(int)), this, SLOT(enablePeer(int)));
  vb->addSpacing(10);
  vb->addWidget(chkPeer);
  peer = new AddrsField(this, "<b>Passerelles homologues</b>", false);
  peer->setEnabled(false);
  vb->addSpacing(10);
  vb->addWidget(peer);
  vb->addStretch(2);
}


bool 
NetPeerTab::saveCurrentProfile()
{
  if (chkPeer->isChecked())
    profile->setNameVal(CLIP_NET, "PEER_ADDR", peer->returnValues());
  else 
    profile->setNameVal(CLIP_NET, "PEER_ADDR", "");

  return true;
}

bool
NetPeerTab::loadCurrentProfile()
{
  QString tmp = profile->getNameVal(CLIP_NET, "PEER_ADDR", multiip_rx);
  if (!tmp.isEmpty()) {
    chkPeer->setChecked(true);
    peer->setValue(tmp);
  }

  return true;
}

void
NetPeerTab::enablePeer(int state)
{
  peer->setEnabled((state == Qt::Checked));
}


// vi:sw=2:ts=2:et:co=80:
