// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file YesNoField.cpp
 * clip-config simple config var yes/no checkbox widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */


#include "YesNoField.h"
#include "ClipNetProfile.h"

YesNoField::YesNoField(const QString &text, QWidget *parent, 
        netfile_t id, const QString &name, bool advanced)
  : QCheckBox(text, parent), fileId(id), varname(name), advanced(advanced)
{ 
  if (advanced)
    setEnabled(false);
}

void
YesNoField::showAdvanced(bool on)
{
  if (advanced)
    setEnabled(on);
}

bool
YesNoField::saveProfile(ClipNetProfile *profile)
{
  profile->setNameVal(fileId, varname, isChecked() ? yes_str : no_str);
  return true;
}

bool
YesNoField::loadProfile(ClipNetProfile *profile)
{
  if (profile->getNameVal(fileId, varname, yes_rx).isEmpty())
    setChecked(false);
  else
    setChecked(true);
  return true;
}


// vi:sw=2:ts=2:et:co=80:
