// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetWifiTab.h
 * clip-config net wireless config tab widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETWIFITAB_H
#define NETWIFITAB_H

#include <QColor>

#include "NetTab.h"

class LabelledField;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;

class NetWifiTab : public NetTab
{
  Q_OBJECT
 
 public:
  NetWifiTab(QWidget *parent, ClipNetProfile *profile);

 public slots:
  bool saveCurrentProfile();
  bool loadCurrentProfile();
  void enable(bool on);

 private slots:
  void scan();
  void updateEncryption(int index);
  void showWepKey(bool on);
  void showWpaKey(bool on);
  void checkWpaKey();
  void setWpaKeyMgmt(int index);

 private:
  QColor activeCol;
  QColor inactiveCol;

  QPushButton *btnScan;

  QButtonGroup *mode;
  QButtonGroup *encryption;

  QRadioButton *btnModeManaged;
  QRadioButton *btnModeAdHoc;
  QRadioButton *btnNone;
  QRadioButton *btnWep;
  QRadioButton *btnWpa;

  QLineEdit *edtEssid;
  QLineEdit *edtWepKey;
  QLineEdit *edtWpaKey;

  QCheckBox *chkWepKey;
  QCheckBox *chkWpaKey;

  QComboBox *cmbWpaKeyMgmt;

  QLabel *lblWepKey;
  QLabel *lblWpaKeyMgmt;
  QLabel *lblWpaKey;
};

#endif // NETWIFITAB_H

// vi:sw=2:ts=2:et:co=80:
