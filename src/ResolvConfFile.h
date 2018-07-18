// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file ResolConfFile.h
 * clip-config net profile resolv.conf file header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef RESOLVCONFFILE_H
#define RESOLVCONFFILE_H

#include <ClipConfig/ConfigFile.h>

#include <QString>
#include <QLinkedList>
#include <QRegExp>


class ResolvConfFileLine 
{
 public:
  ResolvConfFileLine();
  ResolvConfFileLine(const QString& line, int& n, bool new_line);

  bool getNS(int n, QString& res) const;
  bool setNS(int n, const QString& val);

  const QString& getLineAndReset();
  bool pendingChange() const;

 private:
  int NSNumber;
  QString NSValue;
  QString rawline;

  QString originalValue;
  bool isModified;
  bool isNew;
};

class ResolvConfFile : public ConfigFile 
{
 public:
  virtual QString getIndexVal(int ni, const QRegExp& re = QRegExp()) const;
  virtual void setIndexVal(int n, const QString& val);
  virtual bool pendingChanges() const;

 protected:
  virtual void resetContent();
  virtual void readLine(const QString& line);
  virtual const QStringList contentToWrite();

 private:
  int DNSCount;
  QLinkedList<ResolvConfFileLine> content;
};

#endif // RESOLVCONFFILE_H

// vi:sw=2:ts=2:et:co=80:
