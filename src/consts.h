// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file consts.h
 * clip-config constants header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010-2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef COMMON_H
#define COMMON_H

#include <QRegExp>
#include <QString>


// frmNetMain
#define D_MANUAL 0
#define D_DHCP 1

#define I_NONE    0x01
#define I_WIRED   0x02
#define I_WIFI    0x04
#define I_UMTS    0x08

#define I_NODOWNLOAD  (I_NONE|I_UMTS)
#define I_NOADDR      (I_NONE|I_UMTS)

#define W_NOENCRYPTION 0
#define W_WEP 1
#define W_WPA 2

#define W_MODE_MANAGED 0
#define W_MODE_ADHOC 1

#define W_WPA_PSK 0
#define W_WPA_EAP 1

#define WEP_KEY 0
#define WEP_PASSPHRASE 1

#define NF_FTP    0x01
#define NF_PING   0x02

#define NF_SNAT   0x10

// frmNetAdd
#define ADD_CANCEL 0x00
#define ADD_OK 0x01

#define ADD_DONTSAVE 0x00
#define ADD_SAVE_BEFORE 0x02


// ClipNetProfile
typedef enum {
  CLIP_NET = 0,
  CLIP_IPSEC = 1,
  CLIP_UMTS = 2,
  CLIP_WIRELESS = 3,
  CLIP_NETFILTER = 4,
  CLIP_HOSTNAME = 5,
  CLIP_RESOLVCONF = 6,
  CLIP_HOSTS = 7,
  N_CLIP_NETFILES = 8,
  RMH_RESOLVCONF = 8,
  RMH_HOSTS = 9,
  RMH_PROXY = 10,
  RMH_TLS = 11,
  RMB_RESOLVCONF = 12,
  RMB_HOSTS = 13,
  RMB_PROXY = 14,
  RMB_TLS = 15,

  N_NETFILES = 16
} netfile_t;

static const char filenames[N_NETFILES][30] = {
  "net",
  "ipsec",
  "umts",
  "wireless",
  "netfilter",
  "hostname",
  "resolv.conf",
  "hosts",
  "rm_h/resolv.conf",
  "rm_h/hosts",
  "rm_h/proxy",
  "rm_h/tls_tunnels",
  "rm_b/resolv.conf",
  "rm_b/hosts",
  "rm_b/proxy",
  "rm_b/tls_tunnels",
};

extern QString devicesPath;
extern QString smartcardsPath;
extern QString ntpPath;
extern QString dlSourcesPath;

extern QString netdSocketPath;

extern QString netConstPath;

extern QString netconfdParentDir;
extern const QString netconfLink;
extern const QString rmhSubDir;
extern const QString rmbSubDir;
extern const QString wiredMacFile;

extern const QString rmhFlagFile;
extern const QString rmbFlagFile;
extern const QString eth0FlagFile;
extern const QString eth1FlagFile;
extern const QString eth2FlagFile;


// RegExps 
extern const QRegExp profileName_rx;
extern const QString defaultProfileName;
extern const QRegExp defaultProfile_rx;

extern const QRegExp varname_rx;
extern const QString ns_str;

extern const QString yes_str;
extern const QRegExp yes_rx;
extern const QString no_str;

extern const QString dhcp_str;
extern const QRegExp dhcp_rx;
extern const QString virtual_str;
extern const QRegExp virtual_rx;
extern const QRegExp ip_rx;
extern const QRegExp multiip_rx;
extern const QRegExp ports_rx;
extern const QRegExp port_rx;
extern const QRegExp mask_rx;
extern const QRegExp addr_rx;
extern const QRegExp multiaddrs_rx;
extern const QRegExp route_rx;
extern const QRegExp routes_rx;
extern const QRegExp host_rx;
extern const QRegExp mtu_rx;

extern const QString managed_str;
extern const QString adhoc_str;
extern const QRegExp adhoc_rx;
extern const QString wep_str;
extern const QRegExp wep_rx;
extern const QString wpa_str;
extern const QRegExp wpa_rx;
extern const QRegExp wepkey_rx;
extern const QRegExp wepkey_hexakey_rx;
extern const QRegExp wepkey_passphrase_rx;
extern const QString wepkey_passphrase_prefix_str;
extern const QRegExp wpakey_rx;
extern const QString wpamgmt_psk_str;
extern const QString wpamgmt_eap_str;
extern const QRegExp wpamgmt_rx;


extern const QRegExp any_rx;
extern const QRegExp sim_pin_rx;
extern const QRegExp sc_pin_rx;

extern const QRegExp tls_tunnel_name_rx;

extern QString userdSocketPath;

extern const QRegExp username_rx;


#endif // COMMON_H

// vi:sw=2:ts=2:et:co=80:
