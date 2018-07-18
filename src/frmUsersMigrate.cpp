// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmUsersMigrate.cpp
 * clip-config user migrate form.
 *
 * Copyright (C) 2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "frmUsersMigrate.h"
#include "consts.h"
#include "qtcommon.h"
#include <ClipWidgets/WaitDialog.h>

#include <QButtonGroup>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QRegExpValidator>
#include <QSlider>




// frmUsersMigrate constructor
//----------------------

frmUsersMigrate::frmUsersMigrate(QString user)
  : name(user), pwOk(false)
{
  setWindowTitle("Migration vers authentification par carte à puce");
  setWindowIcon(QIcon(ICONPATH"secure-card.png"));

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(7);

  QHBoxLayout *hbPasswd;
  for (int i = 0; i < 3; ++i) {
    hbPasswd = new QHBoxLayout;
    hbPasswd->setSpacing(7);
    switch(i) {
    case 0:
      hbPasswd->addWidget(new QLabel("Mot de passe :", this));
      break; 
    case 1:
      hbPasswd->addWidget(new QLabel("Code PIN :", this));
      break;
    case 2:
      hbPasswd->addWidget(new QLabel("Seconde saisie : ", this));
      break;
    }
    hbPasswd->addStretch(2);
    hbPasswd->addWidget (edtPasswd[i] = new QLineEdit("", this));
    edtPasswd[i]->setEchoMode(QLineEdit::Password);
    connect(edtPasswd[i],
            SIGNAL(textChanged(const QString &)), 
            this, SLOT(updatePw()));
    vb->addLayout(hbPasswd);
  }
  lblPwMatch = new QLabel("", this);
  vb->addWidget(lblPwMatch);
  vb->addSpacing(10);

  QHBoxLayout *hbButtons = new QHBoxLayout;
  hbButtons->setSpacing(7);
  hbButtons->addStretch(2);
  hbButtons->addWidget(btnMigrate = 
            new QPushButton(QIcon(ICONPATH"dialog-ok.png"), 
                            "Migrer", this));
  btnMigrate->setEnabled(false);
  hbButtons->addStretch(2);
  hbButtons->addWidget(btnCancel = 
            new QPushButton(QIcon(ICONPATH"dialog-cancel.png"), 
                            "Annuler", this));
  hbButtons->addStretch(2);
  vb->addLayout(hbButtons);

  // Connections
  //------------

  connect(btnMigrate, SIGNAL(clicked()), this, SLOT(commit()));
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

}

frmUsersMigrate::~frmUsersMigrate()
{
}

/*************************************************************/
/*                      Private slots                        */
/*************************************************************/

void 
frmUsersMigrate::updatePw() 
{
  bool newok;
  bool empty = true;

  newok = !(edtPasswd[0]->text().isEmpty());

  if (edtPasswd[1]->text().isEmpty() || edtPasswd[2]->text().isEmpty()) {
    newok = false;
  } else {
    empty = false;
    if (edtPasswd[1]->text() != edtPasswd[2]->text())
      newok = false;
  }

  if (newok != pwOk) {
    btnMigrate->setEnabled(newok);
    pwOk = newok;
  }
  if (!empty) {
    lblPwMatch->setText((newok) ?
                        "Les deux saisies correspondent" :
                        "Les deux saisies sont différentes");
  }
}

void 
frmUsersMigrate::commit() 
{

  if (edtPasswd[1]->text() != edtPasswd[2]->text()) {
    QMessageBox::warning (0, "Saisie invalide",
        "Les deux saisies ne coïncident pas.\n"
        "Veuillez les resaisir s'il vous plaît.",
        QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape, 0, 0);
    return;
  }

  // Requete
  bool finished = false;
  bool fatal = false;
  uint32_t ret = CMD_FAULT;
  char *cname = strdup(name.toStdString().c_str());
  char *cpassword = strdup(edtPasswd[0]->text().toStdString().c_str());
  char *cpin = strdup(edtPasswd[1]->text().toStdString().c_str());

  MigrateThread t(cname, cpassword, cpin,
                  fatal, ret, finished);
  WaitDialog dialog("Opération en cours", 
      "Veuillez patienter pendant la migration du compte...", finished);
  t.start();
  dialog.exec();
 
  free(cname);
  free(cpassword);
  free(cpin);

  // Traitement du retour
  if (fatal) {
    fatalError("Impossible de se connecter au démon userd.");
    emit(reject());

  } else if (ret != CMD_OK) {
    QString msg;
    
    switch (ret) {
    case CMD_UNKNOWNUSER: 
      msg = "Impossible de migrer le compte " + name + " inexistant.";
      break;
    case CMD_UNDET_USER:
      msg = "Impossible de déterminer le compte dont la session est en cours.";
      break;
    case CMD_WEAK_PASSWD: 
      msg = "Le mot de passe proposé est trop faible.";
      break;
    case CMD_INVALID_CARD:
      msg = "Aucune carte ou problème de communication avec la carte insérée.";
      break;
    case CMD_INVALID_CA_CHAIN:
      msg = "Chaîne de certification invalide.";
      break;
    default:
      msg = "Erreur interne.";
      break;
    }
    
    QMessageBox::warning (0, "Opération impossible", msg,
                          QMessageBox::Ok | QMessageBox::Default, 0, 0);
    emit(reject());

  } else {
    
    QMessageBox::information (0, "Opération réussie", 
                              "L'utilisateur a été correctement migré.",
                              QMessageBox::Ok | QMessageBox::Default, 0, 0);
    emit(accept());
  }
}

/*************************************************************/
/*                      MigrateThread                        */
/*************************************************************/

MigrateThread::MigrateThread(const char *user, const char *password, const char *pincode,
                             bool& ptrFatal, uint32_t& ptrRet, bool& ptrFinished)
  : name(user), passwd(password), pin(pincode), fatal(ptrFatal), ret(ptrRet), finished(ptrFinished)
{}


void 
MigrateThread::run() 
{
  int s = sock_connect(userdSocketPath.toStdString().c_str());
  if (s < 0) { 
    fatal = true;
    finished = true;
    return;
  }
  
  ret = client_migrate_user(s, name, passwd, pin);

  finished = true;
}
