// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file HostsWidget.cpp
 * clip-config hosts file widget.
 * @author Hugo Chargois <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <stdexcept>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <QMessageBox>

#include "consts.h"
#include "HostsWidget.h"

HostsWidget::HostsWidget(QWidget *parent)
  : QFrame(parent)
{
  hostsMgr = new HostsMgr();

  /* ====== Widgets ====== */
  table = new QTableWidget(this);
  table->setRowCount(0);
  table->setColumnCount(2);
  QStringList labels;
  labels << "Adresse IP" << "Nom d'hôte";
  table->setHorizontalHeaderLabels(labels);
  table->setShowGrid(false);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->verticalHeader()->hide();
  table->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  table->horizontalHeader()->setStretchLastSection(true);
  table->setColumnWidth(0, 100);

  QPushButton *delButton = new QPushButton("Supprimer la sélection", this);

  inputIp = new QLineEdit(this);
  inputIp->setValidator(new QRegExpValidator(ip_rx, this));
  inputIp->setEnabled(false);
  inputHostname = new QLineEdit(this);
  inputHostname->setValidator(new QRegExpValidator(host_rx, this));
  QPushButton *addButton = new QPushButton("Ajouter", this);
  localIp = new QRadioButton("Locale", this);
  localIp->setChecked(true);

  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  /* ====== Layouts ====== */
  QHBoxLayout *hb = new QHBoxLayout(this);
  QVBoxLayout *vb = new QVBoxLayout();
  QFormLayout *fl = new QFormLayout();

  hb->addWidget(table);
  hb->addLayout(vb);
  hb->setStretch(0, 7);
  hb->setStretch(1, 4);

  vb->addWidget(delButton);
  vb->addWidget(line);
  vb->addLayout(fl);

  fl->addRow("Nom d'hôte", inputHostname);
  fl->addRow("Adresse IP", localIp);
  fl->addRow("", new QRadioButton("Autre :", this));
  fl->addRow("", inputIp);
  fl->addRow(addButton);

  /* ====== Plumbing ====== */
  connect(delButton, SIGNAL(clicked()), this, SLOT(removeHost()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addHost()));
  connect(inputIp, SIGNAL(returnPressed()), this, SLOT(addHost()));
  connect(inputHostname, SIGNAL(returnPressed()), this, SLOT(addHost()));
  connect(localIp, SIGNAL(toggled(bool)), inputIp, SLOT(setDisabled(bool)));
}

void HostsWidget::addHost()
{
  QString ip;
  try {
    validateInput();

    if (localIp->isChecked()) {
      ip = hostsMgr->getLocalIp();
    } else {
      ip = inputIp->text();
    }
    QString const& name = inputHostname->text();

    hostsMgr->addHost(ip, name);
    addLine(ip, name);
  } catch (QString &e) {
    QMessageBox::warning(this, "Erreur lors de l'ajout de l'hôte", e);
  }
}

void HostsWidget::validateInput()
{
  if (! localIp->isChecked() && ! inputIp->hasAcceptableInput()) {
    throw QString("Adresse IP invalide.");
  }
  if (! inputHostname->hasAcceptableInput()) {
    throw QString("Nom d'hôte invalide.");
  }
}

void HostsWidget::removeHost()
{
  QList<int> selRows;
  QModelIndexList sel = table->selectionModel()->selectedRows();

  foreach(const QModelIndex& index, sel) {
    selRows.append(index.row());
  }

  // Sort rows in reverse order to delete from the bottom to the top,
  // so that indexes always refer to the correct initial row
  // even after deleting other rows.
  qSort(selRows.begin(), selRows.end(), qGreater<int>());

  foreach(int row, selRows) {
    QString const& name = table->item(row, 1)->text();
    if (name == "localhost") {
      if (QMessageBox::question (0, "Supprimer localhost ?",
               "Vous êtes sur le point de supprimer l'hôte local "
               "(localhost), ce qui peut avoir des effets indésirables. "
               "Confirmez-vous la suppression ?",
               QMessageBox::Yes,
               QMessageBox::No | QMessageBox::Default | QMessageBox::Escape
               ) != QMessageBox::Yes) {
        continue;
      }
    }
    hostsMgr->removeHost(name);
    table->removeRow(row);
  }
}

void HostsWidget::addLine(QString const& ip, QString const& name)
{
  int newRow = table->rowCount();
  table->setRowCount(newRow + 1);

  QTableWidgetItem *ipWidget = new QTableWidgetItem(ip);
  ipWidget->setFlags(ipWidget->flags() ^ Qt::ItemIsEditable);
  QTableWidgetItem *nameWidget = new QTableWidgetItem(name);
  nameWidget->setFlags(nameWidget->flags() ^ Qt::ItemIsEditable);

  table->setItem(newRow, 0, ipWidget);
  table->setItem(newRow, 1, nameWidget);
}

HostsWidget::HostsMgr::HostsMgr()
{
}

void HostsWidget::HostsMgr::addHost(QString const& ip, QString const& name)
{
  if (hosts.count(name) > 0) {
    throw QString("Une entrée pour cet hôte existe déjà !");
  }
  hosts.insert(std::pair<QString, QString>(name, ip));
}

void HostsWidget::HostsMgr::removeHost(QString const& name)
{
  hosts.erase(name);
}

QString HostsWidget::HostsMgr::getLocalIp()
{
  try {
    return hosts.at("localhost");
  } catch (const std::out_of_range &oor) {
    return QString("127.0.0.1");
  }
}

QString HostsWidget::HostsMgr::getHosts()
{
  QStringList output = QStringList();
  if (hosts.empty()) {
    return "";
  }
  std::map<QString, QString> transposed;
  for (std::map<QString, QString>::iterator it = hosts.begin();
      it != hosts.end(); it++) {
    QString name = it->first;
    QString ip = it->second;

    if (transposed.count(ip) > 0) {
      QString &val = transposed.at(ip);
      val += " ";
      val += name;
    } else {
      transposed.insert(std::pair<QString, QString>(ip, name));
    }
  }

  for (std::map<QString, QString>::iterator it = transposed.begin();
      it != transposed.end(); it++) {
    QString line = it->first;
    line += " ";
    line += it->second;
    output.append(line);
  }
  return output.join("\n");
}

QString HostsWidget::getHosts()
{
  return hostsMgr->getHosts();
}

void HostsWidget::setHosts(QString hosts)
{
  delete hostsMgr;
  hostsMgr = new HostsMgr();
  table->setRowCount(0);

  QStringList lines = hosts.split(QRegExp("[\r\n]"));
  int lnum = 0;
  foreach(const QString& line, lines) {
    lnum++;
    QString uncommentedLine = line.split(QRegExp("#")).at(0);
    QStringList h = uncommentedLine.split(QRegExp("[ \t]"),
        QString::SkipEmptyParts);
    if (h.count() == 0) {
      /* empty line (or full-line comment) */
      continue;
    }
    if (h.count() == 1) {
      qDebug("Error in hosts file (line %d)", lnum);
      continue;
    }
    const QString &ip = h.at(0);
    for (int j = 1; j < h.count(); j++) {
      const QString &name = h.at(j);
      try {
        hostsMgr->addHost(ip, name);
        addLine(ip, name);
      } catch (QString &e) {
        qDebug("%s", e.toStdString().c_str());
        qDebug("Error in hosts file (line %d)", lnum);
        continue;
      }
    }
  }
}

// vim:sw=2:ts=2:et:co=80:
