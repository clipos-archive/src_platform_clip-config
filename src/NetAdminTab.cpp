// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetAdminTab.cpp
 * clip-config admin gateway config tab widget.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2012 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QGridLayout>
#include <QGroupBox>
#include <QLayout>

#include "NetAdminTab.h"
#include "AddrsField.h"
#include "ClipNetProfile.h"
#include "consts.h"

NetAdminTab::NetAdminTab(QWidget *parent, ClipNetProfile *profile, 
                const QString &lblAdmTargets, const QString &admTgtVar,
                const QString &lblAdmSources, const QString &admSrcVar)
  : admSrcVar(admSrcVar), admTgtVar(admTgtVar)
{
  adminTargets = new AddrsField(this, lblAdmTargets);
  adminSources = new AddrsField(this, lblAdmSources);

  QHBoxLayout *lh = new QHBoxLayout(this);
  lh->setSpacing(6);
  lh->addSpacing(10);
  lh->addWidget(adminSources);
  lh->addSpacing(10);
  lh->addWidget(adminTargets);
}

bool 
NetAdminTab::saveCurrentProfile()
{
  profile->setNameVal(CLIP_NET, admTgtVar, adminTargets->returnValues());
  profile->setNameVal(CLIP_NET, admSrcVar, adminSources->returnValues());

  return true;
}

bool 
NetAdminTab::loadCurrentProfile()
{
  adminTargets->setValue(profile->getNameVal(CLIP_NET, 
                                      admTgtVar, multiaddrs_rx));
  adminSources->setValue(profile->getNameVal(CLIP_NET, 
                                      admSrcVar, multiaddrs_rx));

  return true;
}

// vi:sw=2:ts=2:et:co=80:

