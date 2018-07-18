// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetAddrTab.cpp
 * clip-config net core addresses config tab widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QButtonGroup>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QRadioButton>

#include "NetAddrTab.h"
#include <ClipWidgets/IPField.h>
#include "RoutesField.h"
#include "HostsWidget.h"
#include "InheritSection.h"
#include "ClipNetProfile.h"
#include "consts.h"


NetAddrTab::NetAddrTab(QWidget *parent, 
                  ClipNetProfile *profile, int num)
  : NetTab(parent, profile), numIfs(num)
{ 
  staticOrDhcp = new QButtonGroup();
  staticOrDhcp->setExclusive(true);

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(4);

  vb->addWidget(new QLabel( (numIfs > 1) ? 
          "<b>Première interface (eth0)</b>" : "<b>Adresse</b>", this));
  vb->addSpacing(5);

  btnDhcp = new QRadioButton("Attribution automatique (DHCP)", this);
  staticOrDhcp->addButton(btnDhcp, D_DHCP);
  vb->addWidget(btnDhcp);

  btnStatic = new QRadioButton("Attribution manuelle", this);
  staticOrDhcp->addButton(btnStatic, D_MANUAL);
  vb->addWidget(btnStatic);

  addr = new IPField(this, "Adresse IP", 30, 10, true);
  vb->addWidget(addr);
  vb->addSpacing(5);

  if (numIfs > 1) {
    addr1 = new IPField(this, "Adresse IP ", 30, 10, true);
    vb->addWidget(new QLabel("<b>Deuxième interface (eth1)</b>", this));
    vb->addWidget(addr1);
    vb->addSpacing(5);
  }
  if (numIfs > 2) {
    addr2 = new IPField(this, "Adresse IP ", 30, 10, true);
    vb->addWidget(new QLabel("<b>Troisième interface (eth2)</b>", this));
    vb->addWidget(addr2);
    vb->addSpacing(5);
  }

  if (numIfs > 3)
    qDebug("Not all interfaces will be supported (%d > 3)", numIfs);


  vb->addWidget(new QLabel("<b>Table de routage</b>", this));
  gateway = new IPField(this, "Passerelle par défaut", 30);
  vb->addWidget(gateway);
  vb->addStretch(5);
  extraRoutes = new RoutesField(this, "Routes supplémentaires (optionnel)");
  extraRoutes->setToolTip("Routes supplémentaires, prioritaires\n"
                          "sur la route par défaut\n"
                          "(option avancée)"); 
  extraRoutes->setEnabled(false);
  vb->addWidget(extraRoutes);
  vb->addStretch(5);


  QVBoxLayout *vbHosts = new QVBoxLayout();
  hostsWidget = new HostsWidget(this);
  vbHosts->addWidget(hostsWidget);
  sectionHosts = new InheritSection("Configuration des noms d'hôtes", CLIP_HOSTS, this);
  sectionHosts->setChildrenLayout(vbHosts);
  vb->addWidget(sectionHosts);
  vb->addStretch(5);
  connect(sectionHosts, SIGNAL(toggled(bool, int)),
      parent, SLOT(inheritSectionToggled(bool, int)));

  connect(staticOrDhcp, SIGNAL(buttonClicked(int)),
                                          this, SLOT(dhcpToggled(int)));

  btnDhcp->setChecked(true);
}

void
NetAddrTab::dhcpToggled(int num)
{
  bool dhcp = (num == D_DHCP);
  bool manual = !dhcp;

  addr->setEnabled(manual);
  gateway->setEnabled(manual);

  emit(dhcpOn(dhcp)); 
}

bool
NetAddrTab::saveCurrentProfile()
{
  bool ret = true;
  if (staticOrDhcp->checkedId() == D_DHCP) {
    profile->setNameVal(CLIP_NET, "ETH0_ADDR", dhcp_str);
  } else {
    profile->setNameVal(CLIP_NET, "ETH0_ADDR", addr->returnIP());
    profile->setNameVal(CLIP_NET, "ETH0_MASK", addr->returnMask());
    profile->setNameVal(CLIP_NET, "DEFAULT_ROUTE", gateway->returnIP());
  }
  if (numIfs > 1) {
    profile->setNameVal(CLIP_NET, "ETH1_ADDR", addr1->returnIP());
    profile->setNameVal(CLIP_NET, "ETH1_MASK", addr1->returnMask());
  }
  if (numIfs > 2) {
    profile->setNameVal(CLIP_NET, "ETH2_ADDR", addr2->returnIP());
    profile->setNameVal(CLIP_NET, "ETH2_MASK", addr2->returnMask());
  }
  profile->setNameVal(CLIP_NET, "ROUTE_EXTRA", extraRoutes->returnValues());

  profile->setVal(CLIP_HOSTS, hostsWidget->getHosts());

  return ret;
}

bool
NetAddrTab::loadCurrentProfile()
{
  bool dhcp = !profile->getNameVal(CLIP_NET, "ETH0_ADDR", dhcp_rx).isEmpty(); 
  setDhcp(dhcp);
  
  if (!dhcp) {
    addr->setNameValue(profile->getNameVal(CLIP_NET, "ETH0_ADDR", ip_rx),
                   profile->getNameVal(CLIP_NET, "ETH0_MASK", mask_rx));
    gateway->setNameValue(profile->getNameVal(CLIP_NET, "DEFAULT_ROUTE", ip_rx));
  }
  
  if (numIfs > 1)
    addr1->setNameValue(profile->getNameVal(CLIP_NET, "ETH1_ADDR", ip_rx),
                    profile->getNameVal(CLIP_NET, "ETH1_MASK", mask_rx));
  if (numIfs > 2)
    addr2->setNameValue(profile->getNameVal(CLIP_NET, "ETH2_ADDR", ip_rx),
                    profile->getNameVal(CLIP_NET, "ETH2_MASK", mask_rx));
  
  extraRoutes->setValue(profile->getNameVal(CLIP_NET, "ROUTE_EXTRA", routes_rx));

  hostsWidget->setHosts(profile->getVal(CLIP_HOSTS));
  sectionHosts->update(profile, advanced);
  
  return true;
}

void
NetAddrTab::showAdvanced(bool on)
{
  advanced = on;
  extraRoutes->setEnabled(on);
  sectionHosts->update(profile, on);
}

void
NetAddrTab::setDhcp(bool on)
{
  int num = (on) ? D_DHCP : D_MANUAL;

  if (on)
    btnDhcp->setChecked(true);
  else
    btnStatic->setChecked(true);
  dhcpToggled(num);
}

void
NetAddrTab::setInterface(int type)
{
  if (type == I_WIFI && staticOrDhcp->checkedId() != D_DHCP) {
    int doit = QMessageBox::question(this, "Activation du DHCP",
    "Vous venez d'activer l'interface Wifi pour le profil courant.\n"
    "Les connexions Wifi utilisent généralement le mécanisme DHCP pour\n"
    "l'attribution d'adresse. Or le profil courant est configuré pour\n"
    "utiliser une adresse statique. Souhaitez-vous basculer le profil\n"
    "courant en DHCP ?", 
    QMessageBox::Yes|QMessageBox::Default, 
    QMessageBox::No|QMessageBox::Escape);

  if (doit == QMessageBox::Yes)
    setDhcp(true);
  }
}

// vi:sw=2:ts=2:et:co=80:
