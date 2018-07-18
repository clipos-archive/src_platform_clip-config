// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetGwTab.cpp
 * clip-config jail gateways config tab widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010-2014 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QCheckBox>
#include <QLabel>
#include <QLayout>

#include "AddrsField.h"
#include "NetGwTab.h"
#include "ClipNetProfile.h"
#include "InheritSection.h"
#include "consts.h"

NetGwTab::NetGwTab(QWidget *parent, ClipNetProfile *profile, 
                                      bool has_rmh, bool has_rmb)
  : NetTab(parent, profile)
{
  sectionGw = new InheritSection("Passerelles", CLIP_IPSEC, this);

  QVBoxLayout *vb;
  QHBoxLayout *hb = new QHBoxLayout();
  hb->setSpacing(4);
  hb->addSpacing(20);

  vb = new QVBoxLayout;
  update = new AddrsField(this, "<b>Passerelles UPDATE</b>", false);
  vb->addSpacing(10);
  vb->addWidget(update);
  vb->addStretch(2);

  hb->addLayout(vb);
  hb->addSpacing(10);

  chkAdmin = new QCheckBox("Utiliser une passerelle ADMIN", this);
  connect(chkAdmin, SIGNAL(stateChanged(int)), this, SLOT(enableAdmin(int)));
  vb->addSpacing(10);
  vb->addWidget(chkAdmin);
  admin = new AddrsField(this, "<b>Passerelles ADMIN</b>", false);
  admin->setEnabled(false);
  vb->addSpacing(10);
  vb->addWidget(admin);
  vb->addStretch(2);

  vb = new QVBoxLayout;

  if (has_rmh) {
    rmh = new AddrsField(this, "<b>Passerelles niveau Haut</b>", false);
    vb->addSpacing(10);
    vb->addWidget(rmh);
    vb->addStretch(2);
  } else
    rmh = NULL;

  chkAudit = new QCheckBox("Utiliser une passerelle AUDIT", this);
  connect(chkAudit, SIGNAL(stateChanged(int)), this, SLOT(enableAudit(int)));
  vb->addSpacing(10);
  vb->addWidget(chkAudit);
  audit = new AddrsField(this, "<b>Passerelles AUDIT</b>", false);
  audit->setEnabled(false);
  vb->addSpacing(10);
  vb->addWidget(audit);
  vb->addStretch(2);

  hb->addLayout(vb);
  hb->addSpacing(10);

  QVBoxLayout *gws = new QVBoxLayout;
  gws->addSpacing(10);
  gws->addLayout(hb);

  if (has_rmb) {
    vb = new QVBoxLayout;
    vb->addSpacing(10);

    hb = new QHBoxLayout;
    hb->addSpacing(5);
    hb->addStretch(5);

    chkRMB = new QCheckBox("Utiliser une passerelle niveau Bas", this);
    connect(chkRMB, SIGNAL(stateChanged(int)), this, SLOT(enableRMB(int)));
    hb->addWidget(chkRMB);
    hb->addSpacing(5);
    hb->addStretch(5);

    vb->addLayout(hb);
    vb->addSpacing(10);

    hb = new QHBoxLayout;
    hb->addSpacing(5);
    hb->addStretch(2);

    rmbGw = new AddrsField(this, "<b>Passerelles niveau Bas</b>", false);
    rmbGw->setEnabled(false);
    hb->addWidget(rmbGw);
    hb->addSpacing(10);

    rmbNet = new AddrsField(this, "<b>Réseaux chiffrés niveau Bas</b>");
    rmbNet->setEnabled(false);
    hb->addWidget(rmbNet);
    hb->addSpacing(5);
    hb->addStretch(2);

    vb->addLayout(hb);
    vb->addSpacing(10);

    gws->addSpacing(10);
    gws->addLayout(vb);
  } else {
    chkRMB = NULL;
    rmbGw = NULL;
    rmbNet = NULL;
  }

  gws->addSpacing(10);
  gws->addStretch(3);

  sectionGw->setChildrenLayout(gws);

  QVBoxLayout *sections = new QVBoxLayout(this);
  sections->addWidget(sectionGw);

  connect(sectionGw, SIGNAL(toggled(bool, int)),
      parent, SLOT(inheritSectionToggled(bool, int)));
}


bool 
NetGwTab::saveCurrentProfile()
{
  if (rmh)
    profile->setNameVal(CLIP_IPSEC, "RMH_GW", rmh->returnValues());
  profile->setNameVal(CLIP_IPSEC, "UPDATE_GW", update->returnValues());

  if (chkAdmin->isChecked())
    profile->setNameVal(CLIP_IPSEC, "ADMIN_GW", admin->returnValues());
  else 
    profile->setNameVal(CLIP_IPSEC, "ADMIN_GW", "");

  if (chkAudit->isChecked())
    profile->setNameVal(CLIP_IPSEC, "AUDIT_GW", audit->returnValues());
  else 
    profile->setNameVal(CLIP_IPSEC, "AUDIT_GW", "");

  if (chkRMB) {
    if (chkRMB->isChecked()) {
      profile->setNameVal(CLIP_IPSEC, "RMB_GW", rmbGw->returnValues());
      profile->setNameVal(CLIP_IPSEC, "RMB_NETWORKS", rmbNet->returnValues());
    } else {
      profile->setNameVal(CLIP_IPSEC, "RMB_GW", "");
      profile->setNameVal(CLIP_IPSEC, "RMB_NETWORKS",  "");
    }
  }

  return true;
}

bool
NetGwTab::loadCurrentProfile()
{
  if (rmh)
    rmh->setValue(profile->getNameVal(CLIP_IPSEC, "RMH_GW", multiip_rx));
  update->setValue(profile->getNameVal(CLIP_IPSEC, "UPDATE_GW", multiip_rx));

  QString tmp;
  
  tmp = profile->getNameVal(CLIP_IPSEC, "ADMIN_GW", multiip_rx);
  chkAdmin->setChecked(!tmp.isEmpty());
  admin->setValue(tmp);

  tmp = profile->getNameVal(CLIP_IPSEC, "AUDIT_GW", multiip_rx);
  chkAudit->setChecked(!tmp.isEmpty());
  audit->setValue(tmp);

  if (chkRMB) {
    tmp = profile->getNameVal(CLIP_IPSEC, "RMB_GW", multiip_rx);
    chkRMB->setChecked(!tmp.isEmpty());
    rmbGw->setValue(tmp);
    if (!tmp.isEmpty())
      rmbNet->setValue(profile->getNameVal(CLIP_IPSEC, 
            "RMB_NETWORKS", multiaddrs_rx));
    else 
      rmbNet->setValue(QString());
  }

  sectionGw->update(profile, true);

  return true;
}

void
NetGwTab::enableAdmin(int state)
{
  admin->setEnabled((state == Qt::Checked));
}

void
NetGwTab::enableAudit(int state)
{
  audit->setEnabled((state == Qt::Checked));
}

void
NetGwTab::enableRMB(int state)
{
  if (chkRMB) {
    rmbGw->setEnabled((state == Qt::Checked));
    rmbNet->setEnabled((state == Qt::Checked));
  }
}


// vi:sw=2:ts=2:et:co=80:
