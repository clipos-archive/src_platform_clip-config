// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetInterfaceTab.cpp
 * clip-config net interface config tab widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QFile>

#include "NetInterfaceTab.h"
#include "ClipNetProfile.h"
#include "consts.h"

static const char *strDownload = "Verrouiller les téléchargements :";

static void 
getMac(QString &str, const QString &path) 
{
  QFile f(path);
  if (f.open(QIODevice::ReadOnly)) {
    QString mac(f.readAll());
    if (!mac.isEmpty()) {
      // newline...
      str += QString("  (adresse MAC %1)").arg(mac.trimmed());
    }
    f.close();
  }
}

NetInterfaceTab::NetInterfaceTab(QWidget *parent, 
                  ClipNetProfile *profile, int ifs)
  : NetTab(parent, profile)
{ 
  loading = true;
  interfaces = new QButtonGroup();
  interfaces->setExclusive(true);

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(4);
  vb->addSpacing(10);

  if (ifs & I_NONE) {
    btnNone = new QRadioButton("Pas d'accès réseau", this);
    interfaces->addButton(btnNone, I_NONE);
    vb->addWidget(btnNone);
    vb->addSpacing(10);
    hasNone = true;
  } else {
    btnNone = NULL;
    hasNone = false;
  }

  if (ifs & I_WIRED) {
    QString lblWired("Utiliser l'interface filaire");
    getMac(lblWired, wiredMacFile);
    btnWired = new QRadioButton(lblWired, this);
    interfaces->addButton(btnWired, I_WIRED);
    vb->addWidget(btnWired);
    vb->addSpacing(10);
    hasWired = true;
  } else {
    btnWired = NULL;
    hasWired = false;
  }

  if (ifs & I_WIFI) {
    QString lblWifi("Utiliser l'interface wifi");
    getMac(lblWifi, HASWIFI_FLAG);
    btnWifi = new QRadioButton(lblWifi, this);
    interfaces->addButton(btnWifi, I_WIFI);
    vb->addWidget(btnWifi);
    vb->addSpacing(10);
    hasWifi = true;
  } else {
    btnWifi = NULL;
    hasWifi = false;
  }

  if (ifs & I_UMTS) {
    btnUmts = new QRadioButton("Utiliser un accès téléphonique 3G", this);
    interfaces->addButton(btnUmts, I_UMTS);
    vb->addWidget(btnUmts);
    vb->addSpacing(10);
    hasUmts = true;
  } else {
    btnUmts = NULL;
    hasUmts = false;
  }

  // Advanced options
  lblMTU = new QLabel("MTU de l'interface :", this);
  lblMTU->setEnabled(false);

  spbMTU = new QSpinBox(this);
  spbMTU->setRange(1, 1500);
  spbMTU->setSingleStep(20);
  spbMTU->setValue(1500);
  spbMTU->setEnabled(false);
  spbMTU->setToolTip("Ajustement manuel de la MTU de l'interface\n"
                    "(option avancée, uniquement utile si le réseau\n"
                    "ne permet pas la détection automatique de MTU)");
  lblMTU->setToolTip(spbMTU->toolTip());

  lblDownload = new QLabel(strDownload, this);
  lblDownload->setEnabled(false);

  chkDownload = new QCheckBox(this);
  chkDownload->setChecked(false);
  chkDownload->setEnabled(false);
  chkDownload->setToolTip("Interdire les téléchargements de mises à\n"
                          "jour lorsque ce profil est actif (limite\n"
                          "la bande passante consommée)");
  lblDownload->setToolTip(chkDownload->toolTip());

  QHBoxLayout *hb = new QHBoxLayout();
  hb->addStretch(6);
  hb->addWidget(lblDownload);
  hb->addSpacing(10);
  hb->addWidget(chkDownload);
  hb->addSpacing(30);
  hb->addWidget(lblMTU);
  hb->addSpacing(10);
  hb->addWidget(spbMTU);
  hb->addSpacing(10);
  vb->addStretch(1);
  vb->addLayout(hb);
  vb->addStretch(5);

  connect(interfaces, SIGNAL(buttonClicked(int)), 
            this, SIGNAL(netInterfaceChanged(int)));
  connect(interfaces, SIGNAL(buttonClicked(int)), 
            this, SIGNAL(netInterfaceSet(int)));

  connect(this, SIGNAL(netInterfaceChanged(int)), 
            this, SLOT(interfaceChanged(int)));
  connect(this, SIGNAL(netInterfaceSet(int)), 
            this, SLOT(interfaceSet(int)));

  connect(chkDownload, SIGNAL(toggled(bool)), 
            this, SLOT(downloadLocked(bool)));

  if (hasWired)
    btnWired->setChecked(true);
  else if (hasWifi)
    btnWifi->setChecked(true);
  else if (hasNone)
    btnNone->setChecked(true);
  loading = false;
}

bool
NetInterfaceTab::saveCurrentProfile()
{
  bool ret = true;
  profile->setNameVal(CLIP_NET, "ETH0_MTU", spbMTU->text());
  profile->setNameVal(CLIP_NET, "DOWNLOAD_LOCKED", 
              (chkDownload->isChecked()) ? yes_str : no_str);

  switch (interfaces->checkedId()) {
    case I_NONE:
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_ENABLED", "");
      profile->setNameVal(CLIP_UMTS, "UMTS_ENABLED", "");
      profile->setNameVal(CLIP_NET, "NO_NETWORK", yes_str);
      break;
    case I_WIRED:
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_ENABLED", "");
      profile->setNameVal(CLIP_UMTS, "UMTS_ENABLED", "");
      profile->setNameVal(CLIP_NET, "NO_NETWORK", "");
      break;
    case I_WIFI:
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_ENABLED", yes_str);
      profile->setNameVal(CLIP_UMTS, "UMTS_ENABLED", "");
      profile->setNameVal(CLIP_NET, "NO_NETWORK", "");
      break;
    case I_UMTS:
      profile->setNameVal(CLIP_UMTS, "UMTS_ENABLED", yes_str);
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_ENABLED", "");
      profile->setNameVal(CLIP_NET, "NO_NETWORK", "");
      break;
    default:
      qDebug("Invalid interface type: %d", interfaces->checkedId());
      ret = false;
      break;
  }
  return ret;
}

bool
NetInterfaceTab::loadCurrentProfile()
{
  bool umts = false, wifi = false, none = false;
  bool supported = true;

  none = !profile->getNameVal(CLIP_NET, 
                        "NO_NETWORK", yes_rx).isEmpty(); 

  if (!none) {
    umts = !profile->getNameVal(CLIP_UMTS, 
                        "UMTS_ENABLED", yes_rx).isEmpty(); 
    wifi = !profile->getNameVal(CLIP_WIRELESS, 
                        "WIRELESS_ENABLED", yes_rx).isEmpty(); 
  }
  bool wired = !(none || umts || wifi);

  if (hasNone)
    btnNone->setChecked(none);

  if (hasWired)
    btnWired->setChecked(wired);
  else
    if (wired)
      supported = false;

  if (hasWifi)
    btnWifi->setChecked(wifi);
  else
    if (wifi)
      supported = false;
  
  if (hasUmts)
    btnUmts->setChecked(umts);
  else
    if (umts)
      supported = false;

  int _mtu = profile->getNameVal(CLIP_NET, "ETH0_MTU", mtu_rx).toInt();
  if (_mtu)
    spbMTU->setValue(_mtu);

  if (!supported) {
    QString ifname = (wired) ? "filaire" : (wifi) ? "wifi" : "3G";

    if (hasNone) {
      btnNone->setChecked(true);
      none = true;
    }

    QMessageBox::warning(0, "Interface absente",
      QString("Le profil %1 utilise une interface (interface %2)\n"
      "qui n'est pas présente actuellement sur le système.\n"
      "Il a été temporairement transformé en profil hors-ligne,\n"
      "mais les modifications n'ont pas été sauvegardées")
      .arg(profile->getProfileName()).arg(ifname),
      QMessageBox::Ok);
  }

  bool lock = !profile->getNameVal(CLIP_NET, 
                            "DOWNLOAD_LOCKED", yes_rx).isEmpty();
  lock = (lock || none || umts);

  chkDownload->setChecked(lock);
  downloadLocked(lock);

  emit(netInterfaceChanged(interfaces->checkedId()));

  return supported;
}

void
NetInterfaceTab::showAdvanced(bool on)
{
  advanced = on;

  if (on) { 
    if (interfaces->checkedId() != I_NONE) {
      lblMTU->setEnabled(true);
      spbMTU->setEnabled(true);
    }
    if (!(interfaces->checkedId() & I_NODOWNLOAD)) {
      lblDownload->setEnabled(true);
      chkDownload->setEnabled(true);
    }
  } else {
    lblDownload->setEnabled(false);
    chkDownload->setEnabled(false);
    lblMTU->setEnabled(false);
    spbMTU->setEnabled(false);
  }
}

void
NetInterfaceTab::initProfile(void)
{
  int type = interfaces->checkedId();
  if (!(type & I_NODOWNLOAD) && chkDownload->isChecked()) {
    chkDownload->setChecked(false);
    QMessageBox::information(0, "Déverrouillage des téléchargements",
      "Les téléchargements de mises à jour ont été\n"
      "automatiquement déverrouillés pour le nouveau\n"
      "profil.\n"
      "Si vous souhaitez maintenir le verrouillage des\n"
      "téléchargements pour ce profil, il sera nécessaire\n"
      "de cocher à nouveau l'option explicitement.",
      QMessageBox::Ok);
  }
}

void
NetInterfaceTab::interfaceChanged(int type)
{
  if (advanced) {
    lblDownload->setEnabled(!(type & I_NODOWNLOAD));
    chkDownload->setEnabled(!(type & I_NODOWNLOAD));
    lblMTU->setEnabled(type != I_NONE);
    spbMTU->setEnabled(type != I_NONE);
  }
}

void
NetInterfaceTab::interfaceSet(int type)
{
  if (type == I_UMTS && !chkDownload->isChecked()) {
    QMessageBox::information(0, "Verrouillage des téléchargements", 
          "Les téléchargements de mises à jour sont automatiquement\n"
          "verrouillés pour les profils utilisant l'interface 3G.\n",
          QMessageBox::Ok);
  }
  if ((type & I_NODOWNLOAD))
    chkDownload->setChecked(true);
  else {
    if (chkDownload->isChecked()) {
      chkDownload->setChecked(false);
      QMessageBox::information(0, "Déverrouillage des téléchargements",
          "Les téléchargements de mises à jour ont été\n"
          "automatiquement déverrouillés lors du changement\n"
          "d'interface.\n"
          "Si vous souhaitez maintenir le verrouillage des\n"
          "téléchargements pour ce profil, il sera nécessaire\n"
          "de cocher à nouveau l'option explicitement.",
          QMessageBox::Ok);
    }
  }
}

void
NetInterfaceTab::downloadLocked(bool on)
{
  QString lbl;
  if (on) {
    lbl = QString("<font color='red'>%1</font>").arg(strDownload);
  } else {
    lbl = strDownload;
  }
  lblDownload->setText(lbl);
}

// vi:sw=2:ts=2:et:co=80:
