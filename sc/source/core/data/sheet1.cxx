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

#include "scitems.hxx"
#include <svx/algitem.hxx>
#include <editeng/justifyitem.hxx>
#include <unotools/textsearch.hxx>
#include <sfx2/objsh.hxx>

#include "attrib.hxx"
#include "patattr.hxx"
#include "formulacell.hxx"
#include "sheet.hxx"
#include "document.hxx"
#include "drwlayer.hxx"
#include "olinetab.hxx"
#include "stlsheet.hxx"
#include "global.hxx"
#include "globstr.hrc"
#include "refupdat.hxx"
#include "markdata.hxx"
#include "progress.hxx"
#include "hints.hxx"
#include "prnsave.hxx"
#include "tabprotection.hxx"
#include "sheetevents.hxx"
#include "segmenttree.hxx"
#include "dbdata.hxx"
#include "colorscale.hxx"
#include "conditio.hxx"
#include "globalnames.hxx"
#include "cellvalue.hxx"
#include "scmatrix.hxx"
#include "refupdatecontext.hxx"
#include <rowheightcontext.hxx>

#include "formula/vectortoken.hxx"

#include <vector>

using ::std::vector;

ScSheet::ScSheet( ScDocument* pDoc, SCTAB nNewTab, const OUString& rNewName) :
    aName( rNewName ),
    aCodeName( rNewName ),
    nLinkRefreshDelay( 0 ),
    nLinkMode( 0 ),
    aPageStyle( ScGlobal::GetRscString(STR_STYLENAME_STANDARD) ),
    pTabProtection( NULL ),
    pOutlineTable( NULL ),
    pSheetEvents( NULL ),
    nTab( nNewTab ),
    pDocument( pDoc ),
    pSearchText ( NULL ),
    pSortCollator( NULL ),
    nLockCount( 0 ),
    pScenarioRanges( NULL ),
    aScenarioColor( COL_LIGHTGRAY ),
    aTabBgColor( COL_AUTO ),
    nScenarioFlags( 0 ),
    pDBDataNoName(NULL),
    mpRangeName(NULL),
    mpCondFormatList( new ScConditionalFormatList() ),
    bScenario(false),
    bLayoutRTL(false),
    bLoadingRTL(false),
    bPageSizeValid(false),
    bTableAreaValid(false),
    bVisible(true),
    bStreamValid(false),
    bCalcNotification(false),
    bGlobalKeepQuery(false),
    bPrintEntireSheet(true),
    bActiveScenario(false),
    mbPageBreaksValid(false),
    mbForceBreaks(false)
{

    if ( pDocument->IsDocVisible() )
    {
        //  when a sheet is added to a visible document,
        //  initialize its RTL flag from the system locale
        bLayoutRTL = ScGlobal::IsSystemRTL();
    }

    ScDrawLayer* pDrawLayer = pDocument->GetDrawLayer();
    if (pDrawLayer)
    {
        if ( pDrawLayer->ScAddPage( nTab ) )    // sal_False (not inserted) during Undo
        {
            pDrawLayer->ScRenamePage( nTab, aName );
            sal_uLong nx = (sal_uLong) ((double) (MAXCOL+1) * STD_COL_WIDTH           * HMM_PER_TWIPS );
            sal_uLong ny = (sal_uLong) ((double) (MAXROW+1) * ScGlobal::nStdRowHeight * HMM_PER_TWIPS );
            pDrawLayer->SetPageSize( static_cast<sal_uInt16>(nTab), Size( nx, ny ), false );
        }
    }

}

ScSheet::~ScSheet()
{
    if (!pDocument->IsInDtorClear())
    {
        //  nicht im dtor die Pages in der falschen Reihenfolge loeschen
        //  (nTab stimmt dann als Page-Number nicht!)
        //  In ScDocument::Clear wird hinterher per Clear am Draw Layer alles geloescht.

        ScDrawLayer* pDrawLayer = pDocument->GetDrawLayer();
        if (pDrawLayer)
            pDrawLayer->ScRemovePage( nTab );
    }

    delete pSheetEvents;
    delete pOutlineTable;
    delete pSearchText;
    delete pScenarioRanges;
    delete mpRangeName;
    delete pDBDataNoName;
}

void ScSheet::GetName( OUString& rName ) const
{
    rName = aName;
}

void ScSheet::SetName( const OUString& rNewName )
{
    aName = rNewName;
    aUpperName = OUString(); // invalidated if the name is changed

    // SetStreamValid is handled in ScDocument::RenameTab
}

const OUString& ScSheet::GetUpperName() const
{
    if (aUpperName.isEmpty() && !aName.isEmpty())
        aUpperName = ScGlobal::pCharClass->uppercase(aName);
    return aUpperName;
}

void ScSheet::SetVisible( bool bVis )
{
    if (bVisible != bVis && IsStreamValid())
        SetStreamValid(false);

    bVisible = bVis;
}

void ScSheet::SetStreamValid( bool bSet, bool bIgnoreLock )
{
    if ( bIgnoreLock || !pDocument->IsStreamValidLocked() )
        bStreamValid = bSet;
}

void ScSheet::SetLayoutRTL( bool bSet )
{
    bLayoutRTL = bSet;
}

void ScSheet::SetLoadingRTL( bool bSet )
{
    bLoadingRTL = bSet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
