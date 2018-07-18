// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetJailTab.h
 * clip-config jail net config tab widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETJAILTAB_H
#define NETJAILTAB_H

#include <QString>

#include "NetTab.h"
#include "consts.h"

class IPField;
class ProxyWidget;
class QLabel;
class HostsWidget;
class TlsTunnelWidget;
class InheritSection;

class NetJailTab : public NetTab
{
  Q_OBJECT
 
 public:
  NetJailTab(QWidget *parent, ClipNetProfile *profile, 
          const QString &jail, netfile_t dns, netfile_t proxy = N_NETFILES,
          netfile_t hosts = N_NETFILES, netfile_t tls = N_NETFILES);

 public slots:
  bool saveCurrentProfile();
  bool loadCurrentProfile();
  void warnDhcp(bool on);
  void showAdvanced(bool on);

 private:
  QString jailName;
  netfile_t dnsFile;
  netfile_t proxyFile;
  netfile_t hostsFile;
  netfile_t tlsFile;

  IPField *dns1;
  IPField *dns2;
  ProxyWidget *proxy;
  TlsTunnelWidget *tlsWidget;
  HostsWidget *hostsWidget;

  QLabel *dhcpWarn;

  InheritSection *sectionDns;
  InheritSection *sectionProxy;
  InheritSection *sectionTls;
  InheritSection *sectionHosts;
};

#endif // NETJAILTAB_H

// vi:sw=2:ts=2:et:co=80:
