// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PkgTable.cpp
 * clip-config package table.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI/DCSSI
 * Copyright (C) 2010-2012 SGDSN/SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QLabel>
#include <QStringList>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <iostream>

#include "PkgTableItem.h"
#include "PkgTable.h"
#include "qtcommon.h"
#include <ClipWidgets/WaitDialog.h>

static const QString g_PkgListCommand(PREFIX"/bin/list_optional.pl");

PkgTable::PkgTable(QWidget *parent, const QString &jail, 
                  const QString &out, const QString &in)
  : QWidget(parent), outpath(out), inpath(in), jail(jail)
{
  gl = new QGridLayout(this);
  gl->setHorizontalSpacing(6);
  gl->setVerticalSpacing(6);
  gl->setColumnMinimumWidth(2, 450);
  gl->setColumnStretch(2, 0);

  init();
}

bool 
PkgTable::runHelper(void)
{
  QStringList args;
  args << "-j" << jail;
  if (QFile::exists(inpath)) {
    args << "-cache" << inpath; 
  } else {
    qDebug("No clip_download cache for jail %s, using temporary cache", 
              jail.toStdString().c_str());
    // Cache was not created by clip_download, and we cannot write to it
    // => use a temporary, writable cache instead.
    QString tmppath = QString("/tmp/clip-config-%1.cache").arg(jail);
    args << "-cache" << tmppath;
  }
  QProcess *lister = new QProcess();
  lister->setReadChannel(QProcess::StandardOutput);

  lister->start(g_PkgListCommand, args);

  WaitDialog waitForm("Traitement en cours", 
    "<p>Création de la liste des paquetages optionnels pour le " 
    + jailName(jail) + " en cours...</p>", *lister);
  waitForm.exec ();

  QString plist = QString(lister->readAll());

  return readList(plist);
}

bool 
PkgTable::readList(const QString &plist)
{
  QStringList _pkgs = plist.split('\n', QString::SkipEmptyParts);
  nrows = _pkgs.count();
  if (nrows) {
    int i = 0;
    for (QStringList::Iterator it = _pkgs.begin(); it != _pkgs.end() ; ++it) {
      QString str = *it;
      if (str.startsWith('*')) {
        str = str.right(str.length() - 1);
        QLabel *l = new QLabel;
        l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        l->setText(QString("<b><u>") + str + "</u></b>");
        gl->addWidget(l, i, 0, 1, 3);
        i++;
        gl->addWidget(new QLabel("<b><font color=gray>Paquetage</font></b>"), 
                              i, 0, 1, 2);
        gl->addWidget(new 
          QLabel("<b><i><font color=gray>Description</font></i></b>"), i, 2);
      } else {
        bool b = (str.at(0) == '+');
        str = str.right(str.length() - 1);
        QString pname = str.section(' ', 0, 0);
        QString pdesc = str.section(' ', 1);
        PkgTableItem *it = new PkgTableItem(0, pname, b);
        gl->addWidget(it, i, 0);
        pkgs.push_front(it);
        gl->addItem(new QSpacerItem(5, 0), i, 1);
        QLabel *l = new QLabel(QString("<i>") + pdesc + "</i>");
        l->setWordWrap(true);
        gl->addWidget(l, i, 2);
      }
      i++;
    }
    gl->setColumnStretch(0, 1);
    gl->setColumnStretch(1, 1);
    gl->setColumnStretch(2, 3);
  } else {
    gl->addWidget(new QLabel("<i>Aucun paquetage optionnel n'est "
                         "disponible pour le " + jailName(jail) 
                         + ".</i>", this));
  }

  return true;
}

void
PkgTable::init(void)
{
  QLayoutItem *it;
  QWidget *widget;
  while ((it = gl->takeAt(0)) != 0) {
    widget = it->widget();
    delete it;
    delete widget;
  }
  pkgs.clear();

  runHelper(); 
}

bool 
PkgTable::save(void)
{
  QString toSave;
  for (QLinkedList<PkgTableItem *>::const_iterator it  = pkgs.constBegin(); 
                        it != pkgs.constEnd(); ++it) {
    if ((*it)->set()) {
      toSave.append((*it)->getPkgName());
      toSave.append("\n");
    }
  }
  QFile f(outpath);
  if (!f.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
    qDebug("Failed to open output");
    QMessageBox::critical(this, "Erreur de sauvegarde",
      QString("Impossible d'ouvrir le fichier ") + outpath 
        + " en écriture: " + f.errorString(),
      QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }

  qint64 wret = f.write(toSave.toAscii());
  if (wret == -1) {
    qDebug("Failed to write output");
    QMessageBox::critical(this, "Erreur de sauvegarde",
      QString("Impossible d'écrire dans le fichier ") 
        + outpath + ": " + f.errorString(),
      QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }
  
  if (wret != (qint64)toSave.length()) {
    qDebug("Short write !");
    QMessageBox::critical(this, "Erreur de sauvegarde",
      QString("Ecriture incomplète dans le fichier ")
        + outpath, 
      QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }

  f.close();

  for (QLinkedList<PkgTableItem *>::iterator it  = pkgs.begin(); 
      it != pkgs.end(); ++it)
    (*it)->saved();

  return true;
}

bool 
PkgTable::pending(void)
{
  for (QLinkedList<PkgTableItem *>::const_iterator it  = pkgs.constBegin(); 
                        it != pkgs.constEnd(); ++it) {
    if ((*it)->pending())
      return true;
  }
  return false;
}

// vi:sw=2:ts=2:et:co=80:
