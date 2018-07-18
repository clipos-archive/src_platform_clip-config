// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file AddrsField.h
 * clip-config multiple IP address field header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef ADDRSFIELD_H
#define ADDRSFIELD_H

#include <ClipWidgets/MultiField.h>

class AddrsField : public MultiField
{
  Q_OBJECT

 public:
  AddrsField(QWidget* parent, const QString& lbl, bool withMask = true);
};

#endif // ADDRSFIELD_H

// vi:sw=2:ts=2:et:co=80:
