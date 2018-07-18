// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file DownloadSourcesFrame.h
 * clip-config single item in pkg table.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef _DOWNLOAD_SOURCES_FRAME_H
#define _DOWNLOAD_SOURCES_FRAME_H

#include <QFrame>

class QString;
class QCheckBox;
class QLabel;
class QLineEdit;

class DownloadSourcesFrame : public QFrame
{
  Q_OBJECT
 public:
  DownloadSourcesFrame(QWidget *p, const QString &jail, const QString &base);
  QString writeContent();
  void readContent(const QString &content);
  void setBase(const QString &newbase) { base = newbase; };
  bool pending() const { return changed; };

 private slots:
  void enableHTTPS(int state);
  void setPending();

 private:
  bool changed;

  QString jail;
  QString base;
  QString dist;

  QLabel *lbl;

  QCheckBox *chkHTTPS;
  QCheckBox *chkUSB;
  QCheckBox *chkCDROM;

  QLineEdit *edtURL;
};

#endif

// vi:sw=2:ts=2:et:co=80:
