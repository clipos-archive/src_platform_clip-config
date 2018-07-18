// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmNetConst.cpp
 * clip-config-netconst main dialog.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */


#include <ClipWidgets/IPField.h>
#include <ClipConfig/ClipConfigFile.h>
#include <QApplication>
#include <QCheckBox>
#include <QFile>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>

#include "frmNetConst.h"
#include "qtcommon.h"


frmNetConst::frmNetConst()
  : QDialog(0)
{ 
  setWindowTitle("Configuration des adresses internes");
  setWindowIcon(QIcon(ICONPATH"preferences-system-network.png"));

  file = new ClipConfigFile();
  file->readFile(netConstPath);

  chkUpdateVirt = new QCheckBox("Adresse dynamique", this);
  chkAdminVirt = new QCheckBox("Adresse dynamique", this);
  chkAuditVirt = new QCheckBox("Adresse dynamique", this);

  updateAddr = new IPField(this, "Adresse UPDATE");
  connect(chkUpdateVirt, SIGNAL(stateChanged(int)), 
                        this, SLOT(enableUpdateVirt(int)));
  adminAddr = new IPField(this, "Adresse ADMIN");
  connect(chkAdminVirt, SIGNAL(stateChanged(int)), 
                        this, SLOT(enableAdminVirt(int)));
  auditAddr = new IPField(this, "Adresse AUDIT");
  connect(chkAuditVirt, SIGNAL(stateChanged(int)), 
                        this, SLOT(enableAuditVirt(int)));

  if (hasRMH) {
    chkRmhVirt = new QCheckBox("Adresse dynamique", this);
    rmhAddr = new IPField(this, "Adresse niveau Haut");
    connect(chkRmhVirt, SIGNAL(stateChanged(int)), 
                        this, SLOT(enableRmhVirt(int)));
  } else {
    chkRmhVirt = NULL;
    rmhAddr = NULL;
  }

  btnSave = new QPushButton(QIcon(ICONPATH"document-save.png"), "Sauver", this);
  connect(btnSave, SIGNAL(clicked()), this, SLOT(save()));
  btnQuit = new QPushButton(QIcon(ICONPATH"dialog-close.png"), "Quitter", this);
  connect(btnQuit, SIGNAL(clicked()), this, SLOT(quit()));

  QVBoxLayout *vb = new QVBoxLayout(this);
  QHBoxLayout *hb;
  vb->setSpacing(4);

  vb->addWidget(new QLabel("<b><u>Adresses internes des cages</u></b> "
                "(indépendantes du profil réseau)", this));

#define add_line(ipfield, chkbox) do {\
  hb = new QHBoxLayout;\
  hb->addSpacing(10);\
  hb->addWidget(ipfield);\
  hb->addSpacing(10);\
  hb->addWidget(chkbox);\
  hb->addSpacing(10);\
  vb->addSpacing(5);\
  vb->addLayout(hb); \
} while (0)

  add_line(updateAddr, chkUpdateVirt);
  add_line(adminAddr, chkAdminVirt);
  add_line(auditAddr, chkAuditVirt);
  if (hasRMH) {
    add_line(rmhAddr, chkRmhVirt);
  }

#undef add_line

  QHBoxLayout *hbb = new QHBoxLayout;
  hbb->setSpacing(8);
  hbb->addStretch(5);
  hbb->addWidget(btnSave);
  hbb->addSpacing(10);
  hbb->addWidget(btnQuit);
  hbb->addStretch(5);

  vb->addSpacing(10);
  vb->addLayout(hbb);
  vb->addSpacing(10);
  vb->addStretch(5);

  load();
}

void 
frmNetConst::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Escape) {
    e->accept();
    quit();
  }
}

void 
frmNetConst::update()
{
  QString ip;

  if (chkUpdateVirt->isChecked()) {
    file->setNameVal("UPDATE_ADDR", virtual_str); 
  } else {
    ip = updateAddr->returnIP();
    if (ip != "0.0.0.0") 
      file->setNameVal("UPDATE_ADDR", ip);
  }

  if (chkAdminVirt->isChecked()) {
    file->setNameVal("ADMIN_ADDR", virtual_str); 
  } else {
    ip = adminAddr->returnIP();
    if (ip != "0.0.0.0") 
      file->setNameVal("ADMIN_ADDR", ip);
  }

  if (chkAuditVirt->isChecked()) {
    file->setNameVal("AUDIT_ADDR", virtual_str); 
  } else {
    ip = auditAddr->returnIP();
    if (ip != "0.0.0.0") 
      file->setNameVal("AUDIT_ADDR", ip);
  }

  if (hasRMH) {
    if (chkRmhVirt->isChecked()) {
      file->setNameVal("RMH_ADDR", virtual_str); 
    } else {
      ip = rmhAddr->returnIP();
      if (ip != "0.0.0.0") 
        file->setNameVal("RMH_ADDR", ip);
    }
  }
}

bool
frmNetConst::write()
{
  int confirm = QMessageBox::question(0, "Confirmation",
              "Attention : la modification des adresses internes d'un poste\n"
              "CLIP est une opération avancée, qui n'a pas de raison d'être\n"
              "effectuée dans le fonctionnement normal du poste. Une\n"
              "erreur de configuration de ces adresses peut rendre \n"
              "impossible un démarrage normal.\n"
              "Confirmez-vous la modification ?",
              QMessageBox::Yes, 
              QMessageBox::Cancel | QMessageBox::Default | QMessageBox::Escape);

  if (confirm != QMessageBox::Yes)
    return false;

  file->writeFile();
  return true;
}

void
frmNetConst::save()
{
  update();
  write();
}

void 
frmNetConst::load()
{
  bool virt;
#define load_one(ipfield, chkbox, varname) do {\
  virt = !file->getNameVal(varname, virtual_rx).isEmpty(); \
  if (virt) { \
    chkbox->setChecked(true); \
    ipfield->setNameValue("0.0.0.0"); \
  } else { \
    ipfield->setNameValue(file->getNameVal(varname, ip_rx)); \
    chkbox->setChecked(false); \
  } \
} while (0)
  
  load_one(updateAddr, chkUpdateVirt, "UPDATE_ADDR");
  load_one(adminAddr, chkAdminVirt, "ADMIN_ADDR");
  load_one(auditAddr, chkAuditVirt, "AUDIT_ADDR");

  if (hasRMH) {
    load_one(rmhAddr, chkRmhVirt, "RMH_ADDR");
  }
}

void
frmNetConst::quit()
{
  update();
  if (file->pendingChanges()) {
   confirm:
    int answer = QMessageBox::question(0, "Sauver les modifications ?", 
              "Certaines modifications n'ont pas été sauvegardées.\n"
              "Souhaitez-vous les sauvegarder avant de quitter ?",
              QMessageBox::Yes | QMessageBox::Default, QMessageBox::No,
              QMessageBox::Cancel | QMessageBox::Escape);
    if (answer == QMessageBox::Cancel)
      return;
    if (answer == QMessageBox::Yes)
      if (!write())
        goto confirm;
  }
  QApplication::exit(0);
}

void
frmNetConst::enableUpdateVirt(int state)
{
  updateAddr->setEnabled((state != Qt::Checked));
}

void
frmNetConst::enableAdminVirt(int state)
{
  adminAddr->setEnabled((state != Qt::Checked));
}

void
frmNetConst::enableAuditVirt(int state)
{
  auditAddr->setEnabled((state != Qt::Checked));
}

void
frmNetConst::enableRmhVirt(int state)
{
  if (rmhAddr)
    rmhAddr->setEnabled((state != Qt::Checked));
}

// vi:sw=2:ts=2:et:co=80:
