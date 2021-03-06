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

#include <config_features.h>


#include <helper/statusindicator.hxx>

namespace framework{



StatusIndicator::StatusIndicator(StatusIndicatorFactory* pFactory)
    : m_xFactory         (pFactory)
{
}


StatusIndicator::~StatusIndicator()
{
}


void SAL_CALL StatusIndicator::start(const OUString& sText ,
                                           sal_Int32        nRange)
    throw(css::uno::RuntimeException, std::exception)
{
#if !HAVE_FEATURE_DESKTOP
    (void) sText;
    (void) nRange;
#else
    css::uno::Reference< css::task::XStatusIndicatorFactory > xFactory(m_xFactory);
    if (xFactory.is())
    {
        StatusIndicatorFactory* pFactory = (StatusIndicatorFactory*)xFactory.get();
        pFactory->start(this, sText, nRange);
    }
#endif
}


void SAL_CALL StatusIndicator::end()
    throw(css::uno::RuntimeException, std::exception)
{
#if HAVE_FEATURE_DESKTOP
    css::uno::Reference< css::task::XStatusIndicatorFactory > xFactory(m_xFactory);
    if (xFactory.is())
    {
        StatusIndicatorFactory* pFactory = (StatusIndicatorFactory*)xFactory.get();
        pFactory->end(this);
    }
#endif
}


void SAL_CALL StatusIndicator::reset()
    throw(css::uno::RuntimeException, std::exception)
{
#if HAVE_FEATURE_DESKTOP
    css::uno::Reference< css::task::XStatusIndicatorFactory > xFactory(m_xFactory);
    if (xFactory.is())
    {
        StatusIndicatorFactory* pFactory = (StatusIndicatorFactory*)xFactory.get();
        pFactory->reset(this);
    }
#endif
}


void SAL_CALL StatusIndicator::setText(const OUString& sText)
    throw(css::uno::RuntimeException, std::exception)
{
#if !HAVE_FEATURE_DESKTOP
    (void) sText;
#else
    css::uno::Reference< css::task::XStatusIndicatorFactory > xFactory(m_xFactory);
    if (xFactory.is())
    {
        StatusIndicatorFactory* pFactory = (StatusIndicatorFactory*)xFactory.get();
        pFactory->setText(this, sText);
    }
#endif
}


void SAL_CALL StatusIndicator::setValue(sal_Int32 nValue)
    throw(css::uno::RuntimeException, std::exception)
{
#if !HAVE_FEATURE_DESKTOP
    (void) nValue;
#else
    css::uno::Reference< css::task::XStatusIndicatorFactory > xFactory(m_xFactory);
    if (xFactory.is())
    {
        StatusIndicatorFactory* pFactory = (StatusIndicatorFactory*)xFactory.get();
        pFactory->setValue(this, nValue);
    }
#endif
}

} // namespace framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
