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
#include <comphelper/string.hxx>
#include <svx/algitem.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/brushitem.hxx>
#include <editeng/contouritem.hxx>
#include <editeng/colritem.hxx>
#include <editeng/crossedoutitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/langitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/shdditem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/wghtitem.hxx>
#include <svx/rotmodit.hxx>
#include <editeng/editobj.hxx>
#include <editeng/editeng.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/escapementitem.hxx>
#include <svl/zforlist.hxx>
#include <vcl/keycodes.hxx>
#include <rtl/math.hxx>
#include <unotools/charclass.hxx>

#include "attrib.hxx"
#include "patattr.hxx"
#include "formulacell.hxx"
#include "sheet.hxx"
#include "globstr.hrc"
#include "global.hxx"
#include "document.hxx"
#include "autoform.hxx"
#include "userlist.hxx"
#include "zforauto.hxx"
#include "subtotal.hxx"
#include "formula/errorcodes.hxx"
#include "rangenam.hxx"
#include "docpool.hxx"
#include "progress.hxx"
#include "segmenttree.hxx"
#include "conditio.hxx"
#include "editutil.hxx"
#include <columnspanset.hxx>

#include <math.h>
#include <boost/scoped_ptr.hpp>

// STATIC DATA -----------------------------------------------------------

extern sal_uInt16 nScFillModeMouseModifier;     // global.cxx

namespace {

short lcl_DecompValueString( OUString& rValue, sal_Int32& nVal, sal_uInt16* pMinDigits = NULL )
{
    if ( rValue.isEmpty() )
    {
        nVal = 0;
        return 0;
    }
    const sal_Unicode* p = rValue.getStr();
    sal_Int32 nNeg = 0;
    sal_Int32 nNum = 0;
    if ( p[nNum] == '-' )
        nNum = nNeg = 1;
    while ( p[nNum] && CharClass::isAsciiNumeric( OUString(p[nNum]) ) )
        nNum++;

    sal_Unicode cNext = p[nNum];            // 0 if at the end
    sal_Unicode cLast = p[rValue.getLength()-1];

    // #i5550# If there are numbers at the beginning and the end,
    // prefer the one at the beginning only if it's followed by a space.
    // Otherwise, use the number at the end, to enable things like IP addresses.
    if ( nNum > nNeg && ( cNext == 0 || cNext == ' ' || !CharClass::isAsciiNumeric(OUString(cLast)) ) )
    {   // number at the beginning
        nVal = rValue.copy( 0, nNum ).toInt32();
        //  any number with a leading zero sets the minimum number of digits
        if ( p[nNeg] == '0' && pMinDigits && ( nNum - nNeg > *pMinDigits ) )
            *pMinDigits = nNum - nNeg;
        rValue = rValue.copy(nNum);
        return -1;
    }
    else
    {
        nNeg = 0;
        sal_Int32 nEnd = nNum = rValue.getLength() - 1;
        while ( nNum && CharClass::isAsciiNumeric( OUString(p[nNum]) ) )
            nNum--;
        if ( p[nNum] == '-' )
        {
            nNum--;
            nNeg = 1;
        }
        if ( nNum < nEnd - nNeg )
        {   // number at the end
            nVal = rValue.copy( nNum + 1 ).toInt32();
            //  any number with a leading zero sets the minimum number of digits
            if ( p[nNum+1+nNeg] == '0' && pMinDigits && ( nEnd - nNum - nNeg > *pMinDigits ) )
                *pMinDigits = nEnd - nNum - nNeg;
            rValue = rValue.copy(0, nNum + 1);
            return 1;
        }
    }
    nVal = 0;
    return 0;
}

OUString lcl_ValueString( sal_Int32 nValue, sal_uInt16 nMinDigits )
{
    if ( nMinDigits <= 1 )
        return OUString::number( nValue );           // simple case...
    else
    {
        OUString aStr = OUString::number( std::abs( nValue ) );
        if ( aStr.getLength() < nMinDigits )
        {
            OUStringBuffer aZero;
            comphelper::string::padToLength(aZero, nMinDigits - aStr.getLength(), '0');
            aStr = aZero.makeStringAndClear() + aStr;
        }
        //  nMinDigits doesn't include the '-' sign -> add after inserting zeros
        if ( nValue < 0 )
            aStr = "-" + aStr;
        return aStr;
    }
}

void setSuffixCell(
    ScColumn& rColumn, SCROW nRow, sal_Int32 nValue, sal_uInt16 nDigits, const OUString& rSuffix,
    CellType eCellType, bool bIsOrdinalSuffix )
{
    ScDocument& rDoc = rColumn.GetDoc();
    OUString aValue = lcl_ValueString(nValue, nDigits);
    if (!bIsOrdinalSuffix)
    {
        rColumn.SetRawString(nRow, aValue += rSuffix);
        return;
    }

    OUString aOrdinalSuffix = ScGlobal::GetOrdinalSuffix(nValue);
    if (eCellType != CELLTYPE_EDIT)
    {
        rColumn.SetRawString(nRow, aValue += aOrdinalSuffix);
        return;
    }

    EditEngine aEngine(rDoc.GetEnginePool());
    aEngine.SetEditTextObjectPool(rDoc.GetEditPool());

    SfxItemSet aAttr = aEngine.GetEmptyItemSet();
    aAttr.Put( SvxEscapementItem( SVX_ESCAPEMENT_SUPERSCRIPT, EE_CHAR_ESCAPEMENT));
    aEngine.SetText( aValue );
    aEngine.QuickInsertText(
        aOrdinalSuffix,
        ESelection(0, aValue.getLength(), 0, aValue.getLength() + aOrdinalSuffix.getLength()));

    aEngine.QuickSetAttribs(
        aAttr,
        ESelection(0, aValue.getLength(), 0, aValue.getLength() + aOrdinalSuffix.getLength()));

    // Text object instance will be owned by the cell.
    rColumn.SetEditText(nRow, aEngine.CreateTextObject());
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
