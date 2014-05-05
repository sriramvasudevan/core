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
#include "attrib.hxx"
#include "patattr.hxx"
#include "docpool.hxx"
#include "formulacell.hxx"
#include "sheet.hxx"
#include "column.hxx"
#include "document.hxx"
#include "drwlayer.hxx"
#include "olinetab.hxx"
#include "userlist.hxx"
#include "stlsheet.hxx"
#include "global.hxx"
#include "rechead.hxx"
#include "stlpool.hxx"
#include "brdcst.hxx"
#include "tabprotection.hxx"
#include "globstr.hrc"
#include "segmenttree.hxx"
#include "columniterator.hxx"
#include "globalnames.hxx"
#include "scmod.hxx"
#include "printopt.hxx"

#include <com/sun/star/sheet/TablePageBreakData.hpp>

#include <algorithm>
#include <limits>
#include <iostream>

using ::com::sun::star::uno::Sequence;
using ::com::sun::star::sheet::TablePageBreakData;
using ::std::set;

// STATIC DATA -----------------------------------------------------------

void ScSheet::SetPageSize( const Size& rSize )
{
    if ( rSize.Width() != 0 && rSize.Height() != 0 )
    {
        if (aPageSizeTwips != rSize)
            InvalidatePageBreaks();

        bPageSizeValid = true;
        aPageSizeTwips = rSize;
    }
    else
        bPageSizeValid = false;
}

bool ScSheet::IsProtected() const
{
    return pTabProtection.get() && pTabProtection->isProtected();
}

void ScSheet::SetProtection(const ScTableProtection* pProtect)
{
    if (pProtect)
        pTabProtection.reset(new ScTableProtection(*pProtect));
    else
        pTabProtection.reset();

    if (IsStreamValid())
        SetStreamValid(false);
}

ScTableProtection* ScSheet::GetProtection()
{
    return pTabProtection.get();
}

Size ScSheet::GetPageSize() const
{
    if ( bPageSizeValid )
        return aPageSizeTwips;
    else
        return Size();  // leer
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
