// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file TlsTunnelWidget.cpp
 * clip-config tls tunnel widget.
 * @author Hugo Chargois <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <QMessageBox>

#include "consts.h"
#include "TlsTunnelWidget.h"

TlsTunnelWidget::TlsTunnelWidget(QWidget *parent)
  : QFrame(parent)
{
  tunnelMgr = new TunnelMgr();

  /* ====== Widgets ====== */
  table = new QTableWidget(this);
  table->setRowCount(0);
  table->setColumnCount(4);
  QStringList labels;
  labels << "Nom" << "Port local" << "Destination" << "Port dest.";
  table->setHorizontalHeaderLabels(labels);
  table->setShowGrid(false);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->verticalHeader()->hide();
  table->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  table->setColumnWidth(0, 80);
  table->setColumnWidth(1, 80);
  table->setColumnWidth(2, 140);
  table->setColumnWidth(3, 80);

  QPushButton *delButton = new QPushButton("Supprimer la sélection", this);

  addName = new QLineEdit(this);
  addName->setValidator(new QRegExpValidator(tls_tunnel_name_rx, this));
  addPortSrc = new QSpinBox(this);
  addPortSrc->setRange(1, 65535);
  addDst = new QLineEdit(this);
  addDst->setValidator(new QRegExpValidator(host_rx, this));
  addPortDst = new QSpinBox(this);
  addPortDst->setRange(1, 65535);
  QPushButton *addButton = new QPushButton("Ajouter", this);

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

  fl->addRow("Nom", addName);
  fl->addRow("Port local", addPortSrc);
  fl->addRow("Destination", addDst);
  fl->addRow("Port dest.", addPortDst);
  fl->addRow(addButton);

  /* ====== Plumbing ====== */
  connect(delButton, SIGNAL(clicked()), this, SLOT(removeTunnel()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addTunnel()));
  connect(addName, SIGNAL(returnPressed()), this, SLOT(addTunnel()));
  connect(addDst, SIGNAL(returnPressed()), this, SLOT(addTunnel()));

}

void TlsTunnelWidget::addTunnel()
{
  try {
    validateTunnel(addName, addPortSrc, addDst, addPortDst);

    QString const& name = addName->text();
    int portSrc = addPortSrc->value();
    QString const& dst = addDst->text();
    int portDst = addPortDst->value();

    tunnelMgr->addTunnel(name, portSrc, dst, portDst);
    addLine(name, portSrc, dst, portDst);
  } catch (QString &e) {
    QMessageBox::warning(this, "Erreur lors de l'ajout du tunnel", e);
  }
}

void TlsTunnelWidget::validateTunnel(QLineEdit* name, QSpinBox* portSrc,
    QLineEdit* ipDst, QSpinBox* portDst)
{
  if (! name->hasAcceptableInput()) {
    throw QString("Caractères interdits dans le nom du tunnel.");
  }
  if (! ipDst->hasAcceptableInput()) {
    throw QString("La destination n'est pas une adresse IP "
        "ou un nom de machine valide.");
  }
}

void TlsTunnelWidget::removeTunnel()
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
    tunnelMgr->removeTunnel(table->item(row, 0)->text());
    table->removeRow(row);
  }
}

void TlsTunnelWidget::addLine(QString const& name, int portSrc,
    QString const& ipDst, int portDst)
{
  int newRow = table->rowCount();
  table->setRowCount(newRow + 1);

  QTableWidgetItem *nameWidget = new QTableWidgetItem(name);
  nameWidget->setFlags(nameWidget->flags() ^ Qt::ItemIsEditable);
  QTableWidgetItem *portSrcWidget = new QTableWidgetItem(QString::number(portSrc));
  portSrcWidget->setFlags(portSrcWidget->flags() ^ Qt::ItemIsEditable);
  QTableWidgetItem *ipDstWidget = new QTableWidgetItem(ipDst);
  ipDstWidget->setFlags(ipDstWidget->flags() ^ Qt::ItemIsEditable);
  QTableWidgetItem *portDstWidget = new QTableWidgetItem(QString::number(portDst));
  portDstWidget->setFlags(portDstWidget->flags() ^ Qt::ItemIsEditable);

  table->setItem(newRow, 0, nameWidget);
  table->setItem(newRow, 1, portSrcWidget);
  table->setItem(newRow, 2, ipDstWidget);
  table->setItem(newRow, 3, portDstWidget);
}

TlsTunnelWidget::TunnelMgr::TunnelMgr()
{
}

void TlsTunnelWidget::TunnelMgr::addTunnel(QString const& name, int portSrc,
    QString const& ipDst, int portDst)
{
  if (tunnels.count(name) > 0) {
    throw QString("Un tunnel portant le même nom existe déjà !");
  }
  Tunnel t = Tunnel(portSrc, ipDst, portDst);
  for (std::map<QString, Tunnel>::iterator it = tunnels.begin();
      it != tunnels.end(); it++) {
    if (t.hasSameSource(it->second)) {
      throw QString("Un tunnel utilisant le même port local existe déjà !");
    }
    if (t.hasSameDest(it->second)) {
      throw QString("Un tunnel vers la même destination existe déjà !");
    }
  }
  tunnels.insert(std::pair<QString, Tunnel>(name, t));
}

void TlsTunnelWidget::TunnelMgr::removeTunnel(QString const& name)
{
  tunnels.erase(name);
}

QString TlsTunnelWidget::TunnelMgr::getCSV()
{
  QString csv = QString("");
  if (tunnels.empty()) {
    return csv;
  }
  for (std::map<QString, Tunnel>::iterator it = tunnels.begin();
      it != tunnels.end(); it++) {
    csv += it->first;
    csv += ",";
    csv += it->second.getCSV();
    csv += "\n";
  }
  return csv;
}

TlsTunnelWidget::TunnelMgr::Tunnel::Tunnel(int portSrc, QString const& ipDst,
    int portDst)
  : portSrc(portSrc), ipDst(ipDst), portDst(portDst)
{
}

bool TlsTunnelWidget::TunnelMgr::Tunnel::hasSameSource(Tunnel const& other)
{
  return portSrc == other.portSrc;
}

bool TlsTunnelWidget::TunnelMgr::Tunnel::hasSameDest(Tunnel const& other)
{
  return ipDst == other.ipDst && portDst == other.portDst;
}

QString TlsTunnelWidget::TunnelMgr::Tunnel::getCSV()
{
  QString csv = QString();
  csv += QString::number(portSrc);
  csv += ",";
  csv += ipDst;
  csv += ",";
  csv += QString::number(portDst);
  return csv;
}

QString TlsTunnelWidget::getTunnels()
{
  return tunnelMgr->getCSV();
}

void TlsTunnelWidget::setTunnels(QString tunnels)
{
  delete tunnelMgr;
  tunnelMgr = new TunnelMgr();
  table->setRowCount(0);

  QStringList lines = tunnels.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
  int i = 0;
  foreach(const QString& line, lines) {
    i++;
    if (line.startsWith("#"))
      continue;
    QStringList tun = line.split(",");
    if (tun.count() != 4) {
      qDebug("Error in TLS tunnel configuration file (line %d)", i);
      continue;
    }
    bool ok1, ok2;
    QString const& name = tun[0];
    int portSrc = tun[1].toInt(&ok1);
    QString const& dst = tun[2];
    int portDst = tun[3].toInt(&ok2);

    if (!ok1 || !ok2) {
      qDebug("Error in TLS tunnel configuration file (line %d)", i);
      continue;
    }

    try {
      tunnelMgr->addTunnel(name, portSrc, dst, portDst);
      addLine(name, portSrc, dst, portDst);
    } catch (QString &e) {
      qDebug("%s", e.toStdString().c_str());
      qDebug("Error in TLS tunnel configuration file (line %d)", i);
      continue;
    }
  }
}

// vim:sw=2:ts=2:et:co=80:
