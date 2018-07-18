// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmAuditMain.cpp
 * clip-audit-config main dialog.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */


#include <QApplication>
#include <QDir>
#include <QKeyEvent>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <ClipConfig/CCException.h>
#include <QDesktopWidget>

#include "frmAuditMain.h"
#include "LogRotateFrame.h"
#include "LogRemoteFrame.h"
#include "qtcommon.h"

const QString rotateFile("logfiles");
const QString remoteFile("syslog");

frmAuditMain::frmAuditMain(QWidget *parent, const QString &path)
  : QDialog(parent)
{ 
  setWindowTitle("Configuration de la journalisation");
  setWindowIcon(QIcon(ICONPATH"preferences-audit.png"));

  QRect screen = QApplication::desktop()->screenGeometry();
  setMaximumHeight(screen.height() - 100);

  frmRotate = new LogRotateFrame(this, path + QDir::separator() + rotateFile);
  frmRemote = new LogRemoteFrame(this, path + QDir::separator() + remoteFile);

  btnSave = new QPushButton(QIcon(ICONPATH"document-save.png"), "Sauver", this);
  connect(btnSave, SIGNAL(clicked()), this, SLOT(save()));
  btnQuit = new QPushButton(QIcon(ICONPATH"dialog-close.png"), "Quitter", this);
  connect(btnQuit, SIGNAL(clicked()), this, SLOT(quit()));

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->addWidget(frmRotate);
  vb->addWidget(frmRemote);

  QHBoxLayout *hbb = new QHBoxLayout;
  hbb->setSpacing(8);
  hbb->addStretch(5);
  hbb->addWidget(btnSave);
  hbb->addSpacing(10);
  hbb->addWidget(btnQuit);
  hbb->addStretch(5);

  vb->addSpacing(4);
  vb->addLayout(hbb);
  vb->addSpacing(4);
  vb->addStretch(5);

  if (pending()) { // new options have been added
    try {
      frmRotate->save();
      frmRemote->save();
    } catch (CCException e) {
      QMessageBox::critical(0, "Echec d'écriture",
              "La mise à jour initiale des paramètres a échoué.\n"
              "Les permissions sur les fichiers de /etc/audit sont\n"
              "probablement incorrectes.",
              QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape);
    }
  }
}

void 
frmAuditMain::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Escape) {
    e->accept();
    quit();
  }
}

bool
frmAuditMain::save()
{
  if (!pending())
    return true;

  try {
    frmRotate->save();
    frmRemote->save();
  } catch (CCException e) {
    QMessageBox::critical(0, "Echec de sauvegarde",
              "La sauvegarde des paramètres modifiés a échoué.",
              QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape);
    return false;
  }

  QMessageBox::information(0, "Prise en compte des modifications",
              "Attention : les paramètres de journalisation modifiés ne\n"
              "seront pris en compte qu'après un redémarrage.",
              QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape);

  return true;
}

void
frmAuditMain::quit()
{
  if (pending()) {
   confirm:
    int answer = QMessageBox::question(0, "Sauver les modifications ?", 
              "Certaines modifications n'ont pas été sauvegardées.\n"
              "Souhaitez-vous les sauvegarder avant de quitter ?",
              QMessageBox::Yes | QMessageBox::Default, QMessageBox::No,
              QMessageBox::Cancel | QMessageBox::Escape);
    if (answer == QMessageBox::Cancel)
      return;
    if (answer == QMessageBox::Yes)
      if (!save())
        goto confirm;
  }
  QApplication::exit(0);
}

bool
frmAuditMain::pending()
{
  return (frmRotate->pending() || frmRemote->pending());
}

// vi:sw=2:ts=2:et:co=80:
