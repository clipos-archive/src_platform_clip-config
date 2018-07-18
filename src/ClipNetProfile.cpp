// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file ClipNetProfile.cpp
 * clip-config net profile. 
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "consts.h"
#include "qtcommon.h"
#include "ClipNetProfile.h"
#include <ClipConfig/CCException.h>

#include <QDir>

ConfigFile *
ClipNetProfile::newFile(netfile_t num) 
{
  ConfigFile *f;
  bool worldReadable = false;

  switch (num) {
    case RMH_HOSTS:
    case RMB_HOSTS:
    case RMH_TLS:
    case RMB_TLS:
    case CLIP_HOSTNAME:
    case CLIP_HOSTS:
      worldReadable = true;
      f = new SimpleConfigFile();
      break;

    case RMH_PROXY:
    case RMB_PROXY:
      worldReadable = true;
    case CLIP_NET:
    case CLIP_IPSEC:
    case CLIP_WIRELESS:
    case CLIP_UMTS:
    case CLIP_NETFILTER:
      f = new ClipConfigFile();
      break;

    case RMH_RESOLVCONF:
    case RMB_RESOLVCONF:
    case CLIP_RESOLVCONF:
      worldReadable = true;
      f = new ResolvConfFile();
      break;

    default:
      throw CCException(QString("ClipNetProfile had to deal with "
                                "an unexpected file number: %1").arg(num));
  }

  if (!addFile(num, f))
      throw CCException(QString("ClipNetProfile already has a file at index "
                                " %1.").arg(num));
  if (worldReadable) {
    f->setPublic();
  }
  return f;
}

ClipNetProfile::ClipNetProfile(const QString& profileName, bool withWifi)
  : ClipProfile(profileName, netconfdParentDir), wifiPresent(withWifi)
{
  ConfigFile *f;
  int max = N_CLIP_NETFILES;
  if (hasRMH || hasRMB) 
    max = N_NETFILES;
  for (int i = 0; i < max; i++) {
    f = newFile((netfile_t)i);
    f->readFile(path + "/" + filenames[i]);
  }
}

bool ClipNetProfile::createSubDirs()
{
  QDir dir;
  return ((dir.exists(path + "/" + rmhSubDir) 
              || dir.mkdir(path + "/" + rmhSubDir)) && 
          (dir.exists(path + "/" + rmbSubDir) 
              || dir.mkdir(path + "/" + rmbSubDir)));
}

bool ClipNetProfile::removeSubDirs()
{
  QDir dir;
  return (dir.rmdir(path + "/" + rmhSubDir) 
            && dir.rmdir(path + "/" + rmbSubDir));
}

void ClipNetProfile::writeProfileAs(const QString &clone)
{
  mode_t mask;
  mask = umask(0027);
  ClipProfile::writeProfileAs(clone, netconfdParentDir);
  umask(mask);
}

void ClipNetProfile::inheritProfileAs(const QString &clone)
{
  ClipProfile::inheritProfileAs(clone, netconfdParentDir);
}

void ClipNetProfile::inheritFile(int file, const QString &parent)
{
  ClipProfile::inheritFile(file, netconfdParentDir + "/" + parent);
}

bool ClipNetProfile::isInherited(int file)
{
  return !ClipProfile::inheritsFrom(file).isEmpty();
}

// vi:sw=2:ts=2:et:co=80: 
