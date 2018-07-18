// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmUsersMain.cpp
 * clip-config user management main form.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidget>

#include "frmUsersMain.h"
#include "frmUsersAdd.h"
#include "frmUsersMigrate.h"
#include "consts.h"
#include "qtcommon.h"
#include <ClipConfig/CCException.h>

#include <unistd.h>
#include <userd/userd.h>

int g_verbose;
int g_daemonized;

// frmUsersMain constructor
//-----------------------

frmUsersMain::frmUsersMain()
{
  diskinfo = NULL;

  setWindowTitle("Gestion des comptes utilisateur");
  setWindowIcon(QIcon(ICONPATH"users.png"));
  setMinimumWidth(800);

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(7);

  vb->addWidget(new QLabel("Liste des comptes :", this));
  lstUsers = new QTreeWidget(this);
  columnCount = 4;
  QStringList cols;
  cols << "Nom" << "Session ouverte" << "Type" << "Authentification";
  if (hasRMH) {
    columnCount++;
    cols << "Espace niveau Haut";
  }
  if (hasRMB) {
    columnCount++;
    cols << "Espace niveau Bas";
  }
  lstUsers->setColumnCount(columnCount);
  lstUsers->setHeaderLabels(cols);
  lstUsers->setSelectionMode(QAbstractItemView::SingleSelection);

  vb->addWidget(lstUsers);
  vb->addWidget(lblFreeSpace = new QLabel("", this));

  QHBoxLayout *hbButtons = new QHBoxLayout;
  hbButtons->setSpacing(7);

  hbButtons->addStretch(2);
  hbButtons->addWidget(btnAddUser = 
      new QPushButton(QIcon(ICONPATH"list-add.png"), "Ajouter", this));
  hbButtons->addStretch(2);
  hbButtons->addWidget(btnDelUser = 
      new QPushButton(QIcon(ICONPATH"list-remove.png"), "Supprimer", this));
  hbButtons->addStretch(2);
  hbButtons->addWidget(btnMigrate = 
      new QPushButton(QIcon(ICONPATH"secure-card.png"), "Migrer vers carte", this));
  hbButtons->addStretch(2);
  hbButtons->addWidget(btnLockUser = 
      new QPushButton(QIcon(ICONPATH"lock.png"), "Verrouiller", this));
  hbButtons->addStretch(2);
  hbButtons->addWidget(btnUnlockUser = 
      new QPushButton(QIcon(ICONPATH"unlock.png"), "Déverrouiller", this));
  hbButtons->addStretch(2);
  hbButtons->addWidget(btnQuit = 
      new QPushButton(QIcon(ICONPATH"dialog-close.png"), "Quitter", this));
  hbButtons->addStretch(2);
  vb->addLayout(hbButtons);

  // Connections
  //------------
  connect(lstUsers, 
    SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
    this, SLOT(updateButtons()));
  connect(btnAddUser, SIGNAL(clicked()), this, SLOT(addUser()));
  connect(btnDelUser, SIGNAL(clicked()), this, SLOT(delUser()));
  connect(btnMigrate, SIGNAL(clicked()), this, SLOT(migrateUser()));
  connect(btnLockUser, SIGNAL(clicked()), this, SLOT(lockUser()));
  connect(btnUnlockUser, SIGNAL(clicked()), this, SLOT(unlockUser()));
  connect(btnQuit, SIGNAL(clicked()), this, SLOT(close()));

  if (!updateUserList() || !updateFreeSpace()) {
    throw CCException("Could not connect to daemon");
  }
  updateButtons ();
}

QString 
frmUsersMain::getConfirmation(const QString& verb)
{
  QTreeWidgetItem *i = lstUsers->currentItem();

  if (i == NULL)
    return QString();

  QString name = i->text(0);
  if (! i->text(1).isEmpty()) {
      QMessageBox::warning(0, "Opération impossible", 
			    "Impossible de " + verb + " l'utilisateur de la session en cours.",
			    QMessageBox::Ok | QMessageBox::Default, 0, 0);
      return QString();
  }

  QString question = QString("Etes-vous sûr de vouloir ") + verb +
                        " le compte " + i->text(1) + " " + name + " ?";

  if (QMessageBox::question(0, "Confirmation", 
           question, QMessageBox::Yes | QMessageBox::Default,
			     QMessageBox::No | QMessageBox::Escape) == QMessageBox::Yes)
    return name;
  else
    return QString();
}

bool 
frmUsersMain::updateUserList()
{
  userlist_t *list,  *iter;
  QString err;

  int s = sock_connect(userdSocketPath.toStdString().c_str());
  if (s < 0)
    err = QString("Impossible de se connecter auprès du démon userd.");
  else if (client_list_users(s, &list)) {
    err = QString("Une erreur est survenue lors de la récupération\n"
                  "de la liste des utilisateurs.");
  } else {
    lstUsers->clear();

    QString rmh_size, rmb_size;
    list_for_each(iter, list) {
      char *name;
      if (asprintf(&name, "%.*s", 
                iter->userinfo->nlen, iter->userinfo->name) == 0) {
        err = QString("Pas assez de mémoire ?!");
        break;
      }

      if (hasRMH)
        rmh_size = iter->userinfo->rmh_size > 0 ? 
              QString::number(iter->userinfo->rmh_size) : "-";
      if (hasRMB)
        rmb_size = iter->userinfo->rmb_size > 0 ? 
              QString::number(iter->userinfo->rmb_size) : "-";

      QStringList cols;
      QTreeWidgetItem *item;
      cols << name
            << QString((iter->userinfo->type & T_CURRENT_USER) ? "*" : "")
            << type_str[iter->userinfo->type & T_USERTYPE_MASK]
            << auth_str[iter->userinfo->auth & AUTH_TYPE_MASK];
      if (hasRMH)
        cols << rmh_size;
      if (hasRMB)
        cols << rmb_size;

      item = new QTreeWidgetItem(lstUsers, cols);
      item->setTextAlignment(1, Qt::AlignHCenter);
      if (hasRMH || hasRMB)
        item->setTextAlignment(4, Qt::AlignHCenter);
      if (hasRMH && hasRMB)
        item->setTextAlignment(5, Qt::AlignHCenter);

      free(name);
    }
    userlist_free_all(list);
  }

  ::close(s);
  if (!err.isEmpty()) {
    fatalError(err);
    return false;
  }

  for (int i = 0; i < columnCount; i++)
    lstUsers->resizeColumnToContents(i);

  return true;
}

bool 
frmUsersMain::updateFreeSpace()
{
  QString err;

  if (diskinfo != NULL)
    diskinfo_free (diskinfo);

  int s = sock_connect(userdSocketPath.toStdString().c_str());
  if (s < 0)
    err = QString("Impossible de se connecter auprès du démon userd.");
  else if (client_disk_info(s, &diskinfo)) {
    err = QString("Une erreur est survenue lors de la récupération\n"
                  "des informations du disque.");
  }

  lblFreeSpace->setText("Espace disque disponible : " +
			 QString::number(diskinfo->space_available) + " Mo");

  ::close(s);
  if (!err.isEmpty()) {
    fatalError(err);
    return false;
  }
  return true;
}

void 
frmUsersMain::updateButtons() 
{
  if (lstUsers->currentItem() != 0) {
    btnMigrate->setEnabled(true);
  } else {
    btnMigrate->setEnabled(false);
  }

  if(lstUsers->currentItem() != 0 &&
     lstUsers->currentItem()->text(1).isEmpty()) {
    btnDelUser->setEnabled(true);
    btnLockUser->setEnabled(true);
    btnUnlockUser->setEnabled(true);
  } else {
    btnDelUser->setEnabled(false);
    btnLockUser->setEnabled(false);
    btnUnlockUser->setEnabled(false);
  }
}

void 
frmUsersMain::addUser() 
{
  frmUsersAdd d(*diskinfo);
  (void)d.exec();
 
  updateUserList();
  updateFreeSpace();
  updateButtons();
}

void 
frmUsersMain::delUser()
{
  QString userName = getConfirmation("supprimer");
  if (userName.isEmpty())
    return;

  int s = sock_connect(userdSocketPath.toStdString().c_str());
  if (s < 0)
    fatalError("Impossible de se connecter auprès du démon userd.");

  uint32_t ret = client_delete_user(s, userName.toStdString().c_str());
  if (ret != CMD_OK)
    print_error(ret, "supprimer", userName);
  else
    QMessageBox::information(0, "Opération réussie", 
			      "L'utilisateur " + userName + " a été correctement supprimé.",
			      QMessageBox::Ok | QMessageBox::Default, 0, 0);
  
  updateUserList();
  updateFreeSpace();
  updateButtons();
}


void 
frmUsersMain::migrateUser() 
{
  QTreeWidgetItem *i = lstUsers->currentItem();

  if(!i) return;

  frmUsersMigrate d(i->text(0));
  (void)d.exec();
  
  updateUserList();
  updateButtons();
}


void 
frmUsersMain::lockUnlockUser(bool lock)
{
  QString verb = lock ? "verrouiller" : "déverrouiller";
  QString userName = getConfirmation(verb);
  if (userName.isEmpty())
    return;

  int s = sock_connect(userdSocketPath.toStdString().c_str());
  if (s < 0)
    fatalError("Impossible de se connecter auprès du démon userd.");

  uint32_t ret = lock ? 
          client_lock_user(s, userName.toStdString().c_str()) 
          : client_unlock_user(s, userName.toStdString().c_str());
  if (ret != CMD_OK)
    print_error(ret, verb, userName);

  ::close(s);

  updateUserList();
  updateFreeSpace();
  updateButtons();
}

void frmUsersMain::lockUser()
{
  lockUnlockUser(true);
}

void frmUsersMain::unlockUser() 
{
  lockUnlockUser(false);
}

// vi:sw=2:ts=2:et:co=80:
