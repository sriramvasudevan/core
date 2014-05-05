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

#include "sheet.hxx"
#include "patattr.hxx"
#include "docpool.hxx"
#include "formulacell.hxx"
#include "document.hxx"
#include "drwlayer.hxx"
#include "olinetab.hxx"
#include "rechead.hxx"
#include "stlpool.hxx"
#include "attarray.hxx"
#include "markdata.hxx"
#include "progress.hxx"
#include "dociter.hxx"
#include "conditio.hxx"
#include "chartlis.hxx"
#include "fillinfo.hxx"
#include "bcaslot.hxx"
#include "postit.hxx"
#include "sheetevents.hxx"
#include "globstr.hrc"
#include "segmenttree.hxx"
#include "queryparam.hxx"
#include "queryentry.hxx"
#include "dbdata.hxx"
#include "colorscale.hxx"
#include "tokenarray.hxx"
#include "clipcontext.hxx"
#include "types.hxx"
#include "editutil.hxx"
#include "mtvcellfunc.hxx"
#include "refupdatecontext.hxx"
#include "scopetools.hxx"
#include "tabprotection.hxx"
#include <rowheightcontext.hxx>
#include <refhint.hxx>

#include "scitems.hxx"
#include <editeng/boxitem.hxx>
#include "editeng/editobj.hxx"
#include <svl/poolcach.hxx>
#include <unotools/charclass.hxx>
#include <math.h>
#include <svl/PasswordHelper.hxx>
#include <unotools/transliterationwrapper.hxx>

namespace {

class ColumnRegroupFormulaCells
{
    ScColumn* mpCols;
public:
    ColumnRegroupFormulaCells(ScColumn* pCols) : mpCols(pCols) {}

    void operator() (SCCOL nCol)
    {
        mpCols[nCol].RegroupFormulaCells();
    }
};

}

void ScSheet::SetSheetEvents( const ScSheetEvents* pNew )
{
    delete pSheetEvents;
    if (pNew)
        pSheetEvents = new ScSheetEvents(*pNew);
    else
        pSheetEvents = NULL;

    SetCalcNotification( false );       // discard notifications before the events were set

    if (IsStreamValid())
        SetStreamValid(false);
}


void ScSheet::SetCalcNotification( bool bSet )
{
    bCalcNotification = bSet;
}

void ScSheet::InvalidatePageBreaks()
{
    mbPageBreaksValid = false;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
