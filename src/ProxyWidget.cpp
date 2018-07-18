// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file ProxyWidget.cpp
 * clip-config proxy widget.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QLineEdit>
#include <QLabel>

#include "ProxyField.h"
#include "ProxyWidget.h"

ProxyWidget::ProxyWidget(QWidget *parent)
	: QFrame(parent), changed(false)
{
	QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(5);

	vb->addWidget(new QLabel("<i>Laissez les champs vides pour ne pas "
					"utiliser de proxy.</i>", this));
	vb->addSpacing(10);

	QGridLayout *gl = new QGridLayout();
  gl->setHorizontalSpacing(5);
  gl->setVerticalSpacing(5);

	gl->addWidget(new QLabel("<b>HTTP</b>", this), 0, 0);
	http = new ProxyField(this, "http://", &changed);
  http->setToolTip("URL du proxy HTTP (par ex. http://proxy.fr)");
  gl->addLayout(http, 0, 1);

	gl->addWidget(new QLabel("<b>HTTPS</b>", this), 1, 0);
	https = new ProxyField(this, "http://", &changed);
  https->setToolTip("URL du proxy HTTPS (par ex. http://proxy.fr)");
  gl->addLayout(https, 1, 1);

	gl->addWidget(new QLabel("<b>FTP</b>", this), 2, 0);
	ftp = new ProxyField(this, "ftp://", &changed);
  ftp->setToolTip("URL du proxy FTP (par ex. ftp://proxy.fr)");
  gl->addLayout(ftp, 2, 1);

	QLabel *l = new QLabel("<i>Pas de proxy pour :</i>", this);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	gl->addWidget(l, 3, 0);

	noProxy = new QLineEdit(this);
  noProxy->setToolTip("Serveurs auxquels l'accès se fait sans proxy\n"
                      "(liste d'URL séparées par des virgules,\n"
                      "par ex. : www.ssi.gouv.fr,google.com)");
	gl->addWidget(noProxy, 3, 1);

  vb->addLayout(gl);

	connect(noProxy, SIGNAL(textChanged(const QString &)), 
					this, SLOT(markChanged()));
}

void ProxyWidget::setNameValue(proxy_t type, const QString &val)
{
	switch (type) {
		case PROXY_HTTP:
			http->setNameValue(val);
			break;
		case PROXY_HTTPS:
			https->setNameValue(val);
			break;
		case PROXY_FTP:
			ftp->setNameValue(val);
			break;
		case PROXY_NOPROXY:
			noProxy->setText(val);
			break;
		default:
			qDebug("Unsupported type in setNameValue");
			break;
	}
}

QString ProxyWidget::getNameValue(proxy_t type)
{
	switch (type) {
		case PROXY_HTTP:
			return http->getNameValue();
			break;
		case PROXY_HTTPS:
			return https->getNameValue();
			break;
		case PROXY_FTP:
			return ftp->getNameValue();
			break;
		case PROXY_NOPROXY:
			return noProxy->text();
			break;
		default:
			qDebug("Unsupported type in getNameValue");
			return "";
			break;
	}
}

// vim:sw=2:ts=2:et:co=80:
