// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmNetMain.cpp
 * clip-config main network configuration form.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010-2014 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QProcess>

#include <unistd.h>

#include "ClipNetProfile.h"
#include "NetTab.h"
#include "NetAddrTab.h"
#include "NetDaemonThread.h"
#include "NetAddrTab.h"
#include "NetAdminTab.h"
#include "NetFilterTab.h"
#include "NetGwTab.h"
#include "NetInterfaceTab.h"
#include "NetJailTab.h"
#include "NetPeerTab.h"
#include "NetUmtsTab.h"
#include "NetWifiTab.h"
#include "frmNetAdd.h"
#include <ClipWidgets/WaitDialog.h>
#include <ClipConfig/CCException.h>
#include "qtcommon.h"
#include "consts.h"

#include "frmNetMain.h"

frmNetMain::frmNetMain(int numIfs, int ifTypes)
  : profile(NULL), numIfs(numIfs), advanced(false), interface(0)
{
  setWindowTitle("Configuration du réseau");
  setWindowIcon(QIcon(ICONPATH"preferences-system-network.png"));

  QRect screen = QApplication::desktop()->screenGeometry();
  if (screen.height() < 800) 
    setMinimumHeight(screen.height() - 150);
  else
    setMinimumHeight(650);

  setMinimumWidth(750);

  if (!numIfs) {
    if (QFile::exists(eth0FlagFile))
      numIfs = 1;
    if (QFile::exists(eth1FlagFile))
      numIfs = 2;
    if (QFile::exists(eth2FlagFile))
      numIfs = 3;
  }

  hasWifi = ((ifTypes & I_WIFI) != 0) || QFile::exists(HASWIFI_FLAG);
  hasUmts = ((ifTypes & I_UMTS) != 0) 
                  || ((numIfs == 1) || (!numIfs && hasWifi));

  tabCaptions[NET_TAB_INTERFACE] = "Interface";
  tabCaptions[NET_TAB_UMTS] = "UMTS";
  tabCaptions[NET_TAB_WIFI] = "Wifi";
  tabCaptions[NET_TAB_ADDR] = "Adresse du socle";
  tabCaptions[NET_TAB_RMH] = "Niveau Haut";
  tabCaptions[NET_TAB_RMB] = "Niveau Bas";
  tabCaptions[NET_TAB_CLIENTS] = "Flux routés";
  tabCaptions[NET_TAB_PEER] = "Chiffreur homologue";
  tabCaptions[NET_TAB_ADMIN] = "Téléadministration";
  tabCaptions[NET_TAB_RMHFILTER] = "Pare-feu niveau Haut";
  tabCaptions[NET_TAB_RMBFILTER] = "Pare-feu niveau Bas";
  tabCaptions[NET_TAB_GW] = "Passerelles";
  tabCaptions[NET_TAB_UPDFILTER] = "Flux des Màj";
  tabCaptions[NET_TAB_UPDATE] = "Mises à jour";

  for (int i = 0; i < NET_TAB_MAX; i++) {
    tabIndexes[i] = 0;
    tabPresent[i] = false;
  }

  /****** Profile selection *******/

  netProfiles = new QComboBox(this);
  netProfiles->setMinimumWidth(150);
  
  btnAddProfile = new QPushButton(QIcon(ICONPATH"list-add.png"), "", this);
  btnAddProfile->setDefault(false);
  btnAddProfile->setAutoDefault(false);
  btnAddProfile->setToolTip("Créer un nouveau profil (initialement\n"
                            "une copie du profil courant)");
  
  btnDelProfile = new QPushButton(QIcon(ICONPATH"list-remove.png"), "", this);
  btnDelProfile->setDefault(false);
  btnDelProfile->setAutoDefault(false);
  btnDelProfile->setToolTip("Supprimer le profil courant");

  QPushButton* btnActivateProfile = 
              new QPushButton(QIcon(ICONPATH"dialog-ok.png"), "Activer", this);
  btnActivateProfile->setDefault(false);
  btnActivateProfile->setAutoDefault(false);
  btnActivateProfile->setToolTip("Activer le profil courant");

  btnSaveProfile = new QPushButton(QIcon(ICONPATH"document-save.png"), 
                                   "Sauver", this);
  btnSaveProfile->setDefault(false);
  btnSaveProfile->setAutoDefault(false);
  btnSaveProfile->setToolTip("Sauver le profil courant");

  lblActive = new QLabel("", this);
  QFont font = lblActive->font();
  font.setBold(true);
  lblActive->setFont(font);

  connect(netProfiles, SIGNAL(activated(int)), 
                    this, SLOT(profileSelected(int))); 
   
  connect(btnAddProfile, SIGNAL(clicked()), 
                    this, SLOT(addProfile()));
  connect (btnDelProfile, SIGNAL(clicked()), 
                    this, SLOT(deleteProfile()));
  connect(btnActivateProfile, SIGNAL(clicked()), 
                    this, SLOT(activateProfile()));
  connect(btnSaveProfile, SIGNAL(clicked()), 
                    this, SLOT(saveProfile()));

  connect (this, SIGNAL(setActive(const QString&)), 
                    lblActive, SLOT(setText(const QString&)));

  /****** Bottom ******/
  chkAdvanced = new QCheckBox("Options avancées", this);
  chkAdvanced->setToolTip("Montrer les options avancées :\n"
                          "configuration réseau du socle, passerelles...");
  btnQuit = new QPushButton (QIcon(ICONPATH"dialog-close.png"), 
                             "Quitter", this);
  btnQuit->setDefault(false);
  btnQuit->setAutoDefault(false);

  connect(btnQuit, SIGNAL(clicked()), this, SLOT(quit()));
  connect(chkAdvanced, SIGNAL(toggled(bool)), this, SLOT(showAdvanced(bool)));
  connect(this, SIGNAL(netInterfaceChanged(int)), 
                                              this, SLOT(setInterface(int)));

  /****** Tabs ******/

  tabs = new QTabWidget(this);
  tabList.clear();

  int ifs = I_NONE;
  if (numIfs)
    ifs |= I_WIRED;
  if (hasWifi)
    ifs |= I_WIFI;
  if (hasUmts)
    ifs |= I_UMTS;

  ifTab = new NetInterfaceTab(this, profile, ifs);
  showTab(NET_TAB_INTERFACE, ifTab);
  tabList.push_back(ifTab);
  connect(ifTab, SIGNAL(netInterfaceChanged(int)), 
                                  this, SIGNAL(netInterfaceChanged(int)));
  connect(ifTab, SIGNAL(netInterfaceSet(int)), 
                                  this, SIGNAL(netInterfaceSet(int)));

  if (hasUmts) {
    umtsTab = new NetUmtsTab(this, profile);
    showTab(NET_TAB_UMTS, umtsTab);
    tabList.push_back(umtsTab);
  } else 
    umtsTab = NULL;

  if (hasWifi) {
    wifiTab = new NetWifiTab(this, profile);
    showTab(NET_TAB_WIFI, wifiTab);
    tabList.push_back(wifiTab);
  } else 
    wifiTab = NULL;

  addrTab = new NetAddrTab(this, profile, numIfs);
  showTab(NET_TAB_ADDR, addrTab);
  tabList.push_back(addrTab);

  if (hasRMH) {
    rmhTab = new NetJailTab(this, profile, "Niveau Haut", RMH_RESOLVCONF, RMH_PROXY, RMH_HOSTS, RMH_TLS);
    showTab(NET_TAB_RMH, rmhTab);
    tabList.push_back(rmhTab);
    //connect(addrTab, SIGNAL(dhcpOn(bool)), rmhTab, SLOT(warnDhcp(bool)));
  } else
    rmhTab = NULL;

  if (hasRMB) {
    rmbTab = new NetJailTab(this, profile, "Niveau Bas", RMB_RESOLVCONF, RMB_PROXY, RMB_HOSTS, RMB_TLS);
    showTab(NET_TAB_RMB, rmbTab);
    tabList.push_back(rmbTab);
    connect(addrTab, SIGNAL(dhcpOn(bool)), rmbTab, SLOT(warnDhcp(bool)));
  } else 
    rmbTab = NULL;

  if (numIfs >= 2) {
    clientsTab = new NetFilterTab(this, profile, 
                        "Flux clients autorisés (<i>eth0 => eth1</i>)",
                        "FWD_IN_", "<b>Réseaux clients (côté <i>eth0</i>)</b>", 
                        "CLIENT_NETWORKS", NF_PING|NF_SNAT);
    showTab(NET_TAB_CLIENTS, clientsTab);
    tabList.push_back(clientsTab);

    peerTab = new NetPeerTab(this, profile);
    showTab(NET_TAB_PEER, peerTab);
    tabList.push_back(peerTab);


    adminTab = new NetAdminTab(this, profile,
                      "<b>Postes administrateurs (eth1)</b>", "ADMIN_NETWORKS",
                      "<b>Postes administrés (eth0)</b>", "ADMIN_TARGETS");
    showTab(NET_TAB_ADMIN, adminTab);
    tabList.push_back(adminTab);
  } else {
    clientsTab = NULL;
    peerTab = NULL;
    adminTab = NULL;
  }

  if (hasRMH) {
    rmhFiltTab = new NetFilterTab(this, profile, 
                          "Flux autorisés - niveau Haut", "RMH_OUT_",
                          NF_FTP|NF_PING);
    showTab(NET_TAB_RMHFILTER, rmhFiltTab);
    tabList.push_back(rmhFiltTab);
  } else 
    rmhFiltTab = NULL;

  if (hasRMB) {
    rmbFiltTab = new NetFilterTab(this, profile, 
                          "Flux autorisés - niveau Bas", "RMB_OUT_",
                          NF_FTP|NF_PING);
    showTab(NET_TAB_RMBFILTER, rmbFiltTab);
    tabList.push_back(rmbFiltTab);
  } else 
    rmbFiltTab = NULL;

  gwTab = new NetGwTab(this, profile, hasRMH, hasRMB);
  showTab(NET_TAB_GW, gwTab);
  tabList.push_back(gwTab);
    
  updFiltTab = new NetFilterTab(this, profile,
                "Flux autorisés pour les mises à jour", "UPDATE_OUT_");
  showTab(NET_TAB_UPDFILTER, updFiltTab);
  tabList.push_back(updFiltTab);

  updTab = new NetJailTab(this, profile, "UPDATE", CLIP_RESOLVCONF);
  showTab(NET_TAB_UPDATE, updTab);
  tabList.push_back(updTab);

  for (QLinkedList<NetTab *>::iterator it = tabList.begin(); 
                                      it != tabList.end(); it++) {
    connect(this, SIGNAL(newProfile()), *it, SLOT(initProfile()));
    connect(this, SIGNAL(netInterfaceSet(int)), 
                                    *it, SLOT(setInterface(int)));
  }

  /****** Layout ******/

  QHBoxLayout *hbProfiles = new QHBoxLayout;
  hbProfiles->setSpacing(4);
  hbProfiles->addSpacing(20);
  hbProfiles->addStretch(2);
  hbProfiles->addWidget(netProfiles);
  hbProfiles->addSpacing(10);
  hbProfiles->addWidget(btnAddProfile);
  hbProfiles->addSpacing(10);
  hbProfiles->addWidget(btnDelProfile);
  hbProfiles->addSpacing(30);
  hbProfiles->addWidget(btnActivateProfile);
  hbProfiles->addSpacing(20);
  hbProfiles->addWidget(btnSaveProfile);
  hbProfiles->addStretch(4);

  QHBoxLayout *hbActive = new QHBoxLayout;
  hbActive->setSpacing(4);
  hbActive->addStretch(5);
  hbActive->addWidget(lblActive);
  hbActive->addStretch(5);

  QHBoxLayout *hbBottom = new QHBoxLayout;
  hbBottom->setSpacing(4);
  hbBottom->addStretch(5);
  hbBottom->addWidget(chkAdvanced);
  hbBottom->addStretch(5);
  hbBottom->addWidget(btnQuit);
  hbBottom->addStretch(5);

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(4);
  vb->addLayout(hbProfiles);
  vb->addLayout(hbActive);
  vb->addWidget(tabs);
  vb->addLayout(hbBottom);

  /****** Initialization ******/
  
  listProfiles();
  loadProfile();
  showAdvanced(chkAdvanced->isChecked());
}

/*************************************************************/
/*                      Public slots                         */
/*************************************************************/

void
frmNetMain::inheritSectionToggled(bool on, int file)
{
  if (on) {
    profile->inheritFile(file, defaultProfileName);
  } else {
    profile->disinheritFile(file);
  }
  loadProfile();
}

/*************************************************************/
/*                    Protected slots                        */
/*************************************************************/

void 
frmNetMain::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Escape) {
    e->accept();
    quit();
  }
}

/*************************************************************/
/*                      Private slots                        */
/*************************************************************/

void
frmNetMain::profileSelected(int index)
{
  if (profileIdx == index)
    return;

  updateProfile();
  if (!profile->pendingChanges() ||
      savePending("Voulez-vous les sauvegarder avant d'éditer "
                         "un autre profil ?")) {
    loadProfile();
  } else {
    netProfiles->setCurrentIndex(profileIdx);
  }
}

void
frmNetMain::saveProfile()
{
  updateProfile();
  profile->writeProfile();
}

void
frmNetMain::addProfile()
{
  updateProfile();
  QString pname = netProfiles->currentText();
  bool isDefault = pname == defaultProfileName;
  frmNetAdd dialog(profile->pendingChanges(), pname, isDefault);

  if ((dialog.exec () == Accepted) && ((dialog.result & ADD_OK) != 0)) {
    if (dialog.result & ADD_SAVE_BEFORE)
      saveProfile();

    if (isDefault && dialog.inherit) {
      profile->inheritProfileAs(dialog.newName);
      // These files are never inherited because they are dependant on one
      // another and therefore they should be either all inherited or
      // all not inherited. That would be hard to do in the GUI
      // and is therefore not supported. HC 2013-05-22
      profile->disinheritFile(CLIP_NET);
      profile->disinheritFile(CLIP_UMTS);
      profile->disinheritFile(CLIP_WIRELESS);
    } else {
      profile->writeProfileAs(dialog.newName);
    }

    int i;
    for (i = 0; i < netProfiles->count(); i++) {
      if (netProfiles->itemText(i).compare(dialog.newName) > 0)
        break;
    }
    netProfiles->insertItem(i, 
                QIcon(ICONPATH"applications-internet.png"), dialog.newName);
    netProfiles->setCurrentIndex(i);

    loadProfile();
    emit(newProfile());
  }
}

void
frmNetMain::deleteProfile()
{
  QString pname = netProfiles->currentText();
  if (pname.isEmpty())
    throw CCException("deleteProfile: empty name");

  if (defaultProfile_rx.exactMatch(pname))
    throw CCException("deleteProfile: called on default profile");

  if (QMessageBox::question (0, "Supprimer un profil ?",
           QString ("Êtes-vous certain de vouloir supprimer le profil ")
           + pname + " ?",
           QMessageBox::Yes | QMessageBox::Default,
           QMessageBox::No | QMessageBox::Escape) == QMessageBox::Yes) {
    netProfiles->removeItem(netProfiles->currentIndex());

    if (!profile->deleteProfile()) {
      QMessageBox::warning (0, "Erreur lors de la suppression",
          "Le profil " + pname + " n'a pas pu être complètement effacé.\n"
          "Il vous faudra supprimer les fichiers restants manuellement.\n"
          "Les profils sont stockés dans le répértoire " 
          + netconfdParentDir + ".",
          QMessageBox::Ok | QMessageBox::Default, 0, 0);
    }
    
    loadProfile();
  }
}

void
frmNetMain::activateProfile()
{
  updateProfile();

  if (!profile->pendingChanges() ||
      savePending("Voulez-vous les sauvegarder "
                            "avant d'activer ce profil ?")) {
    bool finished = false;
    char c = '\0';
    QString pname = netProfiles->currentText();
    NetDaemonThread t('R', pname, c, finished);
    WaitDialog dialog("Opération en cours", 
          "Veuillez patienter pendant l'activation du profil...", finished);

    t.start();
    dialog.exec();
   
    if (c == 'Y') {
      QMessageBox::information (0, "Activation effectuée",
                        "Le profil " + pname + " a été activé.",
                        QMessageBox::Ok | QMessageBox::Default, 0, 0);
    } else {
      activation_warning(c, pname);
    }

    emit(setActive(isActive (pname) ? 
                    "Ce profil est actuellement actif." : ""));
  }
}

void
frmNetMain::showAdvanced(bool on)
{
  advanced = on;

  bool defaultProfile = netProfiles->currentText() == defaultProfileName;

  for (QLinkedList<NetTab *>::iterator it = tabList.begin(); 
                                      it != tabList.end(); it++)
    (*it)->showAdvanced(on);

  if ((interface != I_NONE) || defaultProfile) {
    if (on) {
      showTab(NET_TAB_UPDFILTER, updFiltTab);
      showTab(NET_TAB_UPDATE, updTab);
      if (peerTab)
        showTab(NET_TAB_PEER, peerTab);
    } else {
      hideTab(NET_TAB_UPDATE);
      hideTab(NET_TAB_UPDFILTER);
      if (peerTab)
        hideTab(NET_TAB_PEER);
    }
  }
}

void
frmNetMain::setInterface(int type)
{
  interface = type;

  if (hasUmts)
    enableTab(NET_TAB_UMTS, umtsTab, (type == I_UMTS));

  enableTab(NET_TAB_ADDR, addrTab, !(type & I_NOADDR));

  if (hasWifi)
    enableTab(NET_TAB_WIFI, wifiTab, (type == I_WIFI));

  // Always show the netfilter / gateway, etc tabs for
  // the default profile, no allow changings settings
  // which are then inherited by other profiles
  bool defaultProfile = netProfiles->currentText() == defaultProfileName;
  bool net = ((type != I_NONE) || defaultProfile);
  bool net_adv = (net && advanced);

  if (hasRMH) {
    enableTab(NET_TAB_RMH, rmhTab, net);
    enableTab(NET_TAB_RMHFILTER, rmhFiltTab, net);
  }
  
  if (hasRMB) {
    enableTab(NET_TAB_RMB, rmbTab, net);
    enableTab(NET_TAB_RMBFILTER, rmbFiltTab, net);
  }

  if (numIfs > 1) {
    enableTab(NET_TAB_CLIENTS, clientsTab, net);
    enableTab(NET_TAB_PEER, peerTab, net);
    enableTab(NET_TAB_ADMIN, adminTab, net);
  }

  enableTab(NET_TAB_UPDATE, updTab, net_adv);
  enableTab(NET_TAB_UPDFILTER, updFiltTab, net_adv);
}

void
frmNetMain::quit()
{
  updateProfile();
  if (!profile) {
    QApplication::exit(0);
    return;
  }

  if (!profile->pendingChanges() ||
        savePending("Voulez-vous les sauvegarder avant de quitter ?")) {
    delete(profile);
    QApplication::exit(0);
  }
}

/*************************************************************/
/*                          Private                          */
/*************************************************************/
void
frmNetMain::listProfiles()
{
  QDir d(netconfdParentDir, QString::null, QDir::Name, QDir::Dirs);

  if (!d.exists()) {
   QMessageBox::warning (0, "Erreur interne",
       "Le répertoire " + netconfdParentDir + " n'existe pas.\n"
       "Celui-ci est censé contenir les différents profils réseau.\n"
       "Veuillez le recréer manuellement avant de relancer ce programme.",
       QMessageBox::Ok | QMessageBox::Default, 0, 0);
   throw CCException("listProfiles could not find the profiles directory");
  }

  QStringList candidates = d.entryList();
  QStringList::iterator i;
  bool default_present = false;
  int whereToInsert = 0;
  int count = 0;

  netProfiles->clear();
  for (i = candidates.begin(); i != candidates.end(); ++i) {
    if (profileName_rx.exactMatch(*i)) {
      if (defaultProfile_rx.exactMatch(*i))
        default_present = true;
      else if ((*i).compare(defaultProfileName) < 0)
        whereToInsert++;

      netProfiles->insertItem(count, 
              QIcon(ICONPATH"applications-internet.png"), *i);
      if (isActive(*i))
        netProfiles->setCurrentIndex(count);

      count++;
    }
  }

  if (!default_present) {
    netProfiles->insertItem(whereToInsert, 
          QIcon(ICONPATH"applications-internet.png"), defaultProfileName);
    if (isActive(defaultProfileName))
      netProfiles->setCurrentIndex(whereToInsert);
  }
}

void
frmNetMain::loadProfile()
{
  tabs->setCurrentIndex(0);

  if (profile) {
    delete profile;
    profile = NULL;
  }

  profileIdx = netProfiles->currentIndex();
  QString pname = netProfiles->currentText();

  if (pname.isEmpty())
    throw CCException("loadProfile: empty profile name");

  btnDelProfile->setEnabled(!defaultProfile_rx.exactMatch(pname));

  emit(setActive((isActive(pname) ? 
                    "Ce profil est actuellement actif." : "")));

  profile = new ClipNetProfile(pname, hasWifi);

  QLinkedList<NetTab *>::iterator it;

  for (it = tabList.begin(); it != tabList.end(); ++it) 
    (*it)->setProfile(profile);

  for (it = tabList.begin(); it != tabList.end(); ++it) {
    if (!(*it)->loadCurrentProfile()) {
      profile->setValid(false);
    }
  }

  /* When parameters are added to the config, the program might ask to
     save pending changes the user has not made. To avoid this, we
     preemptively save the profile we just loaded. This way, the
     semantics is left unchanged but the parameters are all saved. */
  saveProfile();
}


void
frmNetMain::updateProfile()
{
  QLinkedList<NetTab *>::iterator it;
  for (it = tabList.begin(); it != tabList.end(); it++) 
    (*it)->saveCurrentProfile();
}

bool
frmNetMain::savePending(QString question)
{
  QString msg1("Le profil "+ profile->getProfileName() +
            " contient des modifications non sauvegardées.\n");

  if (!profile->isValid())
    msg1 += "Attention : ce profil a été automatiquement modifié du fait\n"
            "de paramètres non supportés (interface non présente sur\n"
            "le système).\n";

  int answer = QMessageBox::question(0, "Sauver les modifications ?", 
              msg1 + question,
              QMessageBox::Yes | QMessageBox::Default, QMessageBox::No,
              QMessageBox::Cancel | QMessageBox::Escape);
  switch (answer) {
    case QMessageBox::Yes:
      profile->setValid(true); // You asked for it...
      saveProfile();
      return true;
    case QMessageBox::No:
      return true;
    default:
      return false;
  }
}

bool
frmNetMain::isActive(const QString& pname)
{
  bool active = false;
  char link_target[NAME_MAX + 1];
  ssize_t len;
  
  len = readlink(netconfLink.toStdString().c_str(), link_target, NAME_MAX);
  if (len > 0) {
    link_target[len] = 0;
    QString target(link_target);
    if (target.endsWith(QDir::separator()))
      target.chop(1);
    active = (target == netconfdParentDir + "/" + pname);
  }

  return active;
}

/* 
 * Yeah, I need all this junk to insert / remove tabs in the right 
 * order... Or else I missed something...
 */
inline void
frmNetMain::showTab(NetTabIdx num, NetTab *tab)
{
  if (!tabPresent[num]) {
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(tab);
    tabs->insertTab(tabIndexes[num], scrollArea, tabCaptions[num]);
    tabPresent[num] = true;
    for (int _i = num + 1; _i < NET_TAB_MAX; _i++)
      tabIndexes[_i] += 1;
  }
}

inline void
frmNetMain::hideTab(NetTabIdx num)
{
  if (tabPresent[num]) {
    tabs->removeTab(tabIndexes[num]);
    tabPresent[num] = false;
    for (int _i = num + 1; _i < NET_TAB_MAX; _i++)
      tabIndexes[_i] -= 1;
  }
}

inline void
frmNetMain::enableTab(NetTabIdx num, NetTab *tab, bool on)
{
  if (on)
    showTab(num, tab);
  else
    hideTab(num);
}

// vi:sw=2:ts=2:et:co=80:
