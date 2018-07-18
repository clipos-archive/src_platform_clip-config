// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file LogRotateFrame.cpp
 * clip-audit-config log rotation config frame
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QLayout>
#include <QLabel>
#include <QRadioButton>
#include <QRegExp>
#include <QSpinBox>
#include <ClipConfig/ClipConfigFile.h>

#include "LogRotateFrame.h"

static const QRegExp days_rx("\\d{3}");
static const QRegExp num_rx("\\d{1,2}");
static const QRegExp size_rx("\\d{2,6}");

LogRotateFrame::LogRotateFrame(QWidget *parent, const QString &path)
  : QFrame(parent), changed(false)
{
  spbNumber = new QSpinBox(this);
  spbNumber->setRange(5, 99);
  spbNumber->setValue(10);
  spbSize = new QSpinBox(this);
  spbSize->setRange(20, 999999);
  spbSize->setValue(512);
  spbSize->setSingleStep(8);
  spbDays = new QSpinBox(this);
  spbDays->setRange(365, 999);
  spbDays->setValue(365);
  spbDays->setSingleStep(30);
  rbtNumber = new QRadioButton("Nombre de fichiers à conserver :", this);
  rbtDays = new QRadioButton("Durée à conserver (jours) :", this);
  file = new ClipConfigFile;

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->addWidget(new QLabel("<b><u>Configuration de la "
              "rotation des journaux</u></b>", this));
  vb->addSpacing(5);

  QGridLayout *gl = new QGridLayout;
  gl->setVerticalSpacing(5);
  gl->setHorizontalSpacing(8);
  
  gl->addWidget(new QLabel("La rotation d'un journal s'effectue au plus tard "
        "chaque semaine,\nou chaque jour s'il dépasse cette taille maximale.",
        this), 0, 0, 1, 4);
  gl->addWidget(new QLabel("Taille maximale d'un journal (ko) :", this), 1, 1);
  gl->addWidget(spbSize, 1, 3);

  gl->addWidget(new QLabel("Les archives anciennes sont supprimées, au choix,"
        "\naprès un nombre fixe d'archives, ou après un certain temps.",
        this), 2, 0, 1, 4);
  gl->addWidget(rbtDays, 3, 1);
  gl->addWidget(spbDays, 3, 3);

  gl->addWidget(rbtNumber, 4, 1);
  gl->addWidget(spbNumber, 4, 3);

  gl->setColumnMinimumWidth(0, 20);
  gl->setColumnMinimumWidth(2, 50);

  vb->addLayout(gl);

  connect(rbtDays, SIGNAL(toggled(bool)), spbDays, SLOT(setEnabled(bool)));
  connect(rbtNumber, SIGNAL(toggled(bool)), spbNumber, SLOT(setEnabled(bool)));

  read(path);
}

void
LogRotateFrame::save()
{
  update();
  file->writeFile();
}

void
LogRotateFrame::read(const QString &path)
{
  file->readFile(path);
  QString tmp;
  bool ok;
  uint val;

  tmp = file->getNameVal("KEEP_FILES_NUMBER", num_rx);
  if (!tmp.isEmpty()) {
    val = tmp.toUInt(&ok);
    if (ok) {
      spbNumber->setValue(val);
      rbtNumber->setChecked(true);
      spbDays->setEnabled(false);
    }
  }

  // KEEP_DAYS has precedence over KEEP_FILES_NUMBER if both are defined
  tmp = file->getNameVal("KEEP_DAYS", days_rx);
  if (!tmp.isEmpty()) {
    val = tmp.toUInt(&ok);
    if (ok) {
      spbDays->setValue(val);
      rbtDays->setChecked(true);
      spbNumber->setEnabled(false);
    }
  }

  tmp = file->getNameVal("KEEP_SIZE", size_rx);
  if (!tmp.isEmpty()) {
    val = tmp.toUInt(&ok);
    if (ok)
      spbSize->setValue(val);
  }
}

void
LogRotateFrame::update()
{
  file->setNameVal("KEEP_SIZE", QString::number(spbSize->value()));
  if (rbtDays->isChecked()) {
    file->setNameVal("KEEP_DAYS", QString::number(spbDays->value()));
    file->setNameVal("KEEP_FILES_NUMBER", QString(""));
  } else {
    file->setNameVal("KEEP_DAYS", QString(""));
    file->setNameVal("KEEP_FILES_NUMBER", QString::number(spbNumber->value()));
  }
}

bool
LogRotateFrame::pending()
{
  update();
  return file->pendingChanges();
}


// vi:sw=2:ts=2:et:co=80:
