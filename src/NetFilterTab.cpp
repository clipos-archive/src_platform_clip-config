// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetFilterTab.cpp
 * clip-config net filter config tab widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010-2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>

#include "NetFilterTab.h"
#include "PortsWidget.h"
#include "PortsField.h"
#include "AddrsField.h"
#include "YesNoField.h"
#include "ClipNetProfile.h"
#include "InheritSection.h"
#include "consts.h"

NetFilterTab::NetFilterTab(QWidget *parent, ClipNetProfile *profile, 
              const QString &lbl, const QString &prefix, int protos)
  : prefix(prefix), clientsVar("")
{
  sectionFilter = new InheritSection("Filtrage de ports", CLIP_NETFILTER, this);

  QVBoxLayout *lv = new QVBoxLayout();
  lv->setSpacing(4);
  lv->addSpacing(10);

  clients = NULL;
  dynamicIPs = NULL;

  ports = new PortsWidget(this, lbl);
  lv->addWidget(ports);

  protoBox = NULL;

  if (protos) {
    protoBox = new QGroupBox("Options spécifiques", this);

    QGridLayout *gl = new QGridLayout;
    gl->setHorizontalSpacing(6);
    gl->setVerticalSpacing(6);

    int index = 0;

    if (protos & NF_FTP) {
      YesNoField *chkFtp = new YesNoField("FTP sortant", protoBox,
                                          CLIP_NETFILTER, prefix + "FTP");
      chkFtp->setToolTip("Autoriser les flux FTP sortants\n"
                         "(N.B. : il n'est pas nécessaire d'ajouter\n"
                         "explicitement le port 21 plus haut)");
      gl->addWidget(chkFtp, index / 3, index % 3);
      protoFields.push_back(chkFtp);
      index++;
    }

    if (protos & NF_PING) {
      YesNoField *chkPing = new YesNoField("Ping sortant", protoBox,
                                          CLIP_NETFILTER, prefix + "PING");
      chkPing->setToolTip("Autoriser les pings sortants");
      gl->addWidget(chkPing, index / 3, index % 3);
      protoFields.push_back(chkPing);
      index++;
    }

    /* Add others as needed */

    protoBox->setLayout(gl);
    lv->addSpacing(8);
    lv->addWidget(protoBox);
  }
  lv->addStretch(2);

  sectionFilter->setChildrenLayout(lv);
  QVBoxLayout *sections = new QVBoxLayout(this);
  sections->addWidget(sectionFilter);

  connect(sectionFilter, SIGNAL(toggled(bool, int)),
      parent, SLOT(inheritSectionToggled(bool, int)));
}

NetFilterTab::NetFilterTab(QWidget *parent, ClipNetProfile *profile, 
                const QString &lbl, const QString &prefix,
                const QString &lblClients, const QString &clientsVar, 
                int protos)
  : prefix(prefix), clientsVar(clientsVar)
{
  QVBoxLayout *lv = new QVBoxLayout(this);
  lv->setSpacing(4);
  lv->addSpacing(5);

  lv->addWidget(new QLabel("<b>Adresses des clients</b>"));

  clients = new AddrsField(this, lblClients);
  dynamicIPs = new YesNoField("Adresses dynamiques", this, 
                          CLIP_NET, "DYNAMIC_CLIENT_IPS");
  QVBoxLayout *vb = new QVBoxLayout;
  QHBoxLayout *lh = new QHBoxLayout;
  QLabel *lb = new QLabel("<i>Lorsque cette option "
                          "est active, la passerelle attribue une "
                          "adresse dynamiquement aux clients. "
                          "Dans le cas contraire, chaque client utilise "
                          "l'adresse inscrite dans sa configuration.</i>",
                          this);
  lb->setWordWrap(true);
  QLabel *lb2 = new QLabel("<b>Attention : l'attribution dynamique "
                           "ne supporte qu'un seul sous-réseau client.</b>",
                           this);
  lb2->setWordWrap(true);

  lh->addWidget(clients);
  lh->addSpacing(20);
  vb->addSpacing(30);
  vb->addWidget(dynamicIPs);
  vb->addSpacing(10);
  vb->addWidget(lb);
  vb->addWidget(lb2);
  vb->addStretch(5);
  lh->addLayout(vb);
  lh->addStretch(3);
  lv->addLayout(lh);
  lv->addSpacing(20);
  lv->addStretch(2);

  QVBoxLayout *lv2 = new QVBoxLayout();
  ports = new PortsWidget(this, lbl);
  lh = new QHBoxLayout;
  lh->addSpacing(30);
  lh->addWidget(ports);
  lh->addStretch(3);
  lv2->addLayout(lh);

  if (protos) {
    protoBox = new QGroupBox("Options spécifiques", this);

    QGridLayout *gl = new QGridLayout;
    gl->setHorizontalSpacing(6);
    gl->setVerticalSpacing(6);

    int index = 0;

    if (protos & NF_PING) {
      YesNoField *chkPing = new YesNoField("Ping entrant", protoBox,
                                          CLIP_NETFILTER, prefix + "PING");
      chkPing->setToolTip("Autoriser les pings entrants\n"
                           "(depuis les clients)");
      gl->addWidget(chkPing, index / 3, index % 3);
      protoFields.push_back(chkPing);
      index++;
    }

    if (protos & NF_SNAT) {
      YesNoField *chkSnat = new YesNoField("SNAT des clients", protoBox,
                                        CLIP_NETFILTER, "SNAT_CLIENT_NETWORKS");
      chkSnat->setToolTip("Donner aux flux clients l'adresse de la\n"
                           "passerelle lors de leur réémmission sur eth1");
      gl->addWidget(chkSnat, index / 3, index % 3);
      protoFields.push_back(chkSnat);
      index++;
    }

    /* Add others as needed */

    protoBox->setLayout(gl);
    lv2->addSpacing(8);
    lv2->addWidget(protoBox);
  }

  lv2->addStretch(2);

  sectionFilter = new InheritSection("Filtrage de ports", CLIP_NETFILTER, this);
  sectionFilter->setChildrenLayout(lv2);

  lv->addWidget(sectionFilter);

  connect(sectionFilter, SIGNAL(toggled(bool, int)),
      parent, SLOT(inheritSectionToggled(bool, int)));
}

void
NetFilterTab::showAdvanced(bool on)
{
  QLinkedList<YesNoField *>::iterator it;
  for (it = protoFields.begin(); it != protoFields.end(); it++)
    (*it)->showAdvanced(on);
}

bool 
NetFilterTab::saveCurrentProfile()
{
  bool ret = true;

  profile->setNameVal(CLIP_NETFILTER, 
                  prefix + "TCP", ports->tcp->returnValues());
  profile->setNameVal(CLIP_NETFILTER, 
                  prefix + "UDP", ports->udp->returnValues());
  profile->setNameVal(CLIP_NETFILTER, 
                  prefix + "SAME_TCP", ports->same_tcp->returnValues());
  profile->setNameVal(CLIP_NETFILTER, 
                  prefix + "SAME_UDP", ports->same_udp->returnValues());

  if (clients) {
    QString val = clients->returnValues();
    if (dynamicIPs->isChecked()) {
      QStringList vals = val.split(' ', QString::SkipEmptyParts);
      if (vals.size() > 1) {
        QMessageBox::warning(0, "Réseaux multiples non supportés",
          "L'utilisation de réseaux clients multiples\n"
          "n'est pas supportée avec une attribution\n"
          "dynamique des adresses.\n"
          "Seul le premier réseau client a été retenu.");
        val = vals.first();
      }
    }
    profile->setNameVal(CLIP_NET, clientsVar, val);
    dynamicIPs->saveProfile(profile);
  }

  QLinkedList<YesNoField *>::iterator it;
  for (it = protoFields.begin(); it != protoFields.end(); it++) {
    if (!(*it)->saveProfile(profile))
      ret = false;
  }
  
  return ret;
}

bool 
NetFilterTab::loadCurrentProfile()
{
  bool ret = true;

  ports->setNameValue(
    profile->getNameVal(CLIP_NETFILTER, prefix + "TCP", ports_rx),
    profile->getNameVal(CLIP_NETFILTER, prefix + "UDP", ports_rx),
    profile->getNameVal(CLIP_NETFILTER, prefix + "SAME_TCP", ports_rx),
    profile->getNameVal(CLIP_NETFILTER, prefix + "SAME_UDP", ports_rx));

  if (clients) {
    clients->setValue(profile->getNameVal(CLIP_NET, clientsVar, multiaddrs_rx));
    dynamicIPs->loadProfile(profile);
  }

  QLinkedList<YesNoField *>::iterator it;
  for (it = protoFields.begin(); it != protoFields.end(); it++) {
    if (!(*it)->loadProfile(profile))
      ret = false;
  }

  sectionFilter->update(profile, true);
  
  return ret;
}

// vi:sw=2:ts=2:et:co=80:

