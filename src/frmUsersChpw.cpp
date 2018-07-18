// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmUserChpw.cpp
 * clip-config password change form.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "frmUsersChpw.h"
#include "consts.h"
#include "qtcommon.h"
#include <ClipWidgets/WaitDialog.h>

#include <userd/userd.h>

#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include <ctype.h>

int g_daemonized = 0;
int g_verbose = 0;

static inline void 
print_error (uint32_t ret)
{
  QString msg;

  switch (ret) {
    case CMD_UNKNOWNUSER: 
      msg = "Erreur: compte courant inconnu du système";
      break;
    case CMD_UNDET_USER: 
      msg = "Erreur: impossible de déterminer "
                              "le compte courant";
      break;
    case CMD_WEAK_PASSWD: 
      msg = "Erreur: le mot de passe proposé est trop faible."; 
      break;
    case CMD_DISKSPACE: 
      msg = "Erreur: pas assez d'espace disque disponible."; 
      break;
    default: 
      msg = QString("Erreur interne %1").arg(ret);
      break;
    }
  
    QMessageBox::warning (0, "Opération impossible", msg,
        QMessageBox::Ok | QMessageBox::Default, 0, 0);
}

frmUsersChpw::frmUsersChpw(QWidget *parent)
  : QDialog(parent), ok(false), match(true), changed(false)
{
  setWindowTitle("Modification de mot de passe");
  setWindowIcon(QIcon(ICONPATH"preferences-desktop-user-password.png"));

  QVBoxLayout* vb = new QVBoxLayout(this);
  vb->setSpacing(6);

  QHBoxLayout *hbt = new QHBoxLayout(0);
  hbt->setSpacing(6);
  hbt->addWidget(new QLabel("Ancien mot de passe", this));
  hbt->addStretch(2);
  edtOldPw = new QLineEdit("", this);
  edtOldPw->setEchoMode(QLineEdit::Password);
  connect(edtOldPw, SIGNAL(textChanged(const QString &)),
                                        this, SLOT(update()));
  hbt->addWidget(edtOldPw);
  vb->addLayout(hbt);

  for (unsigned int i = 0; i < 2; i++) {
    QHBoxLayout *hb = new QHBoxLayout();
    hb->setSpacing(6);
    hb->addWidget(new QLabel((i == 0) ? 
          "Nouveau mot de passe : " : "Seconde saisie : ", this));
    hb->addStretch(2);
    edtNewPw[i] = new QLineEdit("", this);
    hb->addWidget(edtNewPw[i]);
    vb->addLayout(hb);
    edtNewPw[i]->setEchoMode(QLineEdit::Password);
    connect(edtNewPw[i], SIGNAL(textChanged(const QString &)),
                                          this, SLOT(update()));
  }
  lblMatch = new QLabel("", this);
  vb->addWidget(lblMatch);

  QHBoxLayout *hbb = new QHBoxLayout();
  hbb->setSpacing(6);
  btnSave = 
      new QPushButton(QIcon(ICONPATH"dialog-ok.png"), "Appliquer", this);
  btnSave->setEnabled(false);
  connect(btnSave, SIGNAL(clicked()), this, SLOT(save()));
  hbb->addWidget(btnSave);
  hbb->addStretch(0);
  btnCancel = 
      new QPushButton(QIcon(ICONPATH"dialog-close.png"), "Quitter", this);
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));
  hbb->addWidget(btnCancel);
  vb->addLayout(hbb);

  update();
}

void 
frmUsersChpw::update(void)
{
  bool newok = true;
  bool newmatch = true;
  if (edtNewPw[0]->text() != edtNewPw[1]->text()) {
    newok = false;
    newmatch = false;
  } else if (edtNewPw[0]->text().isEmpty() 
              || edtOldPw->text().isEmpty()) {
    newok = false;
  } 

  if (newok != ok) {
    btnSave->setEnabled(newok);
    ok = newok;
  }

  if (newmatch != match) {
    lblMatch->setText((newmatch) ? "Les deux saisies correspondent" :
                                 "Les deux saisies sont différentes");
    match = newmatch;
  }
  changed = true;
}

void 
frmUsersChpw::save(void)
{
  if (!changed) {
    QMessageBox::information(this, "Changement déjà appliqué",
                   "Les changements ont déjà été appliqués.",
                   QMessageBox::Ok|QMessageBox::Default|QMessageBox::Escape);
    return;
  }
  if (edtNewPw[0]->text() != edtNewPw[1]->text()) {
    QMessageBox::warning(this, "Mot de passe non confirmé",
                   "Les deux saisies du nouveau mot de passe diffèrent.",
                   QMessageBox::Ok|QMessageBox::Default|QMessageBox::Escape, 0);
    return;
  }
  const char *ptr;
  bool checked = true;
  ptr = edtNewPw[0]->text().toStdString().c_str();
  unsigned int i, l = edtNewPw[0]->text().length();
  for (i = 0; i < l; i++) {
    if (!isascii(*(ptr+i)) || !isprint(*(ptr+i)) || isspace(*(ptr+i))) {
      checked = false;
      break;
    }
  }
  if (!checked) {
    QMessageBox::warning(this, "Mot de passe non supporté",
                   "Le nouveau mot de passe contient des caractères non "
                   "supportés.\nEn particulier, les caractères accentués et "
                   "les espaces ne sont pas autorisés.",
                   QMessageBox::Ok|QMessageBox::Default|QMessageBox::Escape, 0);
    return;
  }

  bool finished = false;
  bool fatal = false;
  uint32_t ret;
  ChangeThread t(edtOldPw->text(), edtNewPw[0]->text(), fatal, ret, finished);
  WaitDialog waiter("Opération en cours", "Veuillez patienter pendant "
                  "le changement de mot de passe.", finished);
  t.start();
  waiter.exec();

  if (fatal) {
    fatalError("Impossible de se connecter auprès du démon userd.");
    return;
  }

  if (ret != CMD_OK) {
    print_error(ret);
    return;
  }

  QMessageBox::information(0, "Opération réussie",
            "Le mot de passe du compte courant a été correctement modifié.",
            QMessageBox::Ok|QMessageBox::Default|QMessageBox::Escape);

  changed = false;
}

void 
frmUsersChpw::cancel(void)
{
  if (!changed || (edtNewPw[0]->text().isEmpty() 
        && edtNewPw[1]->text().isEmpty() && edtOldPw->text().isEmpty())) {
    qApp->quit();
    return;
  }

  int abort = QMessageBox::question(this, "Fermeture sans appliquer",
      "Fermer la fenêtre sans appliquer les modifications ?",
      "Fermer", "Ne pas fermer");
  if (!abort) {
    qApp->quit();
    return;
  }
}

ChangeThread::ChangeThread(const QString &oldpw, const QString &newpw, 
          bool &ptrFatal, uint32_t& ptrRet, bool& ptrFinished)
    : oldPw(oldpw), newPw(newpw), fatal(ptrFatal), 
      ret(ptrRet), finished(ptrFinished)
{}

void 
ChangeThread::run() 
{
  int s = sock_connect(userdSocketPath.toStdString().c_str());
  if (s < 0) {
    ret = CMD_FAULT;
    fatal = true;
    finished = true;
    return;
  }

  ret = client_chpw_self(s, oldPw.toStdString().c_str(), 
                            newPw.toStdString().c_str());
  finished = true;
}

// vi:sw=2:ts=2:et:co=80:
