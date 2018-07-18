// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PkgListTab.cpp
 * clip-config scrollable package list.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI/DCSSI
 * Copyright (C) 2010-2012 SGDSN/SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QScrollArea>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QStringList>
#include <QApplication>
#include <QMessageBox>

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#include "PkgTable.h"
#include "PkgListTab.h"
#include <ClipWidgets/WaitDialog.h>
#include "qtcommon.h"
#include "frmPkgMain.h"

static const QString downloadmasterSocketPath("/var/run/download");

PkgListTab::PkgListTab(frmPkgMain *parent, const QString &jail, 
            const QString &output, const QString &input)
  : QFrame(parent), jail(jail)
{
  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(5);
  QScrollArea *sa = new QScrollArea(this);
  sa->setBackgroundRole(QPalette::Background);
  sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  tb = new PkgTable(0, jail, output, input);
  sa->setWidget(tb);

  vb->addWidget(new QLabel(QString("<b>Liste des paquetages optionnels - ")
                + jailName(jail) + "</b>", this));
  vb->addWidget(new QLabel(QString("<font color=blue><b>Bleu</b> : "
                "sélectionné</font> / <b>Noir</b> : disponible "), this));
  vb->addWidget(sa, 1);
  vb->addSpacing(8);

  QHBoxLayout *hbb = new QHBoxLayout();
  hbb->addStretch(5);

  QPushButton *db = new QPushButton(QIcon(ICONPATH"update.png"), 
                                    "Télécharger", this);
  db->setToolTip("Lancer un téléchargement de mises à jour "
                          "pour le " + jailName(jail));
  hbb->addWidget(db);
  hbb->addStretch(5);

  QPushButton *sb = new QPushButton(QIcon(ICONPATH"document-save.png"), 
                                    "Enregistrer", this);
  sb->setToolTip("Enregistrer la liste des paquetages optionnels "
                          "sélectionnés pour le " + jailName(jail));
  if (tb->empty())
    sb->setEnabled(false);
  hbb->addWidget(sb);
  hbb->addStretch(5);
  
  QPushButton *qb = new QPushButton(QIcon(ICONPATH"dialog-close.png"), 
                                    "Quitter", this);
  qb->setToolTip("Quitter l'application");
  hbb->addWidget(qb);
  hbb->addStretch(5);
  vb->addLayout(hbb);

  connect(qb, SIGNAL(clicked()), parent, SLOT(quit()));
  connect(sb, SIGNAL(clicked()), this, SLOT(save()));
  connect(db, SIGNAL(clicked()), this, SLOT(download()));

  QRect geom = tb->geometry();
  mwidth = geom.width() + 50;
}

bool 
PkgListTab::tryQuit(void)
{
  return (!tb->pending()); 
}

bool 
PkgListTab::confirmQuit(void)
{
  if (!tb->pending()) {
    return true;
  }

  int save = QMessageBox::question(this, 
      "Modifications non sauvées",
      "Certaines modifications n'ont pas été sauvées pour la cage " 
      + jail + ". Souhaitez-vous les enregistrer avant de quitter ?",
      "Enregistrer", "Ne pas enregistrer", "Annuler");
  if (save == 2) // Cancel
    return false;
  if (save == 1) {
    return true;
  }

  if (!tb->save()) {
    int abort = QMessageBox::question(this,
      "Erreur d'enregistrement",
      "L'enregistrement des modifications a échoué. "
      "Souhaitez-vous quitter malgré tout ?",
      "Quitter", "Ne pas quitter");
    if (abort)
      return false;
  }
  return true;
}

bool 
PkgListTab::save(void)
{
  if (!tb->save()) {
    qDebug("Failed to save table");
    return false;
  }
  return true;
}


static inline void 
download_warning(const QString& s) {
  QMessageBox::warning(0, "Erreur lors du téléchargement", s,
                    QMessageBox::Ok | QMessageBox::Default, 0, 0);
}

bool 
PkgListTab::download(void)
{
  int save;
  if (!tb->pending())
    goto run;

  save = QMessageBox::question(this, 
      "Modifications non sauvées",
      "Certaines modifications n'ont pas été sauvées. "
      "Souhaitez-vous les enregistrer avant de lancer "
      "le téléchargement ?",
      "Enregistrer", "Ne pas enregistrer", "Annuler");
  if (save == 2) // Cancel
    return true;
  if (save == 1)
    goto run;

  if (!tb->save()) {
    int abort = QMessageBox::question(this,
      "Erreur de sauvegarde",
      "La sauvegarde des modifications a échoué. "
      "Souhaitez-vous lancer le téléchargement malgré tout ?",
      "Lancer", "Ne pas lancer");
    if (abort)
      return true;
  }

run:
  bool finished = false;
  bool ok = false;
  char c;
  DownloadThread t(jail, c, finished);
  WaitDialog dialog("Téléchargement en cours", "Veuillez patienter pendant "
                            "le téléchargement", finished);

  t.start();
  dialog.exec();

  switch (c) {
    case 'Y':
      QMessageBox::information(0, "Téléchargement effectué",
        "Le téléchargement " + jailName(jail) + " a été "
        "correctement effectué. \n\nLes logiciels téléchargés vont "
        "maintenant être automatiquement installés.\n\n"
        "N.B.: l'installation de certaines mises à jour peut "
        "nécessiter un redémarrage du poste.",
        QMessageBox::Ok | QMessageBox::Default, 0, 0);
      ok = true;
      // Reload list of optional packages
      tb->init();
      break;
    
    case 'L':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être lancé : un téléchargement "
        + jailName(jail) + " est déjà en cours, ou les "
        "téléchargements sont verrouillés.");
      break;

    case 'J':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être lancé : "
        "cage non supportée.");
      break;

    case 'S':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être lancé : "
        "erreur de connexion.");
      break;

    case 'W':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être lancé : "
        "erreur d'écriture.");
      break;

    case 'R':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être lancé : "
        "erreur de lecture.");
      break;

    case 's':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être lancé : "
        "impossible de lancer le démon de vérification de signatures.");
      break;

    case 'r':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être lancé : "
        "erreur dans la récupération d'une erreur antérieure.");
      break;

    case 'u':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être lancé : "
        "impossible de joindre une source de mises à jour.");
      break;

    case 'd':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être complété : "
        "tous les paquetages n'ont pas pu être téléchargés.");
      break;

    case 'm':
      download_warning("Le téléchargement " + jailName(jail) + 
        " n'a pas pu être complété : "
        "la mise à jour du miroir local a échoué.");
      break;

    case 'N':
    default:
      download_warning("Le téléchargement " + jailName(jail) 
        + " a rencontré une "
        "erreur indéterminée (consulter les journaux "
        "de téléchargement pour plus d'information).");
      break;
  }

  return ok;
}

DownloadThread::DownloadThread (const QString& jail, 
                          char& ptrResult, bool& ptrFinished)
  : ptrResult(ptrResult), jail(jail), ptrFinished(ptrFinished)
{}

void 
DownloadThread::run () 
{
  int s;
  char c;

  ptrResult = 'J';

  if (jail == "clip") {
    c = 'A';
  } else if (jail == "rm_h") {
    c = 'B';
  } else if (jail == "rm_b") {
    c = 'C';
  } else {
    goto DT_end;
  }

  struct sockaddr_un sau;
  sau.sun_family = AF_UNIX;
  snprintf(sau.sun_path, sizeof(sau.sun_path), "%s", 
                    downloadmasterSocketPath.toStdString().c_str());
      
  ptrResult = 'S';
  s = socket(PF_UNIX, SOCK_STREAM, 0);
  if (s < 0)
    goto DT_end;
  
  if (::connect(s, (struct sockaddr *)&sau, 
      (socklen_t) sizeof(struct sockaddr_un)) < 0)
    goto DT_close;

  if (write(s, &c, 1) != 1) {
    ptrResult = 'W';
    goto DT_close;
  }
  if (read(s, &ptrResult, 1) != 1)
    ptrResult = 'R';

  DT_close:
  ::close (s);
    
  DT_end: 
    ptrFinished = true;
}

// vi:sw=2:ts=2:et:co=80:
