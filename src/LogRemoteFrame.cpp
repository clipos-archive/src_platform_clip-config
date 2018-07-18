// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file LogRemoteFrame.cpp
 * clip-audit-config log rotation config frame
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QCheckBox>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSpinBox>
#include <ClipConfig/ClipConfigFile.h>

#include "LogRemoteFrame.h"
#include "qtcommon.h"
#include "consts.h"

static const QRegExp one_port_rx("\\d{1,5}");

LogRemoteFrame::LogRemoteFrame(QWidget *parent, const QString &path)
  : QFrame(parent), changed(false)
{
  file = new ClipConfigFile;

  edtURL = new QLineEdit(this);
  edtURL->setValidator(new QRegExpValidator(host_rx, this));
  spbPort = new QSpinBox(this);
  spbPort->setRange(1, 65535);
  spbPort->setValue(601);

  chkRemote = new QCheckBox("Activer le déport de journaux", this);
  chkTLS = new QCheckBox("Utiliser TLS", this);
  chkTLSClient = new QCheckBox("Utiliser un certificat client TLS", this);

  chkLogNet = new QCheckBox("Informations réseau", this);
  chkLogUpdate = new QCheckBox("Journaux des mises à jour", this);
  chkLogSecAlert = new QCheckBox("Alertes de sécurité", this);
  chkLogSecWarn = new QCheckBox("Avertissements de sécurité", this);
  chkLogSecInfo = new QCheckBox("Informations de sécurité", this);
  chkLogOther = new QCheckBox("Autres journaux", this);

  lblPort = new QLabel("Port d'écoute du service de collecte :", this);
  lblLogs = new QLabel("<u>Catégories de journaux à transférer :</u>", this);
  lblURL = new QLabel("Adresse du service de collecte :", this);

  connect(chkRemote, SIGNAL(stateChanged(int)), this, SLOT(updateWidgets()));
  connect(chkTLS, SIGNAL(stateChanged(int)), this, SLOT(updateWidgets()));

  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->addWidget(new QLabel("<b><u>Configuration du déport de journaux</u></b>",
                                this));
  vb->addSpacing(5);

  vb->addWidget(chkRemote);
  vb->addSpacing(4);

  QHBoxLayout *hb;
  hb = new QHBoxLayout;
  hb->addSpacing(20);
  hb->addWidget(lblURL);
  hb->addSpacing(5);
  hb->addWidget(edtURL);
  hb->addStretch(5);
  vb->addLayout(hb);
  vb->addSpacing(4);

  hb = new QHBoxLayout;
  hb->addSpacing(20);
  hb->addWidget(lblPort);
  hb->addSpacing(5);
  hb->addWidget(spbPort);
  hb->addStretch(5);
  vb->addLayout(hb);
  vb->addSpacing(4);


  vb->addWidget(chkTLS);
  vb->addSpacing(4);
  vb->addWidget(chkTLSClient);
  vb->addSpacing(4);
  vb->addWidget(lblLogs);
  vb->addSpacing(4);

#define add_chk(chk) do {\
  hb = new QHBoxLayout; \
  hb->addSpacing(20); \
  hb->addWidget(chk); \
  hb->addStretch(5); \
  vb->addLayout(hb); \
  vb->addSpacing(4); \
} while (0)

  add_chk(chkLogNet);
  add_chk(chkLogUpdate);
  add_chk(chkLogSecAlert);
  add_chk(chkLogSecWarn);
  add_chk(chkLogSecInfo);
  add_chk(chkLogOther);

  read(path);
  /* Connect only after reading - we only want to display warnings
   * when the actual user changes ticks those checkboxes.
   */
  connect(chkTLS, SIGNAL(stateChanged(int)), this, SLOT(enableTLS(int)));
  connect(chkTLSClient, SIGNAL(stateChanged(int)), 
                                          this, SLOT(enableTLSClient(int)));
}

void
LogRemoteFrame::save()
{
  update();
  file->writeFile();
}

void
LogRemoteFrame::read(const QString &path)
{
  file->readFile(path);
  QString tmp;
  bool ok;
  uint val;

  tmp = file->getNameVal("REMOTE_LOGGING", yes_rx);
  if (tmp.isEmpty()) {
    chkRemote->setChecked(false);
    updateWidgets();
    return;
  } else
    chkRemote->setChecked(true);

  tmp = file->getNameVal("REMOTE_LOG_HOST", host_rx);
  edtURL->setText(tmp);

  tmp = file->getNameVal("REMOTE_LOG_PORT", one_port_rx);
  if (!tmp.isEmpty()) {
    val = tmp.toUInt(&ok);
    if (ok)
      spbPort->setValue(val);
  }

  tmp = file->getNameVal("REMOTE_LOG_TLS", yes_rx);
  chkTLS->setChecked(!tmp.isEmpty());
  if (!tmp.isEmpty()) {
    tmp = file->getNameVal("REMOTE_LOG_TLS_CLIENTCERT", yes_rx);
    chkTLSClient->setChecked(!tmp.isEmpty());
  }

  tmp = file->getNameVal("REMOTE_LOG_NETWORKING", yes_rx);
  chkLogNet->setChecked(!tmp.isEmpty());
  tmp = file->getNameVal("REMOTE_LOG_UPDATE", yes_rx);
  chkLogUpdate->setChecked(!tmp.isEmpty());
  tmp = file->getNameVal("REMOTE_LOG_SEC_ALERT", yes_rx);
  chkLogSecAlert->setChecked(!tmp.isEmpty());
  tmp = file->getNameVal("REMOTE_LOG_SEC_WARN", yes_rx);
  chkLogSecWarn->setChecked(!tmp.isEmpty());
  tmp = file->getNameVal("REMOTE_LOG_SEC_INFO", yes_rx);
  chkLogSecInfo->setChecked(!tmp.isEmpty());
  tmp = file->getNameVal("REMOTE_LOG_OTHER", yes_rx);
  chkLogOther->setChecked(!tmp.isEmpty());

  updateWidgets();
}

void
LogRemoteFrame::update()
{
  bool on;
  on = chkRemote->isChecked();
#define yesno(var) ((var) ? yes_str : no_str)
#define yesnochk(var) (((var)->isChecked()) ? yes_str : no_str)

  file->setNameVal("REMOTE_LOGGING", yesno(on));
  if (on) {
    if (edtURL->hasAcceptableInput())
      file->setNameVal("REMOTE_LOG_HOST", edtURL->text());
    file->setNameVal("REMOTE_LOG_PORT", QString::number(spbPort->value()));
    bool tls = chkTLS->isChecked();
    file->setNameVal("REMOTE_LOG_TLS", yesno(tls));
    if (tls)
      file->setNameVal("REMOTE_LOG_TLS_CLIENTCERT", yesnochk(chkTLSClient));
    file->setNameVal("REMOTE_LOG_NETWORKING", yesnochk(chkLogNet));
    file->setNameVal("REMOTE_LOG_UPDATE", yesnochk(chkLogUpdate));
    file->setNameVal("REMOTE_LOG_SEC_ALERT", yesnochk(chkLogSecAlert));
    file->setNameVal("REMOTE_LOG_SEC_WARN", yesnochk(chkLogSecWarn));
    file->setNameVal("REMOTE_LOG_SEC_INFO", yesnochk(chkLogSecInfo));
    file->setNameVal("REMOTE_LOG_OTHER", yesnochk(chkLogOther));
  }
}

bool
LogRemoteFrame::pending()
{
  update();
  return file->pendingChanges();
}

void
LogRemoteFrame::updateWidgets()
{
  bool on = chkRemote->isChecked();
  bool tlsOn = (on && chkTLS->isChecked());

  lblURL->setEnabled(on);
  lblPort->setEnabled(on);
  lblLogs->setEnabled(on);
  edtURL->setEnabled(on);
  spbPort->setEnabled(on);
  chkTLS->setEnabled(on);
  chkTLSClient->setEnabled(tlsOn);

  chkLogNet->setEnabled(on);
  chkLogUpdate->setEnabled(on);
  chkLogSecAlert->setEnabled(on);
  chkLogSecWarn->setEnabled(on);
  chkLogSecInfo->setEnabled(on);
  chkLogOther->setEnabled(on);
}

void
LogRemoteFrame::enableTLS(int state)
{
  if (state == Qt::Checked)
    QMessageBox::information(0, "Installation des certificats",
              "Attention : l'utilisation de TLS nécessite d'installer le\n"
              "certificat de l'autorité de certification associée au\n"
              "service de collecte dans /etc/audit/cacert.",
              QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape);
}

void
LogRemoteFrame::enableTLSClient(int state)
{
  if (state == Qt::Checked)
    QMessageBox::information(0, "Installation des clés",
              "Attention : l'utilisation d'une authentification client\n"
              "TLS nécessite d'installer la clé du client et le\n"
              "certificat associé sous\n"
              "/etc/audit/keys/client_key.pem et\n"
              "/etc/audit/keys/client_cert.pem, respectivement.\n",
              QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape);
}
// vi:sw=2:ts=2:et:co=80:
