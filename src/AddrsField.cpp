// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file AddrsField.cpp
 * clip-config multiple IP address field.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "AddrsField.h"
#include "consts.h"

AddrsField::AddrsField(QWidget* parent, const QString& lbl, bool withMask)
  : MultiField(parent, lbl, 
      (withMask) ? addr_rx : ip_rx, " ", 150, 60, true)
{ }

// vi:sw=2:ts=2:et:co=80:
