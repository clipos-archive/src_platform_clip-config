// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmDateTime.cpp
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2011 SGDSN/SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QTime>
#include <QTimeEdit>
#include <QCalendarWidget>

#include <ClipConfig/ClipConfigFile.h>

#include "frmDateTime.h"
#include "qtcommon.h"
#include "consts.h"

frmDateTime::frmDateTime(void)
  : QDialog(0), pending(false)
{
  setWindowTitle("Date et heure");
  setWindowIcon(QIcon(ICONPATH"preferences-system-time.png"));

  file = new ClipConfigFile();
  file->readFile(ntpPath);

  btnApply = new QPushButton(QIcon(ICONPATH"dialog-ok.png"), "Appliquer", this);
  btnQuit = new QPushButton(QIcon(ICONPATH"dialog-close.png"), "Quitter", this);

  edtDate = new QCalendarWidget(this);
  edtTime = new QTimeEdit(QTime::currentTime(), this);
  edtTime->setDisplayFormat("hh:mm:ss");

  chkRemoteSync = new QCheckBox("Activer la synchronisation horaire", this);
  chkRemoteSync->setChecked(true); // make sure we get the 'disable' signal 
  lblRemoteUrl = new QLabel("Serveur de synchronisation horaire", this);
  edtRemoteUrl = new QLineEdit(this);

  connect(btnApply, SIGNAL(clicked()), this, SLOT(apply()));
  connect(btnQuit, SIGNAL(clicked()), this, SLOT(quit()));

  connect(edtDate, SIGNAL(selectionChanged()), this, SLOT(setPending()));
  connect(edtDate, SIGNAL(currentPageChanged(int, int)), this, SLOT(updateDay()));
  connect(edtTime, SIGNAL(timeChanged(const QTime &)), 
                                this, SLOT(setPending()));
  connect(chkRemoteSync, SIGNAL(stateChanged(int)), 
                                this, SLOT(enableRemoteSync(int)));

  QVBoxLayout *vb = new QVBoxLayout(this);
  QHBoxLayout *hb;

  hb = new QHBoxLayout;
  hb->addStretch(3);
  hb->addWidget(new QLabel("<b><u>Configuration manuelle</u></b>", this));
  hb->addStretch(3);
  vb->addLayout(hb);
  vb->addSpacing(5);

  vb->setSpacing(3);
  vb->addSpacing(5);
  vb->addWidget(
    new QLabel("<u>Date</u> :", this));
  vb->addSpacing(5);
  vb->addWidget(edtDate);
  vb->addSpacing(10);
  vb->addWidget(
    new QLabel("<u>Heure</u> :", this));
  vb->addSpacing(5);
  vb->addWidget(edtTime);
  vb->addSpacing(10);

  hb = new QHBoxLayout;
  hb->addStretch(3);
  hb->addWidget(new QLabel("<b><u>Synchronisation automatique</u></b>", this));
  hb->addStretch(3);
  vb->addLayout(hb);
  vb->addSpacing(5);

  vb->addWidget(chkRemoteSync);
  vb->addSpacing(10);
  hb = new QHBoxLayout;
  hb->setSpacing(6);
  hb->addSpacing(5);
  hb->addWidget(lblRemoteUrl);
  hb->addSpacing(10);
  hb->addWidget(edtRemoteUrl);
  hb->addStretch(3);
  vb->addLayout(hb);
  vb->addSpacing(10);

  hb = new QHBoxLayout;
  hb->setSpacing(6);
  hb->addStretch(3);
  hb->addWidget(btnApply);
  hb->addSpacing(20);
  hb->addWidget(btnQuit);
  hb->addStretch(3);
  vb->addLayout(hb);
  vb->addSpacing(5);

  load();
}

void 
frmDateTime::setPending(void)
{
  pending = true;
}

void
frmDateTime::updateDay(void)
{
  QDate date;
  if (QDate::isValid(edtDate->yearShown(),
               edtDate->monthShown(),
               edtDate->selectedDate().day())) {
    date.setDate(edtDate->yearShown(),
                 edtDate->monthShown(),
                 edtDate->selectedDate().day());
  } else {
    date.setDate(edtDate->yearShown(),
                 edtDate->monthShown(),
                 1);
  }

  edtDate->setSelectedDate(date);
  setPending();
}

void
frmDateTime::enableRemoteSync(int state)
{
  edtRemoteUrl->setEnabled((state == Qt::Checked));
  lblRemoteUrl->setEnabled((state == Qt::Checked));
}
  
void 
frmDateTime::quit(void)
{
  update();

  if (!pending && !file->pendingChanges()) {
    QApplication::exit(0);
    return;
  }

  int choice = QMessageBox::question(this, 
      "Modifications non sauvées",
      "Certaines modifications n'ont pas été sauvegardées.\n"
      "Souhaitez-vous les sauver avant de quitter ?",
      "Sauver", "Ne pas sauver", "Annuler");
  if (choice == 2) // Cancel
    return;
  if (choice == 0) {
    if (!apply()) {
      int abort = QMessageBox::question(this,
        "Erreur de sauvegarde",
        "La sauvegarde des modifications a échoué. "
        "Souhaitez-vous quitter malgré tout ?",
        "Quitter", "Ne pas quitter");
      if (abort)
        return;
    }
  }
  QApplication::exit(0);
}

void
frmDateTime::update(void)
{
  file->setNameVal("USE_NTP", (chkRemoteSync->isChecked()) ? yes_str : no_str);
  file->setNameVal("NTP_SERVER", edtRemoteUrl->text());
}

bool
frmDateTime::apply(void)
{
  update();

  if (pending) {
    QDate date = edtDate->selectedDate();
    QTime time = edtTime->time();
    QString timestr = QString("%1%2%3.%4")
                      .arg(date.toString("MMdd")) // MMDD
                      .arg(time.toString("hhmm")) // hhmm
                      .arg(date.toString("yyyy")) // CCYY
                      .arg(time.toString("ss")); // .ss
    QStringList args(timestr);
    int ret = QProcess::execute("date", args);
    if (ret) {
      QMessageBox::warning(0, "Modification de l'heure impossible", 
              QString("La commande externe de modification de l'heure a échoué "
                      "(code de retour %1)").arg(ret), 
                      QMessageBox::Ok | QMessageBox::Default, 0, 0);
      return false;
    }
    pending = false;
  }

  if (file->pendingChanges())
    file->writeFile();
  return true;
}

void
frmDateTime::load(void)
{
  QString url = file->getNameVal("NTP_SERVER", any_rx);

  edtRemoteUrl->setText(url);
  chkRemoteSync->setChecked(!(file->getNameVal("USE_NTP", yes_rx).isEmpty()));
}

// vi:sw=2:ts=2:et:co=80:
