// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetJailTab.cpp
 * clip-config jail net config tab widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QFont>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>

#include "consts.h"
#include "HostsWidget.h"
#include "ProxyWidget.h"
#include "TlsTunnelWidget.h"
#include <ClipWidgets/IPField.h>
#include <ClipWidgets/LabelledField.h>
#include "ClipNetProfile.h"
#include "InheritSection.h"
#include "NetJailTab.h"

NetJailTab::NetJailTab(QWidget *parent, ClipNetProfile *profile, 
               const QString &jail, netfile_t dns, netfile_t prox,
               netfile_t hosts, netfile_t tls)
  : jailName(jail), dnsFile(dns), proxyFile(prox), hostsFile(hosts),
  tlsFile(tls)
{
  QVBoxLayout *sections = new QVBoxLayout(this);

  dns1 = new IPField(this, "Serveur DNS primaire");
  dns2 = new IPField(this, "Serveur DNS secondaire");

  dhcpWarn = new QLabel("", this);
  QFont font = dhcpWarn->font();
  font.setItalic(true);
  dhcpWarn->setFont(font);

  QVBoxLayout *vb = new QVBoxLayout();
  vb->addWidget(dns1);
  vb->addWidget(dns2);
  vb->addWidget(new LabelledField(this, dhcpWarn, 10));
  
  sectionDns = new InheritSection("Configuration DNS", dns, this);
  sectionDns->setChildrenLayout(vb);
  sections->addWidget(sectionDns);
  connect(sectionDns, SIGNAL(toggled(bool, int)),
      parent, SLOT(inheritSectionToggled(bool, int)));

  if (prox == N_NETFILES) {
    proxy = NULL;
  } else {
    vb = new QVBoxLayout();
    proxy = new ProxyWidget(this);
    vb->addWidget(proxy);

    sectionProxy = new InheritSection("Configuration des proxy", prox, this);
    sectionProxy->setChildrenLayout(vb);
    sections->addWidget(sectionProxy);
    connect(sectionProxy, SIGNAL(toggled(bool, int)),
        parent, SLOT(inheritSectionToggled(bool, int)));
  }

  if (hosts == N_NETFILES) {
    hostsWidget = NULL;
  } else {
    vb = new QVBoxLayout();
    hostsWidget = new HostsWidget(this);
    vb->addWidget(hostsWidget);

    sectionHosts = new InheritSection("Configuration des noms d'hôtes", hosts, this);
    sectionHosts->setChildrenLayout(vb);
    sections->addWidget(sectionHosts);
    connect(sectionHosts, SIGNAL(toggled(bool, int)),
        parent, SLOT(inheritSectionToggled(bool, int)));
  }

  if (tls == N_NETFILES) {
    tlsWidget = NULL;
  } else {
    vb = new QVBoxLayout();
    tlsWidget = new TlsTunnelWidget(this);
    vb->addWidget(tlsWidget);

    sectionTls = new InheritSection("Configuration des tunnels TLS", tls, this);
    sectionTls->setChildrenLayout(vb);
    sections->addWidget(sectionTls);
    connect(sectionTls, SIGNAL(toggled(bool, int)),
        parent, SLOT(inheritSectionToggled(bool, int)));
  }
}


bool 
NetJailTab::saveCurrentProfile()
{
  profile->setIndexVal(dnsFile, 1, dns1->returnIP());
  profile->setIndexVal(dnsFile, 2, dns2->returnIP());

  if (proxy) {
    profile->setNameVal(proxyFile, "HTTP_PROXY", proxy->getNameValue(PROXY_HTTP));
    profile->setNameVal(proxyFile, "HTTPS_PROXY", proxy->getNameValue(PROXY_HTTPS));
    profile->setNameVal(proxyFile, "FTP_PROXY", proxy->getNameValue(PROXY_FTP));
    profile->setNameVal(proxyFile, "NO_PROXY", proxy->getNameValue(PROXY_NOPROXY));
    
    if (proxy->isChanged()) {
      QMessageBox::information(0, "Changement de configuration proxy",
      "La configuration des proxys a été modifiée pour la cage " 
      + jailName.toUpper() + ",\n"
      "la session devra être redémarrée dans cette cage pour prendre\n"
      "en compte le changement après application du profil.",
      QMessageBox::Ok);
    }

    proxy->setChanged(false);
  }

  if (tlsWidget) {
    profile->setVal(tlsFile, tlsWidget->getTunnels());
  }

  if (hostsWidget) {
    profile->setVal(hostsFile, hostsWidget->getHosts());
  }

  return true;
}

bool
NetJailTab::loadCurrentProfile()
{
  dns1->setNameValue(profile->getIndexVal(dnsFile, 1));
  dns2->setNameValue(profile->getIndexVal(dnsFile, 2));
  sectionDns->update(profile, true);
  
  if (proxy){
    proxy->setNameValue(PROXY_HTTP, 
        profile->getNameVal(proxyFile, "HTTP_PROXY", host_rx));
    proxy->setNameValue(PROXY_HTTPS, 
        profile->getNameVal(proxyFile, "HTTPS_PROXY", host_rx));
    proxy->setNameValue(PROXY_FTP, 
        profile->getNameVal(proxyFile, "FTP_PROXY", host_rx));
    proxy->setNameValue(PROXY_NOPROXY, 
        profile->getNameVal(proxyFile, "NO_PROXY", host_rx));

    proxy->setChanged(false);
    sectionProxy->update(profile, true);
  }

  if (tlsWidget) {
    tlsWidget->setTunnels(profile->getVal(tlsFile));
    sectionTls->update(profile, advanced);
  }

  if (hostsWidget) {
    hostsWidget->setHosts(profile->getVal(hostsFile));
    sectionHosts->update(profile, advanced);
  }

  return true;
}

void
NetJailTab::warnDhcp(bool on)
{
  if (on)
    dhcpWarn->setText("N.B. : ces paramètres vont remplacer ceux obtenus par DHCP");
  else
    dhcpWarn->setText("");
}

void
NetJailTab::showAdvanced(bool on)
{
  advanced = on;
  if (tlsWidget)
    sectionTls->update(profile, on);
  if (hostsWidget)
    sectionHosts->update(profile, on);
}

// vi:sw=2:ts=2:et:co=80:
