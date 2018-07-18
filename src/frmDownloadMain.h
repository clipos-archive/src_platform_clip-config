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

#ifndef FRMDOWNLOADMAIN_H
#define FRMDOWNLOADMAIN_H


#include <QDialog>

class DownloadSourcesFrame;
class QPushButton;

class frmDownloadMain : public QDialog
{
  Q_OBJECT

 public:
  frmDownloadMain ();
 
 private slots:
  void quit(void);
  bool save(void);

 private:
  bool pending();

  QPushButton *btnQuit;
  QPushButton *btnSave;

  DownloadSourcesFrame *srcCLIP;
  DownloadSourcesFrame *srcRMH;
  DownloadSourcesFrame *srcRMB;
};

#endif // FRMDOWNLOADMAIN_H

// vi:sw=2:ts=2:et:co=80:
