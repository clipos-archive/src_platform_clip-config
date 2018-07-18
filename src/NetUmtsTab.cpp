// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetUmtsTab.cpp
 * clip-config net UMTS config tab widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QLineEdit>
#include <QCheckBox>
#include <QLayout>
#include <QLabel>
#include <QRegExpValidator>

#include "NetUmtsTab.h"
#include "ClipNetProfile.h"
#include <ClipWidgets/LabelledField.h>
#include "consts.h"


NetUmtsTab::NetUmtsTab(QWidget *parent, ClipNetProfile *profile)
  : NetTab(parent, profile)
{ 
  edtPin = new QLineEdit("", this);
  edtPin->setEchoMode(QLineEdit::Password);
  edtPin->setValidator(new QRegExpValidator(sim_pin_rx, this));

  edtApn = new QLineEdit("", this);
  edtApn->setValidator(new QRegExpValidator(any_rx, this));

  edtIdentity = new QLineEdit("", this);
  edtIdentity->setValidator(new QRegExpValidator(any_rx, this));

  edtPassword = new QLineEdit("", this);
  edtPassword->setEchoMode(QLineEdit::Password);
  edtPassword->setValidator(new QRegExpValidator(any_rx, this));

  chkPin = new QCheckBox("Afficher le code PIN saisi", this);
  chkPassword = new QCheckBox("Afficher le mot de passe saisi", this);

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(4);
  vb->addSpacing(10);

  QGridLayout *gl = new QGridLayout;
  gl->setHorizontalSpacing(6);
  gl->setVerticalSpacing(6);
  gl->setColumnMinimumWidth(0, 20);
  gl->setColumnMinimumWidth(3, 20);
  gl->addWidget(new QLabel("Code PIN :", this), 0, 1);
  gl->addWidget(edtPin, 0, 2);
  gl->addWidget(chkPin, 1, 2);
  gl->addWidget(new QLabel("Point d'accès :", this), 2, 1);
  gl->addWidget(edtApn, 2, 2);
  gl->addWidget(new QLabel("Identifiant abonné :", this), 3, 1);
  gl->addWidget(edtIdentity, 3, 2);
  gl->addWidget(new QLabel("Mot de passe :", this), 4, 1);
  gl->addWidget(edtPassword, 4, 2);
  gl->addWidget(chkPassword, 5, 2);

  vb->addLayout(gl);
  vb->addStretch(5);

  connect(chkPin, SIGNAL(toggled(bool)), this, SLOT(showPin(bool)));
  connect(chkPassword, SIGNAL(toggled(bool)), this, SLOT(showPassword(bool)));
}

void
NetUmtsTab::showPin(bool on)
{
  edtPin->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
}

void
NetUmtsTab::showPassword(bool on)
{
  edtPassword->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
}

bool
NetUmtsTab::saveCurrentProfile()
{
  profile->setNameVal(CLIP_UMTS, "UMTS_PIN", edtPin->text());
  profile->setNameVal(CLIP_UMTS, "UMTS_APN", edtApn->text());
  profile->setNameVal(CLIP_UMTS, "UMTS_IDENTITY", edtIdentity->text());
  profile->setNameVal(CLIP_UMTS, "UMTS_PASSWORD", edtPassword->text());

  return true;
}

bool
NetUmtsTab::loadCurrentProfile()
{
  edtPin->setText(profile->getNameVal(CLIP_UMTS, "UMTS_PIN", sim_pin_rx));
  edtApn->setText(profile->getNameVal(CLIP_UMTS, "UMTS_APN", any_rx));
  edtIdentity->setText(profile->getNameVal(CLIP_UMTS, "UMTS_IDENTITY", any_rx));
  edtPassword->setText(profile->getNameVal(CLIP_UMTS, "UMTS_PASSWORD", any_rx));

  return true;
}


// vi:sw=2:ts=2:et:co=80:
