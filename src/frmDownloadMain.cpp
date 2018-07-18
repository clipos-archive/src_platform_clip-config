// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmDownloadMain.cpp
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QApplication>
#include <QFile>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QDesktopWidget>

#include "DownloadSourcesFrame.h"
#include "frmDownloadMain.h"
#include "qtcommon.h"
#include "consts.h"

static const QString 
  ClipSourcesFile("/sources.list.clip");
static const QString 
  RmhSourcesFile("/sources.list.rm_h");
static const QString 
  RmbSourcesFile("/sources.list.rm_b");

using namespace std;

frmDownloadMain::frmDownloadMain() 
  : QDialog(0)
{
  setWindowTitle("Gestion des sources de mises à jour");
  setWindowIcon(QIcon(ICONPATH"download-sources.png"));
  QRect screen = QApplication::desktop()->screenGeometry();

  if (screen.height() < 800) 
    setMinimumHeight(screen.height() - 150);
  else
    setMinimumHeight(550);

  setMinimumWidth(600);

  btnSave = new QPushButton(QIcon(ICONPATH"document-save.png"), "Sauver", this);
  btnQuit = new QPushButton(QIcon(ICONPATH"dialog-close.png"), "Quitter", this);

  srcCLIP = new DownloadSourcesFrame(this, "clip", 
                  (hasRMH || hasRMB) ? "clip4-rm-dpkg" : "clip4-gtw-dpkg");
  QFile src_clip(dlSourcesPath + ClipSourcesFile);
  if (src_clip.exists() 
          && src_clip.open(QIODevice::ReadOnly|QIODevice::Text)) {
    srcCLIP->readContent(src_clip.readAll());
    src_clip.close();
  }

  if (hasRMH) {
    srcRMH = new DownloadSourcesFrame(this, "rm_h", "clip4-rm-dpkg");
    QFile src_rmh(dlSourcesPath + RmhSourcesFile);
    if (src_rmh.exists() 
          && src_rmh.open(QIODevice::ReadOnly|QIODevice::Text)) {
      srcRMH->readContent(src_rmh.readAll());
      src_rmh.close();
    }
  } else {
    srcRMH = NULL;
  }
  if (hasRMB) {
    srcRMB = new DownloadSourcesFrame(this, "rm_b", "clip4-rm-dpkg");
    QFile src_rmb(dlSourcesPath + RmbSourcesFile);
    if (src_rmb.exists() 
          && src_rmb.open(QIODevice::ReadOnly|QIODevice::Text)) {
      srcRMB->readContent(src_rmb.readAll());
      src_rmb.close();
    }
  } else {
    srcRMB = NULL;
  }

  connect(btnSave, SIGNAL(clicked()), this, SLOT(save()));
  connect(btnQuit, SIGNAL(clicked()), this, SLOT(quit()));

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(3);

  vb->addWidget(srcCLIP);
  vb->addSpacing(5);

  if (hasRMH) {
    vb->addWidget(srcRMH);
    vb->addSpacing(5);
  }
  if (hasRMB) {
    vb->addWidget(srcRMB);
    vb->addSpacing(5);
  }

  vb->addStretch(3);

  QHBoxLayout *hb = new QHBoxLayout;
  hb->setSpacing(6);
  hb->addStretch(3);
  hb->addWidget(btnSave);
  hb->addSpacing(20);
  hb->addWidget(btnQuit);
  hb->addStretch(3);

  vb->addLayout(hb);
  vb->addSpacing(5);
}

bool
frmDownloadMain::pending(void)
{
  bool ret = srcCLIP->pending();

  if (hasRMH)
    ret |= srcRMH->pending();

  if (hasRMB)
    ret |= srcRMB->pending();

  return ret;
}

void 
frmDownloadMain::quit(void)
{
  if (!pending()) {
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
    if (!save()) {
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

bool
frmDownloadMain::save(void)
{
  bool ret = true;
  if (srcCLIP->pending()) {
    QFile src_clip(dlSourcesPath + ClipSourcesFile);
    if (src_clip.open(QIODevice::WriteOnly
                                |QIODevice::Text|QIODevice::Truncate)) {
      src_clip.write(srcCLIP->writeContent().toAscii());
      src_clip.putChar('\n');
      src_clip.close();
    } else {
      ret = false;
    }
  }

  if (hasRMH && srcRMH->pending()) {
    QFile src_rmh(dlSourcesPath + RmhSourcesFile);
    if (src_rmh.open(QIODevice::WriteOnly
                                |QIODevice::Text|QIODevice::Truncate)) {
      src_rmh.write(srcRMH->writeContent().toAscii());
      src_rmh.putChar('\n');
      src_rmh.close();
    } else {
      ret = false;
    }
  }

  if (hasRMB && srcRMB->pending()) {
    QFile src_rmb(dlSourcesPath + RmbSourcesFile);
    if (src_rmb.open(QIODevice::WriteOnly
                                |QIODevice::Text|QIODevice::Truncate)) {
      src_rmb.write(srcRMB->writeContent().toAscii());
      src_rmb.putChar('\n');
      src_rmb.close();
    } else {
      ret = false;
    }
  }
 
  return ret;
}

// vi:sw=2:ts=2:et:co=80:
