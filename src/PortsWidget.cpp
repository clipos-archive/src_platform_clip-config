// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PortsWidget.cpp
 * clip-config netfilter ports frame.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QFont>
#include <QLayout>
#include <QLabel>

#include "PortsWidget.h"
#include "PortsField.h"

PortsWidget::PortsWidget(QWidget* parent, const QString& lbl)
  : QFrame(parent)
{
  QVBoxLayout* lv = new QVBoxLayout(this);
  lv->setSpacing(4);

  QLabel *topLbl = new QLabel(lbl, this);
  QFont font = topLbl->font();
  font.setBold(true);
  topLbl->setFont(font);

  lv->addWidget(topLbl);

  QHBoxLayout* lh1 = new QHBoxLayout();
  lh1->setSpacing(4);
  lh1->addStretch(2);
  lh1->addWidget(tcp = new PortsField(this, "TCP (source temporaire)"));
  tcp->setToolTip("Ports destination TCP autorisés en sortie\n"
                  "(port source temporaire, cas le plus courant)");
  lh1->addStretch(2);
  lh1->addWidget(udp = new PortsField(this, "UDP (source temporaire)"));
  udp->setToolTip("Ports destination UDP autorisés en sortie\n"
                  "(port source temporaire, cas le plus courant)");
  lh1->addStretch(2);

  QHBoxLayout* lh2 = new QHBoxLayout();
  lh2->setSpacing(4);
  lh2->addStretch(2);
  lh2->addWidget(same_tcp = new PortsField(this, "TCP (source = destination)"));
  same_tcp->setToolTip("Ports destination TCP autorisés en sortie\n"
                  "(port source égal au port destination)");
  lh2->addStretch(2);
  lh2->addWidget(same_udp = new PortsField(this, "UDP (source = destination)"));
  same_udp->setToolTip("Ports destination UDP autorisés en sortie\n"
                  "(port source égal au port destination - NTP par ex.)");
  lh2->addStretch(2);
  
  lv->addLayout(lh1);
  lv->addLayout(lh2);
}

void 
PortsWidget::setNameValue(const QString& portsTCP, const QString& portsUDP,
          const QString& portsSameTCP, const QString& portsSameUDP)
{
  tcp->setValue(portsTCP);
  udp->setValue(portsUDP);
  same_tcp->setValue(portsSameTCP);
  same_udp->setValue(portsSameUDP);
}

// vi:sw=2:ts=2:et:co=80:
