// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmUsersAdd.cpp
 * clip-config user add form.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "frmUsersAdd.h"
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

static inline const long 
min(long a, long b)
{
  return (a < b) ? a : b;
}

const char *type_str [] = {"Administrateur du socle",
         "Auditeur du socle",
         "Utilisateur",
         "Utilisateur privilégié",
         "Utilisateur nomade"};

const char *auth_str [] = {
         "Mot de passe",
         "Carte à puce"};

void 
print_error(uint32_t ret, QString verb, QString name) 
{
  QString msg;

  switch (ret) {
    case CMD_UNKNOWNUSER: 
      msg = "Impossible de " + verb + " le compte " + name + " inexistant.";
      break;
    case CMD_UNDET_USER:
      msg = "Impossible de déterminer le compte dont la session est en cours.";
      break;
    case CMD_CURRENTUSER:
      msg = "Impossible de " + verb + " l'utilisateur de la session en cours.";
      break;
    case CMD_USEREXISTS:
      msg = "Un compte du même nom existe déjà.";
      break;
    case CMD_INVALID_LOGIN:
      msg = "Le login " + name + "n'est pas de la forme [a-z_][a-zA-Z0-9_]+";
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
    case CMD_DISKSPACE:
      msg = "Pas assez d'espace disque disponible.";
      break;
    default:
      msg = "Erreur interne.";
      break;
  }
  
  QMessageBox::warning (0, "Opération impossible", msg,
                  QMessageBox::Ok | QMessageBox::Default, 0, 0);
}

// frmUsersAdd constructor
//----------------------

frmUsersAdd::frmUsersAdd(const diskinfo_t& di)
  : numRM(0), diskinfo(di), pwOk(false)
{
  setWindowTitle("Ajout d'un utilisateur");
  setWindowIcon(QIcon(ICONPATH"users.png"));

  if (hasRMH)
    numRM++;
  if (hasRMB)
    numRM++;

  userData = userinfo_alloc();;

  setWindowTitle("Ajouter un compte");

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(7);

  QHBoxLayout *hbName = new QHBoxLayout;
  hbName->setSpacing(7);

  hbName->addWidget(new QLabel("Nom : ", this));
  hbName->addStretch(2);
  hbName->addWidget(edtName = new QLineEdit("", this));
  edtName->setValidator(new QRegExpValidator(username_rx, this));
  vb->addLayout(hbName);

  vb->addWidget(new QLabel("Mécanisme d'authentification :", this));
  btnAuthType = new QButtonGroup(this);
  btnAuthType->setExclusive(true);
  btnAuthPw = new QRadioButton("Mot de passe", this);
  btnAuthType->addButton(btnAuthPw, AUTH_PW);
  btnAuthPkcs = new QRadioButton("Carte à puce", this);
  btnAuthType->addButton(btnAuthPkcs, AUTH_PKCS);
  connect(btnAuthType, SIGNAL(buttonClicked(int)), 
                            this, SLOT(authSelected(int)));

  QHBoxLayout *hbAuth = new QHBoxLayout;
  hbAuth->setSpacing(7);
  hbAuth->addStretch(2);
  hbAuth->addWidget(btnAuthPw);
  hbAuth->addSpacing(10);
  hbAuth->addWidget(btnAuthPkcs);
  hbAuth->addStretch(2);
  vb->addLayout(hbAuth);
  vb->addSpacing(10);

  QHBoxLayout *hbPasswd;
  for (int i = 0; i < 2; i++) {
    hbPasswd = new QHBoxLayout;
    hbPasswd->setSpacing(7);
    if (!i) {
      lblPasswd = new QLabel("", this);
      hbPasswd->addWidget(lblPasswd);
    } else {
      hbPasswd->addWidget(new QLabel("Seconde saisie : ", this));
    }
    hbPasswd->addStretch(2);
    hbPasswd->addWidget (edtPasswd[i] = new QLineEdit("", this));
    edtPasswd[i]->setEchoMode(QLineEdit::Password);
    connect(edtPasswd[i], SIGNAL(textChanged(const QString &)), 
                                         this, SLOT(updatePw()));
    vb->addLayout(hbPasswd);
  }
  lblPwMatch = new QLabel("", this);
  vb->addWidget(lblPwMatch);
  vb->addSpacing(10);

  QGridLayout *glType = new QGridLayout;
  glType->setVerticalSpacing(7);
  glType->setHorizontalSpacing(7);
  
  btnGroup = new QGroupBox("Type de compte", this);
  btnType = new QButtonGroup(this);
  btnType->setExclusive(true);

  bool userPossible = di.space_available >= 
    di.clip_partition_minsize + numRM * di.rm_partition_minsize;

  int index = 0;
  btnAdmin = new QRadioButton(type_str[T_ADMIN], btnGroup);
  btnType->addButton(btnAdmin, T_ADMIN);
  glType->addWidget(btnAdmin, index / 2, index % 2);
  index++;

  btnAudit = new QRadioButton(type_str[T_AUDIT], btnGroup);
  btnType->addButton(btnAudit, T_AUDIT);
  glType->addWidget(btnAudit, index / 2, index % 2);
  index++;
  
  if (numRM) {
    btnUser = new QRadioButton(type_str[T_USER], btnGroup);
    btnType->addButton(btnUser, T_USER);
    btnUser->setEnabled(userPossible);
    glType->addWidget(btnUser, index / 2, index % 2);
    index++;
  }
  
  btnPrivUser = new QRadioButton(type_str[T_PRIV_USER], btnGroup);
  btnType->addButton(btnPrivUser, T_PRIV_USER);
  btnPrivUser->setEnabled(userPossible);
  glType->addWidget(btnPrivUser, index / 2, index % 2);
  index++;
  
  if (numRM) {
    btnNomUser = new QRadioButton(type_str[T_NOMAD_USER], btnGroup);
    btnType->addButton(btnNomUser, T_NOMAD_USER);
    btnNomUser->setEnabled(userPossible);
    glType->addWidget(btnNomUser, index / 2, index % 2);
    index++;
  }


  if (numRM && userPossible)
    btnUser->setChecked(true);
  else
    btnAdmin->setChecked(true);

  btnGroup->setLayout(glType);
  vb->addWidget(btnGroup);

  if (numRM) {
    QHBoxLayout *hbTotalSize = new QHBoxLayout;
    hbTotalSize->setSpacing(7);
    hbTotalSize->addWidget(new QLabel("Taille totale allouée : ", this));
    hbTotalSize->addStretch(2);
    hbTotalSize->addWidget (lblTotalSize = new QLabel ("", this));
    vb->addLayout (hbTotalSize);

    sldTotalSize = new QSlider(this);
    sldTotalSize->setMinimum(numRM * di.rm_partition_minsize);
    sldTotalSize->setMaximum(di.space_available - di.clip_partition_minsize);
    sldTotalSize->setPageStep(16);
    sldTotalSize->setSliderPosition(min(numRM * di.default_partsize, 
                            di.space_available - di.clip_partition_minsize));
    sldTotalSize->setOrientation(Qt::Horizontal);
    sldTotalSize->setTracking(true);
    vb->addWidget(sldTotalSize);
  }

  if (numRM > 1) {
    QHBoxLayout *hbRepartition = new QHBoxLayout;
    hbRepartition->setSpacing(7);
    hbRepartition->addWidget(new QLabel("Répartition : ", this));
    hbRepartition->addStretch(2);
    hbRepartition->addWidget(new QLabel ("Niveau Bas = ", this));
    hbRepartition->addWidget(lblRMBSize = new QLabel ("", this));
    hbRepartition->addStretch(2);
    hbRepartition->addWidget(new QLabel ("Niveau Haut = ", this));
    hbRepartition->addWidget(lblRMHSize = new QLabel("", this));
    hbRepartition->addStretch(2);
    hbRepartition->addWidget(btnReset = 
        new QPushButton(QIcon(ICONPATH"edit-clear.png"), "RàZ", this));
    hbRepartition->addStretch(2);
    vb->addLayout(hbRepartition);

    sldRepartition = new QSlider(this); 
    sldRepartition->setMinimum(di.rm_partition_minsize);
    sldRepartition->setMaximum(sldTotalSize->value() - di.rm_partition_minsize);
    sldRepartition->setPageStep(16);
    sldRepartition->setSliderPosition(sldTotalSize->value() / 2 );
    sldRepartition->setOrientation(Qt::Horizontal);
    sldRepartition->setTracking(true);
    vb->addWidget(sldRepartition);
  }

  QHBoxLayout *hbButtons = new QHBoxLayout;
  hbButtons->setSpacing(7);
  hbButtons->addStretch(2);
  hbButtons->addWidget(btnAdd = 
            new QPushButton(QIcon(ICONPATH"dialog-ok.png"), 
                            "Ajouter", this));
  btnAdd->setEnabled(false);
  hbButtons->addStretch(2);
  hbButtons->addWidget(btnCancel = 
            new QPushButton(QIcon(ICONPATH"dialog-cancel.png"), 
                            "Annuler", this));
  hbButtons->addStretch(2);
  vb->addLayout(hbButtons);

  // Connections
  //------------

  connect(btnAdd, SIGNAL(clicked()), this, SLOT(giveUserData()));
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

  if (numRM) {
    connect(btnType, SIGNAL(buttonClicked(int)), this, SLOT(updateWidgets()));
    connect(sldTotalSize, SIGNAL(valueChanged(int)), 
                                           this, SLOT(updateWidgets()));
  }
  if (numRM > 1) {
    connect(sldRepartition, SIGNAL(valueChanged(int)), 
                                           this, SLOT(updateWidgets()));
    connect(btnReset, SIGNAL(clicked()), this, SLOT(resetRepartition()));
  }

  btnAuthPw->setChecked(true);
  authSelected(AUTH_PW);
  updateWidgets();
}

frmUsersAdd::~frmUsersAdd()
{
  userinfo_free(userData);
}

/*************************************************************/
/*                      Private slots                        */
/*************************************************************/

void 
frmUsersAdd::updatePw() 
{
  bool newok = true;
  bool empty = true;

  if (edtPasswd[0]->text().isEmpty() && edtPasswd[1]->text().isEmpty()) {
    newok = false;
  } else {
    empty = false;
    if (edtPasswd[0]->text() != edtPasswd[1]->text())
      newok = false;
  }

  if (newok != pwOk) {
    btnAdd->setEnabled(newok);
    pwOk = newok;
  }
  if (!empty) {
    lblPwMatch->setText((newok) ? "Les deux saisies correspondent" :
                                "Les deux saisies sont différentes");
  }
}

void
frmUsersAdd::authSelected(int type)
{
  int i;
  for (i = 0; i < 2; i++) {
    edtPasswd[i]->setValidator(new QRegExpValidator((type == AUTH_PKCS) ? 
                                sc_pin_rx : any_rx, this));
  }
  lblPasswd->setText((type == AUTH_PKCS) ? "Code PIN :" : "Mot de passe :");
}

void 
frmUsersAdd::updateWidgets() 
{
  if (numRM && (btnType->checkedId() == T_USER 
                  || btnType->checkedId() == T_PRIV_USER 
                  || btnType->checkedId() == T_NOMAD_USER)) {
    sldTotalSize->setEnabled(true);
    if (numRM > 1) {
      sldRepartition->setEnabled(true);
      btnReset->setEnabled(true);
    }

    int totalSize = sldTotalSize->value();
    int rmhSize = 0;
    int rmbSize = 0;
    if (numRM > 1) {
      rmbSize = sldRepartition->value();
      rmhSize = totalSize - rmbSize;
    } else {
      if (hasRMH)
        rmhSize = totalSize;
      if (hasRMB)
        rmbSize = totalSize;
    }
    lblTotalSize->setText(QString::number(totalSize));

    if (numRM > 1) {
      lblRMHSize->setText(QString::number(rmhSize));
      lblRMBSize->setText(QString::number(rmbSize));

      sldRepartition->setMinimum(diskinfo.rm_partition_minsize);
      sldRepartition->setMaximum(totalSize - diskinfo.rm_partition_minsize);
    }
  } else {
    if (numRM) 
      sldTotalSize->setEnabled(false);
    if (numRM > 1) {
      sldRepartition->setEnabled(false);
      btnReset->setEnabled(false);
    }
  }
}

void 
frmUsersAdd::resetRepartition() 
{
  if (numRM > 1) {
    sldRepartition->setSliderPosition(sldTotalSize->value() / 2);
    updateWidgets();
  }
}

void 
frmUsersAdd::giveUserData() 
{
  int auth_type = btnAuthType->checkedId();
  // Préparation des données pour la requête
  if (!edtName->hasAcceptableInput()) {
    QMessageBox::warning (0, "Nom de compte invalide",
        "Veuillez entrer un nom de compte valide s'il vous plaît.",
        QMessageBox::Ok | QMessageBox::Default, 0, 0);
    return;
  }

  if (edtPasswd[0]->text() != edtPasswd[1]->text()) {
    QMessageBox::warning (0, "Saisie invalide",
        "Les deux saisies ne coïncident pas.\n"
        "Veuillez les resaisir s'il vous plaît.",
        QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape, 0, 0);
    return;
  }

  bool checked = true;
  const char *ptr;
  unsigned int i, l;
  switch (auth_type) {
    case AUTH_PW:
      ptr = edtPasswd[0]->text().toStdString().c_str();
      l = edtPasswd[0]->text().length();
      for (i = 0; i < l; i++) {
        if (!isascii(*(ptr+i)) || !isprint(*(ptr+i)) || isspace(*(ptr+i))) {
          checked = false;
          break;
        }
      }
      break;
    case AUTH_PKCS:
      checked = edtPasswd[0]->hasAcceptableInput(); // Same value in [1]
      break;
    default:
      QMessageBox::warning(0, "Authentification inconnue",
        "Le type d'authentification demandé n'est pas supporté.\n"
        "Je me demande bien comment vous avez pu faire ça...",
        QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape, 0, 0);
      return;
  }

  if (!checked) {
    QMessageBox::warning (0, "Saisie invalide",
                   "Le mot de passe saisit contient des caractères non "
                   "supportés.\nEn particulier, les caractères accentués et "
                   "les espaces ne sont pas autorisés.",
                   QMessageBox::Ok|QMessageBox::Default|QMessageBox::Escape, 0);
    return;
  }
    

  int ret1 = asprintf(&userData->name, "%s", 
                  edtName->text().toStdString().c_str());
  userData->nlen = strlen(userData->name);
  int ret2 = asprintf(&userData->passwd, "%s", 
                  edtPasswd[0]->text().toStdString().c_str());
  if (ret1 == -1 || ret2 == -1) {
    QMessageBox::warning (0, "Manque de mémoire",
        "Pas assez de mémoire disponible pour compléter la requète",
        QMessageBox::Ok | QMessageBox::Default, 0, 0);
    return;
  }
  userData->plen = strlen(userData->passwd);
  userData->type = (usertype_t)btnType->checkedId();
  userData->auth = (authtype_t)auth_type;

  if (userData->type == T_USER || userData->type == T_PRIV_USER
                               || userData->type == T_NOMAD_USER) {
    if (numRM > 1) {
      userData->rmb_size = sldRepartition->value();
      userData->rmh_size = sldTotalSize->value() - userData->rmb_size;
    } else {
      if (hasRMH)
        userData->rmh_size = sldTotalSize->value();
      if (hasRMB)
        userData->rmb_size = sldTotalSize->value();
    }
  } else {
    userData->rmb_size = userData->rmh_size = 0;
  }

  // Requete
  bool finished = false;
  bool fatal = false;
  uint32_t ret;
  AddThread t(userData, fatal, ret, finished);
  WaitDialog dialog("Opération en cours", 
      "Veuillez patienter pendant la création du compte...", finished);
  t.start();
  dialog.exec();
 
  // Traitement du retour
  if (fatal) {
    fatalError("Impossible de se connecter auprès du démon userd.");
    return;
  }

  if (ret != CMD_OK) {
    if (ret == CMD_CURRENTUSER)
      ret = CMD_USEREXISTS;

    print_error(ret, "créer", edtName->text());
    return;
  }

  QMessageBox::information (0, "Opération réussie", 
          "L'utilisateur " + edtName->text() + " a été correctement créé.",
          QMessageBox::Ok | QMessageBox::Default, 0, 0);
  emit(accept());
}

/*************************************************************/
/*                        AddThread                          */
/*************************************************************/

AddThread::AddThread(userinfo_t *data, bool& ptrFatal, 
                        uint32_t& ptrRet, bool& ptrFinished)
  : userData(data), fatal(ptrFatal), ret(ptrRet), finished(ptrFinished)
{}


void 
AddThread::run() 
{
  int s = sock_connect(userdSocketPath.toStdString().c_str());
  if (s < 0) { 
    fatal = true;
    finished = true;
    return;
  }
  ret = client_create_user(s, userData);
  finished = true;
}

// vi:sw=2:ts=2:et:co=80:
