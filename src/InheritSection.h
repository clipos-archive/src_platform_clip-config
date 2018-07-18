// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file InheritSection.h
 * clip-config container widget for a section that groups configuration
 * parameters that are defined in the same file
 * @author Hugo Chargois <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef INHERITSECTION_H
#define INHERITSECTION_H

#include <QWidget>

class QCheckBox;
class QLabel;
class QFrame;
class QLayout;
class ClipNetProfile;

class InheritSection : public QWidget
{
  Q_OBJECT
public:
  InheritSection(QString const& title, int file, QWidget *parent = 0);
  void setChildrenLayout(QLayout *layout);
  void update(ClipNetProfile *profile, bool enabled);

signals:
  void toggled(bool on, int file);

private slots:
  void checkBoxClicked(bool checked);

private:
  int file;
  QCheckBox *cb;
  QLabel *titleLabel;
  QFrame *frame;
  void setChildrenEnabled(bool enabled);
  void setChecked(bool checked);
};
#endif // INHERITSECTION_H

// vim:sw=2:ts=2:et:co=80:
