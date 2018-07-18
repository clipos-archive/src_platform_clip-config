// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file ClipNetProfile.h
 * clip-config net profile header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef CLIPNETPROFILE_H
#define CLIPNETPROFILE_H

#include "consts.h"
#include <ClipConfig/SimpleConfigFile.h>
#include <ClipConfig/ClipConfigFile.h>
#include <ClipConfig/ClipProfile.h>
#include <sys/stat.h>
#include "ResolvConfFile.h"

class ClipNetProfile : public ClipProfile
{
 public:
  ClipNetProfile(const QString& profileName, bool withWifi);
  virtual ~ClipNetProfile() {};
  bool createSubDirs();
  bool removeSubDirs();
  void writeProfileAs(const QString& clone);
  void inheritProfileAs(const QString& clone);
  void inheritFile(int file, const QString &parent);
  bool isInherited(int file);

 private:
  bool wifiPresent;
  ConfigFile* newFile(netfile_t num);
};

#endif // CLIPNETPROFILE_H

// vi:sw=2:ts=2:et:co=80: 
