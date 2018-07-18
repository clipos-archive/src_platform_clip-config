// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file InheritSection.cpp
 * clip-config container widget for a section that groups configuration
 * parameters that are defined in the same file
 * @author Hugo Chargois <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>

#include "ClipNetProfile.h"
#include "consts.h"
#include "InheritSection.h"

InheritSection::InheritSection(QString const& title, int file, QWidget *parent)
  : QWidget(parent), file(file)
{
  QVBoxLayout *vb = new QVBoxLayout(this);
  QHBoxLayout *hb = new QHBoxLayout();
  titleLabel = new QLabel("<b>" + title + "</b>", this);
  cb = new QCheckBox("Hériter du profil par défaut", this);

  hb->addWidget(titleLabel);
  hb->addWidget(cb);

  vb->addLayout(hb);
  frame = new QFrame(this);
  frame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
  vb->addWidget(frame);

  connect(cb, SIGNAL(clicked(bool)), this, SLOT(checkBoxClicked(bool)));
}

void
InheritSection::update(ClipNetProfile *profile, bool enabled)
{
  bool inherited = profile->isInherited(file);
  bool defaultProfile = profile->getProfileName() == defaultProfileName;

  /* the title is enabled iff the widget is enabled */
  titleLabel->setEnabled(enabled);

  /* the checkbox is checked iff the profile is inherited */
  cb->setCheckState(inherited ? Qt::Checked : Qt::Unchecked);

  /* the checkbox is enabled iff:
   * the profile is not the default profile
   * AND the widget is enabled */
  cb->setEnabled(!defaultProfile && enabled);

  /* the contents are enabled iff:
   * the profile is not inherited
   * AND the widget is enabled */
  setChildrenEnabled(!inherited && enabled);
}

void
InheritSection::setChildrenLayout(QLayout *layout)
{
  frame->setLayout(layout);
}

void
InheritSection::setChecked(bool checked)
{
  Qt::CheckState state = checked ? Qt::Checked : Qt::Unchecked;
  cb->setCheckState(state);
  setChildrenEnabled(!checked);
}

void
InheritSection::checkBoxClicked(bool checked)
{
  if (QMessageBox::question (0, "(Dés)hériter les paramètres ?",
           "Cette opération aura un effet immédiat et les éventuelles "
           "modifications non-sauvegardées seront perdues, "
           "voulez-vous continuer ?",
           QMessageBox::Yes,
           QMessageBox::No | QMessageBox::Default | QMessageBox::Escape
           ) == QMessageBox::Yes) {
    setChildrenEnabled(!checked);
    emit toggled(checked, file);
  } else {
    // reset in original state (before user clicked)
    this->setChecked(!checked);
  }
}

void
InheritSection::setChildrenEnabled(bool enabled)
{
  // Code stolen from QGroupBox
  QObjectList childList = frame->children();
  for (int i = 0; i < childList.size(); ++i) {
    QObject *o = childList.at(i);
    if (o->isWidgetType()) {
      QWidget *w = static_cast<QWidget *>(o);
      if (enabled) {
        if (!w->testAttribute(Qt::WA_ForceDisabled))
          w->setEnabled(true);
      } else {
        if (w->isEnabled()) {
          w->setEnabled(false);
          w->setAttribute(Qt::WA_ForceDisabled, false);
        }
      }
    }
  }
}

// vim:sw=2:ts=2:et:co=80:
