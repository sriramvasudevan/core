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

#ifndef SC_SHEET_HXX
#define SC_SHEET_HXX

#include <vector>
#include <utility>
#include <tools/gen.hxx>
#include <tools/color.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include "column.hxx"
#include "sortparam.hxx"
#include "compressedarray.hxx"
#include "postit.hxx"
#include "types.hxx"
#include "cellvalue.hxx"
#include "formula/types.hxx"
#include "calcmacros.hxx"

#include <set>
#include <map>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace utl {
    class TextSearch;
}

namespace com { namespace sun { namespace star {
    namespace sheet {
        struct TablePageBreakData;
    }
} } }

namespace formula { struct VectorRefArray; }
namespace sc {

struct FormulaGroupContext;
class StartListeningContext;
class EndListeningContext;
class CopyFromClipContext;
class CopyToClipContext;
class CopyToDocContext;
class MixDocContext;
class ColumnSpanSet;
class ColumnSet;
struct ColumnBlockPosition;
struct RefUpdateContext;
struct RefUpdateInsertTabContext;
struct RefUpdateDeleteTabContext;
struct RefUpdateMoveTabContext;
struct NoteEntry;
class DocumentStreamAccess;
class CellValues;
class RowHeightContext;
class CompileFormulaContext;
struct SetFormulaDirtyContext;
class RefMovedHint;

}

class SfxItemSet;
class SfxStyleSheetBase;
class SvxBoxInfoItem;
class SvxBoxItem;
class SvxSearchItem;

class ScAutoFormatData;
class ScDocument;
class ScEditDataArray;
class ScFormulaCell;
class ScOutlineTable;
class ScPrintSaverTab;
class ScProgress;
class ScRangeList;
class ScSheetEvents;
class ScSortInfoArray;
class ScConditionalFormat;
class ScConditionalFormatList;
class ScStyleSheet;
class ScTableProtection;
class ScUserListData;
struct RowInfo;
struct ScFunctionData;
struct ScLineFlags;
class CollatorWrapper;
class ScFlatUInt16RowSegments;
class ScFlatBoolRowSegments;
class ScFlatBoolColSegments;
struct ScSetStringParam;
struct ScColWidthParam;
struct ScColWidthParam;
class ScRangeName;
class ScDBData;
class ScDocumentImport;

class ScSheet : boost::noncopyable
{
protected:
    typedef ::std::vector< ScRange > ScRangeVec;


    OUString aName;
    OUString aCodeName;
    OUString aComment;

    OUString       aLinkDoc;
    OUString       aLinkFlt;
    OUString       aLinkOpt;
    OUString       aLinkTab;
    sal_uLong           nLinkRefreshDelay;
    sal_uInt8           nLinkMode;

    // page style template
    OUString   aPageStyle;
    Size            aPageSizeTwips;                 // size of the print-page

    boost::scoped_ptr<ScTableProtection> pTabProtection;


    ScOutlineTable* pOutlineTable;

    ScSheetEvents*  pSheetEvents;

    SCTAB           nTab;
    ScDocument*     pDocument;
    utl::TextSearch*    pSearchText;

    mutable OUString aUpperName;             // #i62977# filled only on demand, reset in SetName

    // sort parameter to minimize stack size of quicksort
    ScSortParam     aSortParam;
    CollatorWrapper*    pSortCollator;

    ScRangeVec      aPrintRanges;

    sal_uInt16          nLockCount;

    ScRangeList*    pScenarioRanges;
    Color           aScenarioColor;
    Color           aTabBgColor;
    sal_uInt16          nScenarioFlags;
    ScDBData*       pDBDataNoName;
    mutable ScRangeName* mpRangeName;

    boost::scoped_ptr<ScConditionalFormatList> mpCondFormatList;

    bool            bScenario:1;
    bool            bLayoutRTL:1;
    bool            bLoadingRTL:1;
    bool            bPageSizeValid:1;
    mutable bool    bTableAreaValid:1;
    bool            bVisible:1;
    bool            bStreamValid:1;
    bool            bCalcNotification:1;
    bool            bGlobalKeepQuery:1;
    bool            bPrintEntireSheet:1;
    bool            bActiveScenario:1;
    bool            mbPageBreaksValid:1;
    bool            mbForceBreaks:1;

friend class ScDocument;                    // for FillInfo
friend class ScValueIterator;
friend class ScHorizontalValueIterator;
friend class ScDBQueryDataIterator;
friend class ScCellIterator;
friend class ScQueryCellIterator;
friend class ScHorizontalCellIterator;
friend class ScHorizontalAttrIterator;
friend class ScDocAttrIterator;
friend class ScAttrRectIterator;
friend class ScColumnTextWidthIterator;
friend class ScDocumentImport;
friend class sc::DocumentStreamAccess;
friend class sc::ColumnSpanSet;
friend class sc::EditTextIterator;

public:
                ScSheet( ScDocument* pDoc, SCTAB nNewTab, const OUString& rNewName);
                ~ScSheet();

    ScDocument& GetDoc();
    const ScDocument& GetDoc() const;

    const ScSheetEvents* GetSheetEvents() const              { return pSheetEvents; }
    void        SetSheetEvents( const ScSheetEvents* pNew );

    bool        IsVisible() const                            { return bVisible; }
    void        SetVisible( bool bVis );

    bool        IsStreamValid() const                        { return bStreamValid; }
    void        SetStreamValid( bool bSet, bool bIgnoreLock = false );

    bool        GetCalcNotification() const                  { return bCalcNotification; }
    void        SetCalcNotification( bool bSet );

    bool        IsLayoutRTL() const                          { return bLayoutRTL; }
    bool        IsLoadingRTL() const                         { return bLoadingRTL; }
    void        SetLayoutRTL( bool bSet );
    void        SetLoadingRTL( bool bSet );

    void        GetName( OUString& rName ) const;
    void        SetName( const OUString& rNewName );

    const OUString& GetUpperName() const;

    const OUString&   GetPageStyle() const                    { return aPageStyle; }

    bool            IsProtected() const;
    void            SetProtection(const ScTableProtection* pProtect);
    ScTableProtection* GetProtection();

    Size            GetPageSize() const;
    void            SetPageSize( const Size& rSize );

    void        InvalidatePageBreaks();

    void        ScReplaceTabsStr( OUString& rStr, const OUString& rSrch, const OUString& rRepl ); // from sw

    bool            IsPrintEntireSheet() const          { return bPrintEntireSheet; }

};


#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
