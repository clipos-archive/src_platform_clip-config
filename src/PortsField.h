// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PortsField.cpp
 * clip-config multiple ports field.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef PORTSFIELD_H
#define PORTSFIELD_H

#include <ClipWidgets/MultiField.h>

class PortsField : public MultiField
{
  Q_OBJECT

 public:
  PortsField(QWidget *parent, const QString& lbl);

 protected:
  void addValue(const QString& p);
};

#endif // PORTSFIELD_H

// vi:sw=2:ts=2:et:co=80:
