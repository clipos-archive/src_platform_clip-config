// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file HostsWidget.h
 * clip-config hosts file widget header.
 * @author Hugo Chargois <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef HOSTSWIDGET_H
#define HOSTSWIDGET_H

#include <QFrame>
#include <QWidget>
#include <map>

class QTableWidget;
class QLineEdit;
class QSpinBox;
class QRadioButton;

class HostsWidget : public QFrame
{
  Q_OBJECT
public:
  HostsWidget(QWidget *parent = 0);
  QString getHosts();
  void setHosts(QString hosts);

protected slots:
  void addHost();
  void removeHost();

private:
  class HostsMgr;

  HostsMgr *hostsMgr;
  QTableWidget *table;
  QLineEdit *inputIp;
  QLineEdit *inputHostname;
  QRadioButton *localIp;
  void addLine(QString const& ip, QString const& name);
  void validateInput();
};

class HostsWidget::HostsMgr
{
public:
  HostsMgr();
  void addHost(QString const& ip, QString const& name);
  void removeHost(QString const& name);
  QString getLocalIp();
  QString getHosts();

private:
  std::map<QString, QString> hosts;
};

#endif // HOSTSWIDGET_H

// vim:sw=2:ts=2:et:co=80:
