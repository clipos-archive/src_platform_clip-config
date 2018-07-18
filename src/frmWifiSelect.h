// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmWifiSelect.h
 * clip-config wireless network selection form header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMWIFISELECT_H
#define FRMWIFISELECT_H

#include <QDialog>
#include <vector>

class QButtonGroup;
class QPushButton;

typedef enum {
  WifiEncNone,
  WifiEncWEP,
  WifiEncWPA,
} wifi_enc_t;

typedef enum {
  WifiModeManaged,
  WifiModeAdHoc,
} wifi_mode_t;

class frmWifiSelect : public QDialog
{
  Q_OBJECT
  
 public:
  frmWifiSelect(void);

  QString getEssid(void) {return essid; };
  wifi_enc_t getType(void) {return type; };
  wifi_mode_t getMode(void) {return mode; };

 private slots:
  void clicked(int id);
  void save(void);
  void close(void);

 private:
  QString essid;
  QButtonGroup *buttons;
  QPushButton *cb, *sb;
  std::vector<wifi_enc_t> encTypes;
  wifi_enc_t type;
  std::vector<wifi_mode_t> modes;
  wifi_mode_t mode;
};

#endif

// vi:sw=2:ts=2:et:co=80:
