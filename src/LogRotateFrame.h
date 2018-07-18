// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file LogRotateFrame.h
 * clip-audit-config log rotation config frame header.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef _LOG_ROTATE_FRAME_H
#define _LOG_ROTATE_FRAME_H

#include <QFrame>

class QString;
class QSpinBox;
class QRadioButton;

class ClipConfigFile;

class LogRotateFrame : public QFrame
{
 public:
  LogRotateFrame(QWidget *p, const QString &path);
  bool pending();
  void save();

 private:
  bool changed;
  ClipConfigFile *file;

  QSpinBox *spbNumber;
  QSpinBox *spbSize;
  QSpinBox *spbDays;
  QRadioButton *rbtNumber;
  QRadioButton *rbtDays;

  void read(const QString &path);
  void update();
};

#endif // _LOG_ROTATE_FRAME_H

// vi:sw=2:ts=2:et:co=80:
