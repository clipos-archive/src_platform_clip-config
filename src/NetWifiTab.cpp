// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetWifiTab.cpp
 * clip-config net wireless config tab widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QCheckBox>
#include <QComboBox>
#include <QButtonGroup>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QRadioButton>
#include <QRegExpValidator>

#include "qtcommon.h"
#include "consts.h"
#include "NetWifiTab.h"
#include <ClipWidgets/LabelledField.h>
#include "ClipNetProfile.h"
#include "NetDaemonThread.h"
#include <ClipWidgets/WaitDialog.h>
#include "frmWifiSelect.h"


NetWifiTab::NetWifiTab(QWidget *parent, ClipNetProfile *profile)
  : NetTab(parent, profile)
{ 
  edtEssid = new QLineEdit("", this);
  edtEssid->setValidator(new QRegExpValidator(any_rx, this));

  edtWepKey = new QLineEdit("", this);
  edtWepKey->setValidator(new QRegExpValidator(wepkey_rx, this));
  edtWepKey->setEchoMode(QLineEdit::Password);

  edtWpaKey = new QLineEdit("", this);
  edtWpaKey->setValidator(new QRegExpValidator(wpakey_rx, this));
  edtWpaKey->setEchoMode(QLineEdit::Password);
  activeCol = edtWpaKey->palette().color(QPalette::Active, QPalette::Base);
  inactiveCol = edtWpaKey->palette().color(QPalette::Inactive, QPalette::Base);

  mode = new QButtonGroup(this);
  mode->setExclusive(true);

  btnModeManaged = new QRadioButton("Managed (par défaut)", this);
  btnModeManaged->setToolTip("Mode Managed : cas habituel des bornes Wifi");
  mode->addButton(btnModeManaged, W_MODE_MANAGED);
  btnModeAdHoc = new QRadioButton("Ad-Hoc (cas particulier)", this);
  btnModeAdHoc->setToolTip("Mode Ad-Hoc : cas spécifique, par ex. réseau\n"
                           "Wifi créé par un téléphone portable");
  mode->addButton(btnModeAdHoc, W_MODE_ADHOC);

  encryption = new QButtonGroup(this);
  encryption->setExclusive(true);

  btnNone = new QRadioButton("Aucun", this);
  btnNone->setToolTip("Aucun chiffrement des flux (non recommandé)");
  encryption->addButton(btnNone, W_NOENCRYPTION);
  btnWep = new QRadioButton("WEP", this);
  btnWep->setToolTip("Chiffrement (faible) des flux (non recommandé)");
  encryption->addButton(btnWep, W_WEP);
  btnWpa = new QRadioButton("WPA", this);
  btnWpa->setToolTip("Chiffrement des flux (recommandé)");
  encryption->addButton(btnWpa, W_WPA);

  chkWepKey = new QCheckBox("Afficher la clé saisie", this);
  chkWpaKey = new QCheckBox("Afficher la clé saisie", this);

  cmbWpaKeyMgmt = new QComboBox(this);
  cmbWpaKeyMgmt->insertItem(W_WPA_PSK, wpamgmt_psk_str);
  cmbWpaKeyMgmt->insertItem(W_WPA_EAP, wpamgmt_eap_str);
  cmbWpaKeyMgmt->setToolTip("Mode d'authentification WPA :\n"
                            " - WPA_PSK (mot de passe)\n"
                            " - WPA_EAP (certificat client)");

  btnScan = new QPushButton(QIcon(ICONPATH"network-wireless.png"), 
                         "Choisir un identifiant parmi "
                         "les réseaux actifs", this);
  btnScan->setToolTip("Lancer une analyse pour lister les réseaux\n"
                      "Wifi disponible et leurs caractéristiques");

  lblWepKey = new QLabel("Clé :", this);
  lblWpaKeyMgmt = new QLabel("Méthode d'authentification", this);
  lblWpaKey = new QLabel("Clé / Mot de passe :", this);

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(4);
  vb->addSpacing(10);
  
  vb->addWidget(btnScan);
  vb->addSpacing(10);

  QHBoxLayout *hb;

#define init_hb(sp) do { \
  hb = new QHBoxLayout; \
  hb->setSpacing(4); \
  hb->addSpacing(sp); \
} while (0)

#define finish_hb(sp) do { \
  vb->addLayout(hb); \
  if (sp) \
    vb->addSpacing(sp); \
} while (0)

  init_hb(20);
  hb->addWidget(new QLabel("ESSID", this));
  hb->addSpacing(5);
  hb->addWidget(edtEssid);
  finish_hb(5);
 
  init_hb(20);
  hb->addWidget(new QLabel("Mode: ", this));
  hb->addSpacing(5);
  hb->addWidget(btnModeManaged);
  hb->addSpacing(5);
  hb->addWidget(btnModeAdHoc);
  finish_hb(5);

  init_hb(20);
  hb->addWidget(new QLabel("Méthode de chiffrement", this));
  finish_hb(5);

  init_hb(35);
  hb->addWidget(btnNone);
  finish_hb(15);

  init_hb(35);
  hb->addWidget(btnWep);
  finish_hb(5);

  init_hb(50);
  hb->addWidget(lblWepKey);
  hb->addSpacing(5);
  hb->addWidget(edtWepKey);
  finish_hb(5);
 
  init_hb(90);
  hb->addWidget(chkWepKey);
  finish_hb(15);

  init_hb(35);
  hb->addWidget(btnWpa);
  finish_hb(5);

  init_hb(50);
  hb->addWidget(lblWpaKeyMgmt);
  hb->addStretch(5);
  hb->addWidget(cmbWpaKeyMgmt);
  finish_hb(0);

  init_hb(50);
  hb->addWidget(lblWpaKey);
  hb->addSpacing(5);
  hb->addWidget(edtWpaKey);
  finish_hb(5);

  init_hb(190);
  hb->addWidget(chkWpaKey);
  finish_hb(5);

#undef init_hb
#undef finish_hb

  connect(btnScan, SIGNAL(clicked()), this, SLOT(scan()));
  
  connect(encryption, SIGNAL(buttonClicked(int)), 
                                this, SLOT(updateEncryption(int)));
  
  connect(chkWepKey, SIGNAL(toggled(bool)), this, SLOT(showWepKey(bool)));
  connect(chkWpaKey, SIGNAL(toggled(bool)), this, SLOT(showWpaKey(bool)));
  
  connect(cmbWpaKeyMgmt, SIGNAL(activated(int)), 
                                          this, SLOT(setWpaKeyMgmt(int)));

  connect(edtWpaKey, SIGNAL(textChanged(const QString&)),
                                              this, SLOT(checkWpaKey()));

  btnNone->setChecked(true);
  updateEncryption(encryption->checkedId());
}

/*************************************************************/
/*                      Public slots                         */
/*************************************************************/

void
NetWifiTab::enable(bool on)
{
  if (on)
    updateEncryption(encryption->checkedId());
}

bool
NetWifiTab::saveCurrentProfile()
{
  bool ret = true;
  if (edtEssid->text().isEmpty())
    return ret;

  profile->setNameVal(CLIP_WIRELESS, "WIRELESS_ESSID", edtEssid->text());

  switch (mode->checkedId()) {
    case W_MODE_MANAGED:
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_MODE", managed_str);
      break;
    case W_MODE_ADHOC:
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_MODE", adhoc_str);
      break;
    default:
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_MODE", managed_str);
      break;
  }

  QString key;
  const char *vname, *ovname;
  switch (encryption->checkedId()) {
    case W_NOENCRYPTION:
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_ENCRYPTION", "");
      break;
    case W_WEP:
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_ENCRYPTION", wep_str);
      key = edtWepKey->text();
      if (key.isEmpty()) {
        QMessageBox::warning(0, "Clé WEP vide", 
          "Vous n'avez pas défini de clé WEP.\n"
          "La connexion wifi ne pourra pas être établie.");
          profile->setNameVal(CLIP_WIRELESS, "WIRELESS_WEPKEY", "");
          ret = false;
       }
      if (!wepkey_hexakey_rx.exactMatch(key))
        key = wepkey_passphrase_prefix_str + key;
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_WEPKEY", key);
      break;
    case W_WPA:
      profile->setNameVal(CLIP_WIRELESS, "WIRELESS_ENCRYPTION", wpa_str);
      profile->setNameVal(CLIP_WIRELESS, 
                         "WPA_KEY_MGMT", cmbWpaKeyMgmt->currentText());

      if (cmbWpaKeyMgmt->currentIndex() == W_WPA_EAP) {
        vname = "WPA_PASSWORD";
        ovname = "WPA_PSK";
      } else {
        vname = "WPA_PSK";
        ovname = "WPA_PASSWORD";
      }
      if (edtWpaKey->hasAcceptableInput())
        profile->setNameVal(CLIP_WIRELESS, vname, edtWpaKey->text());
      else {
        QMessageBox::warning(0, "Clé WPA vide ou incorrecte", 
          "Vous n'avez pas défini une clé WPA correcte.\n"
          "La connexion wifi ne pourra pas être établie.");
        profile->setNameVal(CLIP_WIRELESS, vname, "");
        ret = false;
      }
      profile->setNameVal(CLIP_WIRELESS, ovname, "");
      break;
    default:
      qDebug("Unsupported encryption type: %d", encryption->checkedId());
      ret = false;
      break;
  }
      
  return ret;
}

bool
NetWifiTab::loadCurrentProfile()
{
  int enc;
  QString wepkey, keymgmt;
  QString essid = profile->getNameVal(CLIP_WIRELESS, "WIRELESS_ESSID", any_rx);
  if (essid.isEmpty())
    return true;

  edtEssid->setText(essid);

  chkWepKey->setChecked(false);
  edtWepKey->clear();
  chkWpaKey->setChecked(false);
  edtWpaKey->clear();

  if (profile->getNameVal(CLIP_WIRELESS, "WIRELESS_MODE", adhoc_rx).isEmpty())
    btnModeManaged->setChecked(true);
  else
    btnModeAdHoc->setChecked(true);

  if (!profile->getNameVal(CLIP_WIRELESS, 
                              "WIRELESS_ENCRYPTION", wep_rx).isEmpty())
    enc = W_WEP;
  else if (!profile->getNameVal(CLIP_WIRELESS, 
                              "WIRELESS_ENCRYPTION", wpa_rx).isEmpty())
    enc = W_WPA;
  else
    enc = W_NOENCRYPTION;

  switch (enc) {
    case W_NOENCRYPTION:
      btnNone->setChecked(true);
      break;
    case W_WEP:
      wepkey = profile->getNameVal(CLIP_WIRELESS, 
                                    "WIRELESS_WEPKEY", wepkey_hexakey_rx);
      if (wepkey.isEmpty()) {
        wepkey = profile->getNameVal(CLIP_WIRELESS, 
                                  "WIRELESS_WEPKEY", wepkey_passphrase_rx);
        if (wepkey.startsWith(wepkey_passphrase_prefix_str))
          wepkey = wepkey.mid(2);
        else
          wepkey = QString();
      }
      edtWepKey->setText(wepkey);
      btnWep->setChecked(true);
      break;
    case W_WPA:
      keymgmt = profile->getNameVal(CLIP_WIRELESS, 
                                  "WPA_KEY_MGMT", wpamgmt_rx);
      if (!keymgmt.isEmpty() && (keymgmt == wpamgmt_eap_str)) {
        cmbWpaKeyMgmt->setCurrentIndex(W_WPA_EAP);
        edtWpaKey->setText(profile->getNameVal(CLIP_WIRELESS, 
                                  "WPA_PASSWORD", any_rx));
        } else {
          cmbWpaKeyMgmt->setCurrentIndex(W_WPA_PSK);
          setWpaKeyMgmt(W_WPA_PSK);
          edtWpaKey->setText(profile->getNameVal(CLIP_WIRELESS, "WPA_PSK", any_rx));
        }
      btnWpa->setChecked(true);
      break;
  }

  updateEncryption(enc);
  return true;
}

/*************************************************************/
/*                      Private slots                        */
/*************************************************************/

void 
NetWifiTab::scan()
{
  bool finished = false;
  char c;
  QString profileName = profile->getProfileName();
  NetDaemonThread t('L', profileName, c, finished);
  WaitDialog dialog("Opération en cours", 
                "Veuillez patienter pendant l'analyse des réseaux actifs...", 
                finished);

  t.start ();
  dialog.exec ();
 
  if (c != 'Y') {
    scan_warning(c);
    return;
  }

  frmWifiSelect lister;
  lister.exec();

  QString essid = lister.getEssid ();
  wifi_enc_t type = lister.getType ();
  wifi_mode_t mode = lister.getMode ();

  if (essid.isEmpty ())
    return;

  if (essid == "??")
    essid = "";

  edtEssid->setText(essid);
  edtWepKey->clear();
  edtWpaKey->clear();

  switch (mode) {
    case WifiModeManaged:
      btnModeManaged->setChecked(true);
      break;
    case WifiModeAdHoc:
      btnModeAdHoc->setChecked(true);
      break;
    default:
      qDebug("Unsupported wifi mode %d", mode);
      break;
  }

  switch (type) {
    case WifiEncNone:
      btnNone->setChecked(true);
      break;
    case WifiEncWEP:
      btnWep->setChecked(true);
      QMessageBox::information (0, "Saisie de clé WEP",
        "Il est nécessaire de saisir manuellement la clé WEP "
        "associée à cette connexion.",
        QMessageBox::Ok | QMessageBox::Default, 0, 0);
      break;
    case WifiEncWPA:
      btnWpa->setChecked(true);
      QMessageBox::information (0, "Saisie de clé WPA",
        "Il est nécessaire de saisir manuellement la clé WPA "
        "associée à cette connexion.",
        QMessageBox::Ok | QMessageBox::Default, 0, 0);
      checkWpaKey();
      break;
  }
  updateEncryption(encryption->checkedId());
}

void 
NetWifiTab::updateEncryption(int index)
{
  lblWepKey->setEnabled(index == W_WEP);
  edtWepKey->setEnabled(index == W_WEP);
  chkWepKey->setEnabled(index == W_WEP);

  lblWpaKey->setEnabled(index == W_WPA);
  lblWpaKeyMgmt->setEnabled(index == W_WPA);
  edtWpaKey->setEnabled(index == W_WPA);
  chkWpaKey->setEnabled(index == W_WPA);
}

void 
NetWifiTab::showWepKey(bool on)
{
  edtWepKey->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
}

void 
NetWifiTab::showWpaKey(bool on)
{
  edtWpaKey->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
}

void 
NetWifiTab::checkWpaKey()
{
  if (edtWpaKey->hasAcceptableInput()) {
    QPalette pal = edtWpaKey->palette();
    pal.setColor(QPalette::Active, QPalette::Base, activeCol);
    pal.setColor(QPalette::Inactive, QPalette::Base, inactiveCol);
    edtWpaKey->setPalette(pal);
  } else {
    QPalette pal = edtWpaKey->palette();
    pal.setColor(QPalette::Active, QPalette::Base, Qt::red);
    pal.setColor(QPalette::Inactive, QPalette::Base, Qt::red);
    edtWpaKey->setPalette(pal);
  }
}

void 
NetWifiTab::setWpaKeyMgmt(int index)
{
  switch (index) {
    case W_WPA_PSK:
      edtWpaKey->setValidator(new QRegExpValidator(wpakey_rx, this));
      break;
    case W_WPA_EAP:
      edtWpaKey->setValidator(new QRegExpValidator(any_rx, this));
      break;
    default:
      qDebug("Unsupported wpa key management: %d", index);
      break;
  }

  checkWpaKey();
}

// vi:sw=2:ts=2:et:co=80:
