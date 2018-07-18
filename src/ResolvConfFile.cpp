// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file ResolConfFile.cpp
 * clip-config net profile resolv.conf file.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include "consts.h"
#include "ResolvConfFile.h"

ResolvConfFileLine::ResolvConfFileLine() 
{
  isModified = isNew = false;
  rawline = originalValue = QString::null;
}

ResolvConfFileLine::ResolvConfFileLine(const QString& line, 
                                      int& n, bool new_line) 
{
  NSNumber = -1;
  if (line.startsWith(ns_str)) {
    int index = ip_rx.indexIn(line);
    if (index != -1) {
      NSValue = line.mid(index, ip_rx.matchedLength());
      originalValue = NSValue;
      NSNumber = n++;
    }
  }
  rawline = line;
  isModified = false;
  isNew = new_line;
}

bool 
ResolvConfFileLine::getNS(int n, QString& res) const
{
  if (NSNumber > 0 && NSNumber == n) {
    res = NSValue;
    return true;
  }
  
  return false;
}

bool 
ResolvConfFileLine::setNS(int n, const QString& val)
{
  if (NSNumber > 0 && NSNumber == n) {
    NSValue = val;
    rawline = ns_str + " " + val;
    isModified = (originalValue != NSValue);
    return true;
  }
  
  return false;
}

const QString& 
ResolvConfFileLine::getLineAndReset() 
{
  originalValue = NSValue;
  isModified = false;
  isNew = false;
  return rawline;
}

bool 
ResolvConfFileLine::pendingChange() const
{
  if (isNew && NSValue == QString("0.0.0.0"))
    return false;

#if 0
  if (isNew)
    qDebug("  Line added: \"%s\"", rawline.toStdString().c_str());
  else if (isModified)
    qDebug("  DNS %d modifed from \"%s\" to \"%s\"", NSNumber, 
    originalValue.toStdString().c_str(), NSValue.toStdString().c_str());
#endif

  return isNew || isModified;
}

void 
ResolvConfFile::resetContent() 
{
  content.clear();
  DNSCount = 1;
}

void 
ResolvConfFile::readLine(const QString& line) 
{
  content.append(ResolvConfFileLine(line, DNSCount, false));
}

QString 
ResolvConfFile::getIndexVal(int n, const QRegExp& re) const 
{
  QLinkedList<ResolvConfFileLine>::const_iterator i;
  QString res;

  for (i = content.constBegin(); i != content.constEnd(); ++i) {
    if ((*i).getNS(n, res))
      return res;
  }

  return QString();
}

void 
ResolvConfFile::setIndexVal(int n, const QString& val) 
{
  QLinkedList<ResolvConfFileLine>::iterator i;

  for (i = content.begin(); i != content.end(); ++i) {
    if ((*i).setNS(n, val))
      return;
  }

  if (val != QString("0.0.0.0"))
    content.append(ResolvConfFileLine(ns_str + " " + val, n, true));
}

bool 
ResolvConfFile::pendingChanges() const 
{
  QLinkedList<ResolvConfFileLine>::const_iterator i;

  //qDebug("Checking changes in %s:", filename.toStdString().c_str());
  for (i = content.constBegin(); i != content.constEnd(); ++i) {
    if ((*i).pendingChange())
      return true;
  }

  return false;
}

const QStringList 
ResolvConfFile::contentToWrite() 
{
  QStringList res;
  QLinkedList<ResolvConfFileLine>::iterator i;

  for (i = content.begin(); i != content.end(); ++i) {
    QString line = (*i).getLineAndReset();
    if (line != QString("nameserver 0.0.0.0"))
      res.append(line);
  }
  return res;
}

// vi:sw=2:ts=2:et:co=80:
