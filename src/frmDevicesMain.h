// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmDevicesMain.h
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2011 SGDSN/SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMDEVICESMAIN_H
#define FRMDEVICESMAIN_H


#include <QDialog>

class QPushButton;
class QRadioButton;
class QButtonGroup;
class QCheckBox;

class ClipConfigFile;

class frmDevicesMain : public QDialog
{
  Q_OBJECT

 public:
  frmDevicesMain(void);
 
 private slots:
  void quit(void);
  void update(bool write);
  bool save(void);
  void load(void);

 private:
  bool switchSound(char l);

  char lastSndLevel;
  bool captureEnabled;
  ClipConfigFile *devicesFile;
  ClipConfigFile *smartcardsFile;

  QPushButton *btnQuit;
  QPushButton *btnSave;

  // Devices tab

  QButtonGroup *grpSound;
  QRadioButton *btnSoundRMH;
  QRadioButton *btnSoundRMB;
  QRadioButton *btnSoundNone;
  QCheckBox *btnSoundCapture;

  QButtonGroup *grpPrint;
  QRadioButton *btnPrintRMH;
  QRadioButton *btnPrintRMB;

  QButtonGroup *grpScann;
  QRadioButton *btnScannRMH;
  QRadioButton *btnScannRMB;

  QButtonGroup *grpCamera;
  QRadioButton *btnCamRMH;
  QRadioButton *btnCamRMB;
  QRadioButton *btnCamNone;

  // Smartcards tab
  QCheckBox *btnPkcs11RMH;
  QCheckBox *btnPkcs11RMB;
  QCheckBox *btnPkcs11Core;

  QCheckBox *btnPgpRMH;
  QCheckBox *btnPgpRMB;

};

#endif // FRMDEVICESMAIN_H

// vi:sw=2:ts=2:et:co=80:
