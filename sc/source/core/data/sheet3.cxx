/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include <rtl/math.hxx>
#include <unotools/textsearch.hxx>
#include <svl/zforlist.hxx>
#include <svl/zformat.hxx>
#include <unotools/charclass.hxx>
#include <unotools/collatorwrapper.hxx>
#include <com/sun/star/i18n/CollatorOptions.hpp>
#include <stdlib.h>
#include <unotools/transliterationwrapper.hxx>

#include "sheet.hxx"
#include "scitems.hxx"
#include "attrib.hxx"
#include "formulacell.hxx"
#include "document.hxx"
#include "globstr.hrc"
#include "global.hxx"
#include "stlpool.hxx"
#include "compiler.hxx"
#include "patattr.hxx"
#include "subtotal.hxx"
#include "docoptio.hxx"
#include "markdata.hxx"
#include "rangelst.hxx"
#include "attarray.hxx"
#include "userlist.hxx"
#include "progress.hxx"
#include "cellform.hxx"
#include "postit.hxx"
#include "queryparam.hxx"
#include "queryentry.hxx"
#include "segmenttree.hxx"
#include "subtotalparam.hxx"
#include "docpool.hxx"
#include "cellvalue.hxx"
#include "tokenarray.hxx"
#include "mtvcellfunc.hxx"
#include "columnspanset.hxx"

#include "svl/sharedstringpool.hxx"

#include <vector>
#include <boost/scoped_array.hpp>
#include <boost/unordered_set.hpp>

using namespace ::com::sun::star;

ScDocument& ScSheet::GetDoc()
{
    return *pDocument;
}

const ScDocument& ScSheet::GetDoc() const
{
    return *pDocument;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
