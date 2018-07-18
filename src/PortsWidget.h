// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PortsWidget.h
 * clip-config netfilter ports frame.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef PORTSWIDGET_H
#define PORTSWIDGET_H

#include <QFrame>

class PortsField;

class PortsWidget : public QFrame
{
  Q_OBJECT
    
 public:
  PortsField *tcp;
  PortsField *udp;
  PortsField *same_tcp;
  PortsField *same_udp;

  PortsWidget(QWidget *parent, const QString& lbl);
  void setNameValue(const QString& portsTCP, const QString& portsUDP,
      const QString& portsSameTCP, const QString& portsSameUDP);
};

#endif // PORTSWIDGET_H

// vi:sw=2:ts=2:et:co=80:
