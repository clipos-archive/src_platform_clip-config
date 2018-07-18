// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file RoutesField.h
 * clip-config multiple routes field header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef ROUTESFIELD_H
#define ROUTESFIELD_H

#include <ClipWidgets/MultiField.h>

class RoutesField : public MultiField
{
  Q_OBJECT

 public:
  RoutesField(QWidget* parent, const QString& lbl);
  const QString returnValues() const;

 protected:
  void addValue(const QString& v);
};

#endif // ROUTESFIELD_H

// vi:sw=2:ts=2:et:co=80:
