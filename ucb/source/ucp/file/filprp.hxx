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
#ifndef _FILPRP_HXX_
#define _FILPRP_HXX_

#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/ucb/XContentProvider.hpp>
#include <cppuhelper/implbase1.hxx>


namespace fileaccess {

    class shell;

class XPropertySetInfo_impl : public cppu::WeakImplHelper1<
    css::beans::XPropertySetInfo >
    {
    public:
        XPropertySetInfo_impl( shell* pMyShell,const OUString& aUnqPath );
        XPropertySetInfo_impl( shell* pMyShell,const com::sun::star::uno::Sequence< com::sun::star::beans::Property >& seq );

        virtual ~XPropertySetInfo_impl();

        virtual com::sun::star::uno::Sequence< com::sun::star::beans::Property > SAL_CALL
        getProperties(
            void )
            throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        virtual com::sun::star::beans::Property SAL_CALL
        getPropertyByName(
            const OUString& aName )
            throw( com::sun::star::beans::UnknownPropertyException,
                   com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

        virtual sal_Bool SAL_CALL
        hasPropertyByName( const OUString& Name )
            throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    private:
        shell*                                                                     m_pMyShell;
        com::sun::star::uno::Reference< com::sun::star::ucb::XContentProvider >    m_xProvider;
        sal_Int32                                                                  m_count;
        com::sun::star::uno::Sequence< com::sun::star::beans::Property >           m_seq;
    };
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
