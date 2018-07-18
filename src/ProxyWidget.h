// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file ProxyWidget.h
 * clip-config proxy widget header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef PROXYWIDGET_H
#define PROXYWIDGET_H

#include <QFrame>

class QLineEdit;
class ProxyField;

typedef enum {
	PROXY_HTTP = 0,
	PROXY_HTTPS = 1,
	PROXY_FTP = 2,
	PROXY_NOPROXY = 3,
} proxy_t;

class ProxyWidget : public QFrame
{
	Q_OBJECT
public:
	ProxyWidget(QWidget *parent = 0);
	void setNameValue(proxy_t type, const QString &val);
	QString getNameValue(proxy_t type);
	void setChanged(bool b) { changed = b; };
	bool isChanged(void) { return changed; };

protected slots:
	void markChanged(void) { changed = true; };

private:
	bool changed;

	ProxyField *http;
	ProxyField *https;
	ProxyField *ftp;
	QLineEdit *noProxy;
};

#endif // PROXYWIDGET_H

// vim:sw=2:ts=2:et:co=80:
