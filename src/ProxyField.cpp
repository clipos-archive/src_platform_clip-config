// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file ProxyField.cpp
 * clip-config single proxy field.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>

#include "ProxyField.h"

ProxyField::ProxyField(QWidget *parent, const QString &pref, bool *changed)
	: QHBoxLayout(), prefix(pref), changed(changed)
{
  setSpacing(6);
	addWidget(new QLabel("URL : ", parent), 0);
	url = new QLineEdit(parent);
	url->setMinimumWidth(300);
	addWidget(url, 2, Qt::AlignCenter|Qt::AlignVCenter);
	addSpacing(8);
	addWidget(new QLabel("port : ", parent), 0);
	port = new QSpinBox(parent);
  port->setRange(1, 65535);
	addWidget(port, 2, Qt::AlignCenter|Qt::AlignVCenter);

	connect(url, SIGNAL(textChanged(const QString &)), 
					this, SLOT(markChanged()));
}

void
ProxyField::setNameValue(const QString &val)
{
	if (val.isEmpty()) {
		url->setText("");
		port->setValue(8080);
		return;
	}
	QString u = val.section(':', 0, -2);
	bool ok;
	int p = val.section(':', -1).toInt(&ok, 10);
	if (!ok) {
		qDebug("port conversion error");
		p = 8080;
	}

	url->setText(u);
	port->setValue(p);
}

QString
ProxyField::getNameValue(void)
{
	QString val = url->text();
	if (val.isEmpty())
		return val;
	if (!val.startsWith(prefix))
		val.insert(0, prefix);
	val.append(':');
	val.append(port->text());

	return val;
}

void
ProxyField::setToolTip(const QString &tip)
{
  url->setToolTip(tip);
}

// vim:sw=2:ts=2:et:co=80:
