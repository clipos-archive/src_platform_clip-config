// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetTab.cpp
 * clip-config net config tab widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */


#include "NetTab.h"
#include "ClipNetProfile.h"

NetTab::NetTab(QWidget *parent, ClipNetProfile *profile)
  : QFrame(parent), pending(false), advanced(false), profile(profile)
{ }

bool
NetTab::changesPending()
{
  return pending;
}

void
NetTab::showAdvanced(bool on)
{
  advanced = on;
}

void
NetTab::setProfile(ClipNetProfile *prof)
{
  profile = prof;
}


// vi:sw=2:ts=2:et:co=80:
