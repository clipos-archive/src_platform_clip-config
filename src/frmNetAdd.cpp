// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmNetAdd.cpp
 * clip-config new network profile form.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "qtcommon.h"
#include "consts.h"
#include "frmNetAdd.h"
#include "ClipNetProfile.h"

#include <QCheckBox>
#include <QDir>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QRegExpValidator>



// frmNetAdd constructor
//-----------------------

frmNetAdd::frmNetAdd(bool pendingChanges, const QString& currentProfile,
    bool isDefault)
  : QDialog (0, Qt::Dialog | Qt::SubWindow),
    newName(QString::null), result(ADD_CANCEL)
{
  setWindowTitle("Ajout de profil réseau");
  setWindowIcon(QIcon(ICONPATH"applications-internet.png"));

  edtNewName = new QLineEdit("", this);
  edtNewName->setValidator(new QRegExpValidator (profileName_rx, this));
   
  chkSave = 0;

  QPushButton* btnOK = new QPushButton(QIcon(ICONPATH"dialog-ok.png"), 
                                       "OK", this);
  connect(btnOK, SIGNAL(clicked()), this, SLOT(OKPressed()));

  QPushButton* btnCancel = new QPushButton(QIcon(ICONPATH"dialog-cancel.png"), 
                                           "Annuler", this);
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

  QHBoxLayout* layButtons = new QHBoxLayout;
  layButtons->setSpacing(4);

  layButtons->addSpacerItem(new QSpacerItem(50, 0, 
                  QSizePolicy::Expanding, QSizePolicy::Minimum));
  layButtons->addWidget(btnOK);
  layButtons->addSpacerItem(new QSpacerItem(2, 0, 
                  QSizePolicy::Expanding, QSizePolicy::Minimum));
  layButtons->addWidget(btnCancel);
  layButtons->addItem(new QSpacerItem(4, 0, 
                  QSizePolicy::Expanding, QSizePolicy::Minimum));

  QVBoxLayout* lv = new QVBoxLayout(this);
  lv->setSpacing(4);

  lv->addSpacerItem(new QSpacerItem(0, 10, 
                  QSizePolicy::Minimum, QSizePolicy::Expanding));
  lv->addWidget(new QLabel("Nom du nouveau profil :", this));
  lv->addWidget(edtNewName);

  inheritButton = new QRadioButton(
      "Hériter les paramètres (recommandé)", this);
  inheritButton->setToolTip("<qt/>Tout changement de configuration du profil "
      "par défaut sera automatiquement reporté dans les profils qui en "
      "héritent. Il est possible de choisir dans chaque onglet les "
      "paramètres qui doivent différer du profil par défaut.");
  QRadioButton* copyButton = new QRadioButton("Copier les paramètres", this);
  copyButton->setToolTip("<qt/>Les paramètres du profil courant sont recopiés "
    "dans le nouveau profil et les configurations des deux profils "
    "évoluent ensuite indépendamment.");
  lv->addWidget(inheritButton);
  lv->addWidget(copyButton);
  if (isDefault) {
    inheritButton->setChecked(true);
  } else {
    copyButton->setChecked(true);
    inheritButton->setEnabled(false);
    lv->addWidget(new QLabel("L'héritage n'est possible qu'à partir du "
          "profil par défaut."));
  }

  if (pendingChanges) {
    lv->addSpacerItem(new QSpacerItem(0, 10, 
                  QSizePolicy::Minimum, QSizePolicy::Expanding));
    lv->addWidget(new QLabel("Attention, le profil en cours d'édition "
                      "contient des modifications non sauvegardées.", this));

    chkSave = new QCheckBox("Sauvegarder le profil " + currentProfile + 
                                " avant la création du nouveau profil", this);
    chkSave->setChecked(false);
    lv->addWidget(chkSave);
  }

  lv->addSpacerItem(new QSpacerItem(0, 10, 
                  QSizePolicy::Minimum, QSizePolicy::Expanding));
  lv->addLayout(layButtons);
}

void 
frmNetAdd::OKPressed() 
{
  QString profileName = edtNewName->text();

  if (!profileName_rx.exactMatch(profileName)) {
    QMessageBox::warning (0, "Nom de profil incorrect",
			  "Le nom " + profileName + " n'est pas un nom de profil valide.",
			  QMessageBox::Ok | QMessageBox::Default, 0, 0);
    return;
  }
    
  // Check validator profileName_rx
  QDir dir;
  QString path = netconfdParentDir + "/" + profileName;
  
  // TODO : Proposer d'écraser ?
  if (dir.exists(path)) {
    QMessageBox::warning (0, "Nom de profil existant",
			  "Le profil " + profileName + " existe déjà. Pour l'écraser, "
        "veuillez d'abord le supprimer.",
			  QMessageBox::Ok | QMessageBox::Default, 0, 0);
    return;
  }
    
  newName = profileName;
  result = ADD_OK | ((chkSave != 0 && chkSave->isChecked()) ? 
                                        ADD_SAVE_BEFORE : ADD_DONTSAVE);

  inherit = inheritButton->isChecked();

  accept();
}

// vi:sw=2:ts=2:et:co=80:
