// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file DownloadSourcesFrame.cpp
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QLayout>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QRegExpValidator>
#include <QStringList>

#include "DownloadSourcesFrame.h"
#include "consts.h"

DownloadSourcesFrame::DownloadSourcesFrame(QWidget *parent, 
    const QString &jail, const QString &base)
  : QFrame(parent), changed(false), jail(jail), base(base)
{
  QString jailname;
  if (jail == "clip") {
    dist = "clip";
    jailname = "le socle CLIP";
  } else if (jail == "rm_h") {
    dist = "rm";
    jailname = "le niveau Haut";
  } else if (jail == "rm_b") {
    dist = "rm";
    jailname = "le niveau Bas";
  } else {
    dist = "rm";
    jailname = jail.toUpper();
  }

  lbl = new QLabel(QString("<b><u>Sources de téléchargement pour %1</u></b>")
                  .arg(jailname), this);
  chkHTTPS = new QCheckBox("Réseau (HTTPS)");
  chkUSB = new QCheckBox("Support USB");
  chkCDROM = new QCheckBox("CD-ROM");

  edtURL = new QLineEdit("", this);
  edtURL->setValidator(new QRegExpValidator(host_rx, this));
  edtURL->setMinimumWidth(350);

  connect(chkHTTPS, SIGNAL(stateChanged(int)), this, SLOT(enableHTTPS(int)));
  connect(chkHTTPS, SIGNAL(stateChanged(int)), this, SLOT(setPending()));
  connect(chkUSB, SIGNAL(stateChanged(int)), this, SLOT(setPending()));
  connect(chkCDROM, SIGNAL(stateChanged(int)), this, SLOT(setPending()));
  connect(edtURL, SIGNAL(textChanged(const QString &)), this, SLOT(setPending()));

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(3);
  vb->addWidget(lbl);
  vb->addSpacing(3);

  vb->addWidget(chkHTTPS);
  vb->addSpacing(3);

  QHBoxLayout *hb = new QHBoxLayout;
  hb->addSpacing(20);
  hb->addWidget(new QLabel("URL du serveur :", this));
  hb->addSpacing(5);
  hb->addWidget(edtURL);
  hb->addStretch(5);

  vb->addLayout(hb);
  vb->addSpacing(3);
  vb->addWidget(chkUSB);
  vb->addSpacing(3);
  vb->addWidget(chkCDROM);
}

void 
DownloadSourcesFrame::enableHTTPS(int state)
{
  edtURL->setEnabled((state == Qt::Checked));
}

QString
DownloadSourcesFrame::writeContent()
{
  QStringList list;

  if (jail == "rm_h")
    list << "deb [local=true] copy:///var/pkg/rm_b/mirrors main rm";
  else if (jail == "rm_b")
    list << "deb [local=true] copy:///var/pkg/rm_h/mirrors main rm";

  if (chkHTTPS->isChecked() && edtURL->hasAcceptableInput()) {
    QString url = edtURL->text();

    if (!url.startsWith("https://"))
      url.prepend("https://");
    
    list << QString("deb    %1/mirrors/%2/%3/%3-core-conf %3 main")
              .arg(url).arg(base).arg(dist);
    list << QString("deb    %1/mirrors/%2/%3/%3-apps-conf %3 main")
              .arg(url).arg(base).arg(dist);
  }

  if (chkUSB->isChecked()) {
    list << QString("deb    copy:///mnt/usb/mirrors/%1/%2/%2-core-conf %2 main")
              .arg(base).arg(dist);
    list << QString("deb    copy:///mnt/usb/mirrors/%1/%2/%2-apps-conf %2 main")
              .arg(base).arg(dist);
  }
  if (chkCDROM->isChecked()) {
    list << QString("deb    "
              "copy:///mnt/cdrom/mirrors/%1/%2/%2-core-conf %2 main")
              .arg(base).arg(dist);
    list << QString("deb    "
              "copy:///mnt/cdrom/mirrors/%1/%2/%2-apps-conf %2 main")
              .arg(base).arg(dist);
  }

  changed = false;
  return list.join("\n\n");
}

void
DownloadSourcesFrame::readContent(const QString &content)
{
  QStringList list = content.split("\n");

  chkHTTPS->setChecked(false);
  chkUSB->setChecked(false);
  chkCDROM->setChecked(false);

  for (QStringList::const_iterator i = list.constBegin(); 
                    i != list.constEnd(); i++) {
    if (!(*i).startsWith("deb"))
      continue;
    if ((*i).contains("copy:///mnt/usb")) {
      chkUSB->setChecked(true);
      continue;
    }

    if ((*i).contains("copy:///mnt/cdrom")) {
      chkCDROM->setChecked(true);
      continue;
    }

    if ((*i).contains("https://")) {
      QString url = (*i).mid((*i).indexOf("https://"));
      url.truncate(url.indexOf("/mirrors"));
      edtURL->setText(url);
      chkHTTPS->setChecked(true);
    }
  }

  changed = false;
}

void
DownloadSourcesFrame::setPending()
{
  changed = true;
}

// vi:sw=2:ts=2:et:co=80:
