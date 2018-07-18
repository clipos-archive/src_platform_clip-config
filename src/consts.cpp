// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file consts.cpp
 * clip-config constants
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "consts.h"

QString dlSourcesPath("/etc/admin/clip_download");
QString devicesPath("/etc/admin/conf.d/devices");
QString smartcardsPath("/etc/admin/conf.d/smartcards");
QString ntpPath("/etc/admin/conf.d/ntp");

// Net

QString netdSocketPath(NETD_SOCKET);

QString netConstPath("/etc/admin/conf.d/net");

QString netconfdParentDir(NETCONFD_PATH);
const QString netconfLink(NETCONF_LINK);
const QString rmhSubDir("rm_h");
const QString rmbSubDir("rm_b");
const QString wiredMacFile("/var/run/wired_mac");

const QString rmhFlagFile("/var/run/has_jail_rm_h");
const QString rmbFlagFile("/var/run/has_jail_rm_b");
const QString eth0FlagFile("/var/run/has_if_eth0");
const QString eth1FlagFile("/var/run/has_if_eth1");
const QString eth2FlagFile("/var/run/has_if_eth2");


const QRegExp profileName_rx("[0-9a-zA-Z_\\-]+");
const QString defaultProfileName(DEFAULT_PROFILENAME);
const QRegExp defaultProfile_rx("^" DEFAULT_PROFILENAME "$");

const QRegExp varname_rx("\\w+");
const QString ns_str("nameserver");

const QString yes_str("yes");
const QRegExp yes_rx("^yes$");
const QString no_str("no");

const QString dhcp_str("dhcp");
const QRegExp dhcp_rx("^dhcp$");
const QString virtual_str("virtual");
const QRegExp virtual_rx("^virtual$");
const QRegExp ip_rx("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
const QRegExp multiip_rx("(?:\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}"
          " )*\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
const QRegExp port_rx("([0-9]*:[0-9]*|[0-9]+|)");
const QRegExp ports_rx("[0-9,:]+");
const QRegExp mask_rx("\\d{1,2}");
const QRegExp addr_rx("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/\\d{1,2}");
const QRegExp multiaddrs_rx("(?:\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/"
          "\\d{1,2} )*\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/\\d{1,2}");
const QRegExp route_rx ("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/"
          "\\d{1,2}:\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
const QRegExp routes_rx("(?:\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/"
          "\\d{1,2}:\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3} )*"
				  "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/\\d{1,2}:"
          "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
const QRegExp host_rx("[a-zA-Z0-9./:_,\\-]+");
const QRegExp mtu_rx("1?\\d{1,3}");

const QString managed_str("Managed");
const QString adhoc_str("Ad-Hoc");
const QRegExp adhoc_rx("^Ad-Hoc$");
const QString wep_str("wep");
const QRegExp wep_rx("^wep$");
const QString wpa_str("wpa");
const QRegExp wpa_rx("^wpa$");
const QRegExp wepkey_rx("^([0-9a-fA-F]{10}|[0-9a-fA-F]{26}|\\S+)$");
const QRegExp wepkey_hexakey_rx("^([0-9a-fA-F]{10}|[0-9a-fA-F]{26})$");
const QRegExp wepkey_passphrase_rx("^s:\\S+$");
const QString wepkey_passphrase_prefix_str("s:");
const QRegExp wpakey_rx("^([0-9a-fA-F]{64}|.{8,63})$");
const QString wpamgmt_psk_str("WPA-PSK");
const QString wpamgmt_eap_str("WPA-EAP");
const QRegExp wpamgmt_rx("WPA-PSK|WPA-EAP");

const QRegExp sim_pin_rx("^[0-9]{4,8}$");
const QRegExp any_rx(".+");
const QRegExp sc_pin_rx(".{4,16}");

const QRegExp tls_tunnel_name_rx("[a-zA-Z0-9_\\-]+");

// Users

QString userdSocketPath(USERD_SOCKET);

const QRegExp username_rx("[a-z_][0-9a-z_\\-]{0,31}");

// vi:sw=2:ts=2:et:co=80:
