// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file qtcommon.h
 * clip-config common functions header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef QTCOMMON_H
#define QTCOMMON_H

class QString;

void fatalError(const QString &err);
QString jailName(const QString &jail);

extern bool hasRMH;
extern bool hasRMB;

#define ICONPATH PREFIX"/share/icons/"


#endif // QTCOMMON_H

// vi:sw=2:ts=2:et:co=80:
