// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmNetMain.h
 * clip-config main network configuration form header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMNETMAIN_H
#define FRMNETMAIN_H

#include <QDialog>
#include <QLinkedList>
#include <QMap>
#include <QStringList>

class ClipNetProfile;

class NetTab;
class NetAddrTab;
class NetAdminTab;
class NetDaemonThread;
class NetFilterTab;
class NetGwTab;
class NetInterfaceTab;
class NetJailTab;
class NetPeerTab;
class NetUmtsTab;
class NetWifiTab;

class QCheckBox;
class QComboBox;
class QKeyEvent;
class QLabel;
class QPushButton;
class QTabWidget;

typedef enum {
  NET_TAB_INTERFACE = 0,
  NET_TAB_UMTS,
  NET_TAB_WIFI,
  NET_TAB_ADDR,
  NET_TAB_RMH,
  NET_TAB_RMB,
  NET_TAB_CLIENTS,
  NET_TAB_ADMIN,
  NET_TAB_RMHFILTER,
  NET_TAB_RMBFILTER,
  NET_TAB_PEER,
  NET_TAB_GW,
  NET_TAB_UPDFILTER,
  NET_TAB_UPDATE,
  NET_TAB_MAX,
} NetTabIdx;

class frmNetMain : public QDialog
{
  Q_OBJECT

public:
  frmNetMain(int numIfs, int ifTypes);

 public slots:
  void inheritSectionToggled(bool on, int file);

 private:
  QComboBox *netProfiles;
  ClipNetProfile *profile;
  QStringList knownProfiles;
  int profileIdx;

  QLabel *lblActive;

  QPushButton *btnAddProfile;
  QPushButton *btnDelProfile;
  QPushButton *btnActivateProfile;
  QPushButton *btnSaveProfile;

  QPushButton *btnQuit;
  QCheckBox   *chkAdvanced;

  QTabWidget *tabs;

  int numIfs;
  bool hasWifi;
  bool hasUmts;

  NetInterfaceTab *ifTab;
  NetAddrTab *addrTab;
  NetUmtsTab *umtsTab;
  NetWifiTab *wifiTab;

  NetJailTab *rmhTab;
  NetJailTab *rmbTab;
  NetJailTab *updTab;
  NetGwTab   *gwTab;
  NetPeerTab *peerTab;
  
  NetFilterTab *clientsTab;
  NetAdminTab  *adminTab;
  NetFilterTab *rmhFiltTab;
  NetFilterTab *rmbFiltTab;
  NetFilterTab *updFiltTab;

  QLinkedList<NetTab *> tabList;
  QString tabCaptions[NET_TAB_MAX];
  bool tabPresent[NET_TAB_MAX];
  int tabIndexes[NET_TAB_MAX];

  bool advanced;
  int interface;

 protected:
  virtual void keyPressEvent(QKeyEvent* e);

 private:
  void listProfiles();
  void loadProfile();
  void updateProfile();
  bool savePending(QString action);
  bool isActive(const QString& profileName);

  void showTab(NetTabIdx num, NetTab *tab);
  void hideTab(NetTabIdx num);
  void enableTab(NetTabIdx num, NetTab *tab, bool on);

 signals:
  void setActive(const QString &);
  /* Interface set by user or profile */
  void netInterfaceChanged(int);
  /* Interface set by user explicitly */
  void netInterfaceSet(int);
  void newProfile();

 private slots:
  void profileSelected(int index);
  void saveProfile();
  void addProfile();
  void activateProfile();
  void deleteProfile();
  
  void setInterface(int);
  void showAdvanced(bool);

  void quit();
};

#endif // FRMNETMAIN_H

// vi:sw=2:ts=2:et:co=80:
