// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmWifiSelect.cpp
 * clip-config wireless network selection form.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QLayout>
#include <QScrollArea>
#include <QFile>
#include <QApplication>
#include <QLabel>
#include <QMessageBox>

#include "frmWifiSelect.h"
#include "qtcommon.h"
#include <ClipWidgets/WaitDialog.h>

static const QString g_WifiList(PREFIX"/var/wifiscan.txt");

frmWifiSelect::frmWifiSelect()
  : QDialog(0), essid(""), type(WifiEncNone)
{
  setWindowTitle("Sélection d'un réseau wifi");
  setWindowIcon(QIcon(ICONPATH"network-wireless.png"));

  QStringList args;

  QFile input(g_WifiList);
  if (input.open(QIODevice::ReadOnly)) {
    QString content(input.readAll());
    args = content.split('\n', QString::SkipEmptyParts);
  } else {
    QMessageBox::critical(0, "Erreur interne", 
        "Impossible de lire la liste des réseaux Wifi : " +
        qApp->translate("QFile", input.errorString().toStdString().c_str(), 
                      NULL, QCoreApplication::CodecForTr));
  }

  buttons = new QButtonGroup(this);
  buttons->setExclusive(true);
  connect(buttons, SIGNAL(buttonClicked(int)), this, SLOT(clicked(int)));

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(5);
  QScrollArea *sa = new QScrollArea(this);
  sa->setBackgroundRole(QPalette::Background);
  sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  int count = args.count() + 1;
  if (count > 1) {
    QWidget *gr = new QWidget;
    QGridLayout *grl = new QGridLayout;
    grl->setSpacing(6);

    QLabel *i = new QLabel("<b>Identifiant (ESSID)</b>", gr);
    i->setAlignment(Qt::AlignVCenter);
    grl->addWidget(i, 0, 0);

    QLabel *q = new QLabel("<b>Qualité de réception</b>", gr);
    q->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    grl->addWidget(q, 0, 1);
    
    QLabel *e = new QLabel("<b>Chiffrement</b>", gr);
    e->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    grl->addWidget(e, 0, 2);
    
    QLabel *m = new QLabel("<b>Mode</b>", gr);
    m->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    grl->addWidget(m, 0, 3);

    int num = 1;
    for (QStringList::Iterator it = args.begin(); 
                      it != args.end(); ++it, ++num) {
      QString str = *it;
      QString essid = str.section('"', 0, 0);
      if (essid == "??")
        essid = "<identifiant masqué>";
      QString qual = str.section('"', 1, 1);
      QString cipher = str.section('"', 2, 2);
      QString mode = str.section('"', 3, 3);
      QString encr;
      wifi_enc_t t;
      if (cipher == "none") {
        encr = "Aucun";
        t = WifiEncNone;
      } else {
        encr = cipher.toUpper();
        t = (cipher == "wep") ? 
          WifiEncWEP : WifiEncWPA;
      }
      QRadioButton *btn = new QRadioButton(essid, gr);
      buttons->addButton(btn, num);
      grl->addWidget(btn, num, 0);

      QLabel *q = new QLabel(qual, gr);
      q->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
      grl->addWidget(q, num, 1);
      
      QLabel *e = new QLabel(encr, gr);
      e->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
      grl->addWidget(e, num, 2);
      
      QLabel *m = new QLabel(mode, gr);
      m->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
      grl->addWidget(m, num, 3);
      
      encTypes.push_back(t);
      wifi_mode_t mm = WifiModeManaged;
      if (mode == "Ad-Hoc")
        mm = WifiModeAdHoc;
      modes.push_back(mm);
    }
    gr->setLayout(grl);
    sa->setWidget(gr);
  } else {
    QLabel *l = new QLabel("<i>Aucun réseau Wifi actif "
        "n'a été détecté.", this);
    l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    sa->setWidget(l);
  }

  vb->addWidget(sa, 1);
  vb->addSpacing(5);
  vb->addWidget(new QLabel("<b>Liste des réseaux détectés</b>", this));
  vb->addSpacing(5);
  vb->addWidget(new QLabel("<i>Sélectionnez un réseau dans la liste "
                                "pour utiliser ce réseau dans le profil "
                                "courant.</i>", this));
  vb->addSpacing(5);

  QHBoxLayout *hbb = new QHBoxLayout;
  sb = new QPushButton(QIcon(ICONPATH"dialog-ok.png"), "Accepter", this);
  sb->setEnabled(false);
  hbb->addWidget(sb);
  connect(sb, SIGNAL(clicked()), this, SLOT(save()));
  cb = new QPushButton(QIcon(ICONPATH"dialog-cancel.png"), "Annuler", this);
  hbb->addWidget(cb);
  connect(cb, SIGNAL(clicked()), this, SLOT(close()));
  vb->addLayout(hbb);

  //QRect g = this->geometry();
  //this->setGeometry(g.left(), g.top(), sv->contentsWidth() + 25, g.height());  
}

void 
frmWifiSelect::clicked(int id)
{
  sb->setEnabled(true);
}

void 
frmWifiSelect::save(void)
{
  QAbstractButton *selected = buttons->checkedButton();
  if (!selected) {
    done(0);
    return;
  }
  essid = selected->text();
  if (essid == "<identifiant masqué>") {
    QMessageBox::information(0, "Identifiant masqué",
            "Le réseau Wifi sélectionné masque son identifiant.\n"
            "Il sera nécessaire de saisir cet identifiant manuellement.",
            QMessageBox::Ok | QMessageBox::Default, 0, 0);
    essid = "??";
  }

  int num = buttons->checkedId() - 1;
  if ((unsigned)num >= encTypes.size()) {
    qDebug("num is too big");
    type = WifiEncNone;
  } else {
    type = encTypes[num];
  }
  if ((unsigned)num >= modes.size()) {
    qDebug("num is too big");
    mode = WifiModeManaged;
  } else {
    mode = modes[num];
  }
  done(0);
}

void 
frmWifiSelect::close(void)
{
  QAbstractButton *selected = buttons->checkedButton();
  if (!selected) {
    done(0);
    return;
  }
  int abort = QMessageBox::question(this, "Fermeture sans sauvegarde",
                    "Fermer la fenêtre de sélection sans sauvegarde ?",
                    "Fermer", "Ne pas fermer");
  if (!abort) {
    done(0);
    return;
  }
}

// vi:sw=2:ts=2:et:co=80:
