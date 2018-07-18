// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmDevicesMain.cpp
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2011-2014 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>

#include <ClipConfig/ClipConfigFile.h>

#include "frmDevicesMain.h"
#include "qtcommon.h"
#include "consts.h"

using namespace std;


#define DEVICE_NONE   0x01
#define DEVICE_CLIP   0x02
#define DEVICE_RMH    0x04
#define DEVICE_RMB    0x08

#define two_rb_layout(label, pix, rb1, rb2) do {\
  vb->addWidget(new QLabel("<b>"label" :</b>", this)); \
  hb = new QHBoxLayout; \
  hb->setSpacing(6); \
  hb->addSpacing(10); \
  lb = new QLabel(this); \
  lb->setPixmap(QPixmap(ICONPATH pix)); \
  hb->addWidget(lb); \
  hb->addSpacing(10); \
  hb->addWidget(rb1); \
  hb->addSpacing(10); \
  hb->addWidget(rb2); \
  hb->addStretch(2); \
  vb->addSpacing(5); \
  vb->addLayout(hb); \
  line = new QFrame(this); \
  line->setFrameShape(QFrame::HLine); \
  line->setFrameShadow(QFrame::Sunken); \
  vb->addSpacing(5); \
  vb->addWidget(line); \
  vb->addSpacing(10); \
} while (0)

#define three_rb_layout_noline(label, pix, rb1, rb2, rb3) do {\
  vb->addWidget(new QLabel("<b>"label" :</b>", this)); \
  hb = new QHBoxLayout; \
  hb->setSpacing(6); \
  hb->addSpacing(10); \
  lb = new QLabel(this); \
  lb->setPixmap(QPixmap(ICONPATH pix)); \
  hb->addWidget(lb); \
  hb->addSpacing(10); \
  hb->addWidget(rb1); \
  hb->addSpacing(10); \
  hb->addWidget(rb2); \
  hb->addSpacing(10); \
  hb->addWidget(rb3); \
  hb->addStretch(2); \
  vb->addSpacing(5); \
  vb->addLayout(hb); \
} while (0)

#define add_layout_line() do {\
  line = new QFrame(this); \
  line->setFrameShape(QFrame::HLine); \
  line->setFrameShadow(QFrame::Sunken); \
  vb->addSpacing(5); \
  vb->addWidget(line); \
  vb->addSpacing(10); \
} while (0)

#define three_rb_layout(label, pix, rb1, rb2, rb3) do {\
  three_rb_layout_noline(label, pix, rb1, rb2, rb3); \
  add_layout_line(); \
} while (0)
  

const QString deviceRequest("/usr/bin/clip-device-request");

frmDevicesMain::frmDevicesMain()
  : QDialog(0)
{
  setWindowTitle("Attribution des périphériques");
  setWindowIcon(QIcon(ICONPATH"preferences-desktop-peripherals.png"));

  devicesFile = new ClipConfigFile();
  devicesFile->readFile(devicesPath);
  smartcardsFile = new ClipConfigFile();
  smartcardsFile->readFile(smartcardsPath);

  btnSave = new QPushButton(QIcon(ICONPATH"document-save.png"), "Sauver", this);
  btnQuit = new QPushButton(QIcon(ICONPATH"dialog-close.png"), "Quitter", this);

  connect(btnSave, SIGNAL(clicked()), this, SLOT(save()));
  connect(btnQuit, SIGNAL(clicked()), this, SLOT(quit()));

  QTabWidget *tabs = new QTabWidget(this);
  QVBoxLayout *vb;
  QHBoxLayout *hb;
  QLabel *lb;
  QFrame *line;

  /***********************************************************/
  /*                      Devices tab                        */
  /***********************************************************/
  QFrame *devicesFrame = new QFrame(this);

  grpSound = new QButtonGroup();
  grpSound->setExclusive(true);
  btnSoundRMH = new QRadioButton("Niveau Haut", devicesFrame);
  btnSoundRMH->setToolTip("Attribuer la carte son au <b>niveau haut</b>");
  grpSound->addButton(btnSoundRMH, DEVICE_RMH);
  btnSoundRMB = new QRadioButton("Niveau Bas", devicesFrame);
  btnSoundRMB->setToolTip("Attribuer la carte son au <b>niveau bas</b>");
  grpSound->addButton(btnSoundRMB, DEVICE_RMB);
  btnSoundNone = new QRadioButton("Aucun", devicesFrame);
  btnSoundNone->setToolTip("N'attribuer la carte son à aucun niveau");
  grpSound->addButton(btnSoundNone, DEVICE_NONE);

  btnSoundCapture = new QCheckBox("Activer le micro", devicesFrame);
  btnSoundCapture->setToolTip("Permet aux applications d'accéder au micro");

  grpPrint = new QButtonGroup();
  grpPrint->setExclusive(true);
  btnPrintRMH = new QRadioButton("Niveau Haut", devicesFrame);
  btnPrintRMH->setToolTip("Attribuer l'imprimante USB au <b>niveau haut</b> "
                          "lors de son branchement");
  grpPrint->addButton(btnPrintRMH, DEVICE_RMH);
  btnPrintRMB = new QRadioButton("Niveau Bas", devicesFrame);
  btnPrintRMB->setToolTip("Attribuer l'imprimante USB au <b>niveau bas</b> "
                          "lors de son branchement");
  grpPrint->addButton(btnPrintRMB, DEVICE_RMB);

  grpScann = new QButtonGroup();
  grpScann->setExclusive(true);
  btnScannRMH = new QRadioButton("Niveau Haut", devicesFrame);
  btnScannRMH->setToolTip("Attribuer le scanner USB au <b>niveau haut</b> "
                          "lors de son branchement");
  grpScann->addButton(btnScannRMH, DEVICE_RMH);
  btnScannRMB = new QRadioButton("Niveau Bas", devicesFrame);
  btnScannRMB->setToolTip("Attribuer le scanner USB au <b>niveau bas</b> "
                          "lors de son branchement");
  grpScann->addButton(btnScannRMB, DEVICE_RMB);

  grpCamera = new QButtonGroup();
  grpCamera->setExclusive(true);
  btnCamRMH = new QRadioButton("Niveau Haut", devicesFrame);
  btnCamRMH->setToolTip("Attribuer la WebCam au <b>niveau haut</b> "
                          "lors de son branchement");
  grpCamera->addButton(btnCamRMH, DEVICE_RMH);
  btnCamRMB = new QRadioButton("Niveau Bas", devicesFrame);
  btnCamRMB->setToolTip("Attribuer la WebCam au <b>niveau bas</b> "
                          "lors de son branchement");
  grpCamera->addButton(btnCamRMB, DEVICE_RMB);
  btnCamNone = new QRadioButton("Aucun", devicesFrame);
  btnCamNone->setToolTip("N'attribuer la WebCam à aucun niveau");
  grpCamera->addButton(btnCamNone, DEVICE_NONE);


  if (!hasRMH) {
    btnSoundRMH->setEnabled(false);
    btnPrintRMH->setEnabled(false);
    btnScannRMH->setEnabled(false);
    btnCamRMH->setEnabled(false);
  }
  if (!hasRMB) {
    btnSoundRMB->setEnabled(false);
    btnPrintRMB->setEnabled(false);
    btnScannRMB->setEnabled(false);
    btnCamRMB->setEnabled(false);
  }

  vb = new QVBoxLayout(devicesFrame);
  vb->setSpacing(3);
  vb->addSpacing(5);

  three_rb_layout_noline("Attribution de la carte son <i>(immédiat)</i>", 
                  "speaker32.png", btnSoundRMH, btnSoundRMB, btnSoundNone);

  hb = new QHBoxLayout;
  hb->setSpacing(6);
  hb->addSpacing(60);
  hb->addWidget(btnSoundCapture);
  hb->addStretch(5);
  vb->addSpacing(5);
  vb->addLayout(hb);

  add_layout_line();

  two_rb_layout("Attribution de l'imprimante USB <i>(prochain branchement)</i>",
                  "printer32.png", btnPrintRMH, btnPrintRMB);
  two_rb_layout("Attribution du scanner <i>(prochain branchement)</i>", 
                  "scanner32.png", btnScannRMH, btnScannRMB);
  three_rb_layout("Attribution de la webcam <i>(prochain branchement)</i>", 
                  "webcam32.png", btnCamRMH, btnCamRMB, btnCamNone);

  tabs->addTab(devicesFrame, 
                  QIcon(ICONPATH"preferences-desktop-peripherals.png"),
                  "Périphériques d'entrée/sortie");

  /***********************************************************/
  /*                      Smartcards tab                     */
  /***********************************************************/

  QFrame *smartcardsFrame = new QFrame(this);

  btnPkcs11Core = new QCheckBox("Socle", smartcardsFrame);
  btnPkcs11Core->setToolTip("Exposer la carte à puce "
                                "dans le <b>socle CLIP</b>");
  btnPkcs11RMH = new QCheckBox("Niveau Haut", smartcardsFrame);
  btnPkcs11RMH->setToolTip("Exposer la carte à puce au <b>niveau haut</b>\n");
  btnPkcs11RMB = new QCheckBox("Niveau Bas", smartcardsFrame);
  btnPkcs11RMB->setToolTip("Exposer la carte à puce au <b>niveau bas</b>\n");

  btnPgpRMH = new QCheckBox("Niveau Haut", smartcardsFrame);
  btnPgpRMH->setToolTip("Exposer la carte OpenPGP au "
                          "<b>niveau haut</b>\n");
  btnPgpRMB = new QCheckBox("Niveau Bas", smartcardsFrame);
  btnPgpRMB->setToolTip("Exposer la carte OpenPGP au "
                          "<b>niveau bas</b>\n");

  if (!hasRMH) {
    btnPkcs11RMH->setEnabled(false);
    btnPgpRMH->setEnabled(false);
  }
  if (!hasRMB) {
    btnPkcs11RMB->setEnabled(false);
    btnPgpRMB->setEnabled(false);
  }

  vb = new QVBoxLayout(smartcardsFrame);
  vb->setSpacing(3);
  vb->addSpacing(5);

  three_rb_layout("Exposition de la carte à puce <i>(prochain démarrage)</i>", 
                  "secure-card.png", btnPkcs11RMH, btnPkcs11RMB, btnPkcs11Core);
  two_rb_layout("Exposition de la carte OpenPGP <i>(prochain démarrage)</i>",
                  "secure-card.png", btnPgpRMH, btnPgpRMB);
  vb->addSpacing(5);
  vb->addStretch(5);

  tabs->addTab(smartcardsFrame, QIcon(ICONPATH"secure-card.png"), "Cartes à puce");

  /***********************************************************/
  /*                      Global layout                      */
  /***********************************************************/

  vb = new QVBoxLayout(this);
  vb->addSpacing(5);
  vb->addWidget(tabs);
  vb->addSpacing(10);

  hb = new QHBoxLayout;
  hb->setSpacing(6);
  hb->addStretch(3);
  hb->addWidget(btnSave);
  hb->addSpacing(20);
  hb->addWidget(btnQuit);
  hb->addStretch(3);
  vb->addLayout(hb);
  vb->addSpacing(5);

  load();
}

void 
frmDevicesMain::update(bool write)
{
  // Devices

  QString sound;
  char sndlevel = 'n';

  switch (grpSound->checkedId()) {
    case DEVICE_RMH:
      sound = "rm_h";
      sndlevel = 'h';
      break;
    case DEVICE_RMB:
      sound = "rm_b";
      sndlevel = 'b';
      break;
    case DEVICE_NONE:
      sound = "none";
      sndlevel = 'n';
      break;
  }

  QString printer = (grpPrint->checkedId() == DEVICE_RMH) ?  "rm_h" : "rm_b";
  QString scanner = (grpScann->checkedId() == DEVICE_RMH) ?  "rm_h" : "rm_b";
  
  QString camera;
  switch (grpCamera->checkedId()) {
    case DEVICE_RMH:
      camera = "rm_h";
      break;
    case DEVICE_RMB:
      camera = "rm_b";
      break;
    case DEVICE_NONE:
      camera = "none";
      break;
  }
  
  QString capture("no");
  if (btnSoundCapture->isChecked())
    capture = "yes";

  devicesFile->setNameVal("SOUNDCARD0_JAIL", sound);
  devicesFile->setNameVal("SOUND_CAPTURE", capture);
  devicesFile->setNameVal("PRINTER0_JAIL", printer);
  devicesFile->setNameVal("SCANNER_JAIL", scanner);
  devicesFile->setNameVal("VIDEO0_JAIL", camera);

  // Smartcards
  QStringList pkcs11;
  QStringList pgp;

  if (btnPkcs11Core->isChecked())
    pkcs11 << "core";
  if (btnPkcs11RMH->isChecked())
    pkcs11 << "rm_h";
  if (btnPkcs11RMB->isChecked())
    pkcs11 << "rm_b";

  if (btnPgpRMH->isChecked())
    pgp << "rm_h";
  if (btnPgpRMB->isChecked())
    pgp << "rm_b";

  smartcardsFile->setNameVal("PKCS11_JAILS", pkcs11.join(" "));
  smartcardsFile->setNameVal("OPENPGP_JAILS", pgp.join(" "));

  if (!write)
    return;

  devicesFile->writeFile();
  smartcardsFile->writeFile();

  if (sndlevel != lastSndLevel 
          || captureEnabled != btnSoundCapture->isChecked())
    if (switchSound(sndlevel)) {
      lastSndLevel = sndlevel;
      captureEnabled = btnSoundCapture->isChecked();
    }
}

void 
frmDevicesMain::quit(void)
{
  update(false);
  if (!devicesFile->pendingChanges() && !smartcardsFile->pendingChanges()) {
    QApplication::exit(0);
    return;
  }

  int choice = QMessageBox::question(this, 
      "Modifications non sauvées",
      "Certaines modifications n'ont pas été sauvegardées.\n"
      "Souhaitez-vous les sauver avant de quitter ?",
      "Sauver", "Ne pas sauver", "Annuler");
  if (choice == 2) // Cancel
    return;
  if (choice == 0) {
    if (!save()) {
      int abort = QMessageBox::question(this,
        "Erreur de sauvegarde",
        "La sauvegarde des modifications a échoué. "
        "Souhaitez-vous quitter malgré tout ?",
        "Quitter", "Ne pas quitter");
      if (abort)
        return;
    }
  }
  QApplication::exit(0);
}

bool
frmDevicesMain::save(void)
{
  update(true);
  return true;
}

void
frmDevicesMain::load(void)
{ 
  // Devices
  
  QString sound = devicesFile->getNameVal("SOUNDCARD0_JAIL", any_rx);
  QString capture = devicesFile->getNameVal("SOUND_CAPTURE", yes_rx);
  QString printer = devicesFile->getNameVal("PRINTER0_JAIL", any_rx);
  QString scanner = devicesFile->getNameVal("SCANNER_JAIL", any_rx);
  QString camera = devicesFile->getNameVal("VIDEO0_JAIL", any_rx);

  if (sound == "rm_h") {
    btnSoundRMH->setChecked(true);
    lastSndLevel = 'h';
  } else if (sound == "rm_b") {
    btnSoundRMB->setChecked(true);
    lastSndLevel = 'b';
  } else if (sound == "none" || sound == "prompt") {
    // sound == "prompt" for backward compat 
    btnSoundNone->setChecked(true);
    lastSndLevel = 'n';
  }

  btnSoundCapture->setChecked(!capture.isEmpty());
  captureEnabled = !capture.isEmpty();

  if (printer == "rm_h") 
    btnPrintRMH->setChecked(true);
  else if (printer == "rm_b") 
    btnPrintRMB->setChecked(true);

  if (scanner == "rm_h") 
    btnScannRMH->setChecked(true);
  else if (scanner == "rm_b") 
    btnScannRMB->setChecked(true);

  if (camera == "rm_h") 
    btnCamRMH->setChecked(true);
  else if (camera == "rm_b") 
    btnCamRMB->setChecked(true);
  else
    btnCamNone->setChecked(true);

  // Smartcards

  QString pkcs11 = smartcardsFile->getNameVal("PKCS11_JAILS", any_rx);
  QString pgp = smartcardsFile->getNameVal("OPENPGP_JAILS", any_rx);

  if (pkcs11.contains("core")) {
    btnPkcs11Core->setChecked(true);
  } 
  if (pkcs11.contains("rm_h")) {
    btnPkcs11RMH->setChecked(true);
  } 
  if (pkcs11.contains("rm_b")) {
    btnPkcs11RMB->setChecked(true);
  } 

  if (pgp.contains("rm_h")) {
    btnPgpRMH->setChecked(true);
  } 
  if (pgp.contains("rm_b")) {
    btnPgpRMB->setChecked(true);
  } 
}

bool
frmDevicesMain::switchSound(char l)
{
  QStringList args;
  QString level("Aucun");

  args << "-s" << QChar(l);
  // Should not take long
  int ret = QProcess::execute(deviceRequest, args);
  if (ret) {
    QMessageBox::warning(0, "Commutation du son impossible",
        QString("La commande d'affectation de la carte son a échoué "
                  "(code de retour %1).").arg(ret),
          QMessageBox::Ok | QMessageBox::Default, 0, 0);
    return false;
  }

  switch (l) {
    case 'h':
      level = "Haut";
      break;
    case 'b':
      level = "Bas";
      break;
  }
    
  QMessageBox::information(0, "Commutation du son",
           QString("La carte son est désormais affectée au niveau : %1, "
                   "micro %2.").arg(level) .arg( btnSoundCapture->isChecked() ? 
                                  "activé" : "désactivé"),
           QMessageBox::Ok | QMessageBox::Default, 0, 0);

  return true;
}

// vi:sw=2:ts=2:et:co=80:
