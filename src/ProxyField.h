// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file ProxyField.h
 * clip-config single proxy field header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef PROXYFIELD_H
#define PROXYFIELD_H

#include <QLayout>

class QLineEdit;
class QSpinBox;

class ProxyField : public QHBoxLayout
{
	Q_OBJECT
public:
	ProxyField(QWidget *p, const QString &pref, bool *changed);

	void setNameValue(const QString &val);
	QString getNameValue(void);
  void setToolTip(const QString &tip);

protected slots:
	void markChanged(void) { *changed = true; };

private:
	QLineEdit *url;
	QSpinBox *port;
	QString prefix;
	bool *changed;
};

#endif 

// vim:sw=2:ts=2:et:co=80:
