// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmNetAdd.h
 * clip-config new network profile form header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMNETADD_H
#define FRMNETADD_H

#include <QDialog>
#include <QString>

class QCheckBox;
class QLineEdit;
class QRadioButton;


class frmNetAdd : public QDialog
{
  Q_OBJECT

public:
  frmNetAdd(bool pendingChanges, const QString& currentProfile, bool isDefault);
  QString newName;
  int result;
  bool inherit;

 private:
   QCheckBox *chkSave;
   QLineEdit *edtNewName;
   QRadioButton *inheritButton;

 private slots:
   void OKPressed();
};



#endif // FRMNETADD_H

// vi:sw=2:ts=2:et:co=80:
