// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file TlsTunnelWidget.h
 * clip-config tls tunnel widget header.
 * @author Hugo Chargois <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef TLSTUNNELWIDGET_H
#define TLSTUNNELWIDGET_H

#include <QFrame>
#include <QWidget>
#include <map>

class QTableWidget;
class QLineEdit;
class QSpinBox;

class TlsTunnelWidget : public QFrame
{
  Q_OBJECT
public:
  TlsTunnelWidget(QWidget *parent = 0);
  QString getTunnels();
  void setTunnels(QString tunnels);

protected slots:
  void addTunnel();
  void removeTunnel();

private:
  class TunnelMgr;

  TunnelMgr *tunnelMgr;
  QTableWidget *table;
  QLineEdit *addName;
  QSpinBox *addPortSrc;
  QLineEdit *addDst;
  QSpinBox *addPortDst;
  void addLine(QString const& name, int portSrc, QString const& ipDst, int portDst);
  void validateTunnel(QLineEdit* name, QSpinBox* portSrc, QLineEdit* ipDst, QSpinBox* portDst);
};

class TlsTunnelWidget::TunnelMgr
{
public:
  TunnelMgr();
  void addTunnel(QString const& name, int portSrc, QString const& ipDst, int portDst);
  void removeTunnel(QString const& name);
  QString getCSV();

private:
  class Tunnel;

  std::map<QString, Tunnel> tunnels;
};

class TlsTunnelWidget::TunnelMgr::Tunnel
{
public:
  Tunnel(int portSrc, QString const& ipDst, int portDst);
  bool hasSameSource(Tunnel const& other);
  bool hasSameDest(Tunnel const& other);
  QString getCSV();

private:
  int portSrc;
  QString ipDst;
  int portDst;
};
#endif // TLSTUNNELWIDGET_H

// vim:sw=2:ts=2:et:co=80:
