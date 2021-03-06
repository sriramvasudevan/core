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

#include <cppuhelper/implbase3.hxx>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XChangesBatch.hpp>
#include <rtl/ref.hxx>

#include "updatecheckconfiglistener.hxx"
#include "updateinfo.hxx"

/* This helper class provides by name access to a sequence of named values */
class NamedValueByNameAccess
{
    const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& m_rValues;

public:
    NamedValueByNameAccess(
        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& rValues) :
        m_rValues(rValues) {} ;

    ~NamedValueByNameAccess();

    ::com::sun::star::uno::Any getValue(const sal_Char * pName);
};


/* This class encapsulates the configuration item actually used for storing the state
 * the update check is actually in.
 */
class UpdateCheckROModel
{
public:
    UpdateCheckROModel(NamedValueByNameAccess& aNameAccess) : m_aNameAccess(aNameAccess) {};

    bool isAutoCheckEnabled() const;
    bool isDownloadPaused() const;
    OUString getLocalFileName() const;
    sal_Int64 getDownloadSize() const;

    OUString getUpdateEntryVersion() const;
    void getUpdateEntry(UpdateInfo& rInfo) const;

private:

    OUString getStringValue(const sal_Char *) const;

    NamedValueByNameAccess& m_aNameAccess;
};



/* This class implements the non published UNO service com.sun.star.setup.UpdateCheckConfig,
 * which primary use is to be able to track changes done in the Toos -> Options page of this
 * component, as this is not supported by the OOo configuration for extendable groups.
 */

class UpdateCheckConfig : public ::cppu::WeakImplHelper3<
    ::com::sun::star::container::XNameReplace,
    ::com::sun::star::util::XChangesBatch,
    ::com::sun::star::lang::XServiceInfo >
{
    UpdateCheckConfig( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >& xContainer,
                       const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >& xAvailableUpdates,
                       const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >& xIgnoredUpdates,
                       const ::rtl::Reference< UpdateCheckConfigListener >& rListener );

    virtual ~UpdateCheckConfig();

public:

    static ::com::sun::star::uno::Sequence< OUString > getServiceNames();
    static OUString getImplName();

    static ::rtl::Reference< UpdateCheckConfig > get(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext,
        const ::rtl::Reference< UpdateCheckConfigListener >& rListener = ::rtl::Reference< UpdateCheckConfigListener >());

    // Should really implement ROModel ..
    bool isAutoCheckEnabled() const;
    bool isAutoDownloadEnabled() const;
    OUString getUpdateEntryVersion() const;

    /* Updates the timestamp of last check, but does not commit the change
     * as either clearUpdateFound() or setUpdateFound() are expected to get
     * called next.
     */
    void updateLastChecked();

    /* Returns the date of the last successful check in seconds since 1970 */
    sal_Int64 getLastChecked() const;

    /* Returns configured check interval in seconds */
    sal_Int64 getCheckInterval() const;

    /* Reset values of previously remembered update
     */
    void clearUpdateFound();

    /* Stores the specified data of an available update
     */
    void storeUpdateFound(const UpdateInfo& rInfo, const OUString& aCurrentBuild);

    // Returns the local file name of a started download
    OUString getLocalFileName() const;

    // Returns the local file name of a started download
    OUString getDownloadDestination() const;

    // stores the local file name of a just started download
    void storeLocalFileName(const OUString& rFileName, sal_Int64 nFileSize);

    // Removes the local file name of a download
    void clearLocalFileName();

    // Stores the bool value for manually paused downloads
    void storeDownloadPaused(bool paused);

    // Returns the directory that acts as the user's desktop
    static OUString getDesktopDirectory();

    // Returns a directory accessible for all users
    static OUString getAllUsersDirectory();

    // store and retrieve information about extensions
    bool storeExtensionVersion( const OUString& rExtensionName,
                                const OUString& rVersion );
    bool checkExtensionVersion( const OUString& rExtensionName,
                                const OUString& rVersion );

    // XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  )
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL hasElements(  )
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    // XNameAccess
    virtual ::com::sun::star::uno::Any SAL_CALL getByName( const OUString& aName )
        throw (::com::sun::star::container::NoSuchElementException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual ::com::sun::star::uno::Sequence< OUString > SAL_CALL getElementNames(  )
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL hasByName( const OUString& aName )
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    // XNameReplace
    virtual void SAL_CALL replaceByName( const OUString& aName, const ::com::sun::star::uno::Any& aElement )
        throw (::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::container::NoSuchElementException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    // XChangesBatch
    virtual void SAL_CALL commitChanges(  )
        throw (::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL hasPendingChanges(  )
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::util::ElementChange > SAL_CALL getPendingChanges(  )
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName()
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL supportsService(OUString const & serviceName)
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual ::com::sun::star::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

private:

    static OUString getSubVersion( const OUString& rVersion, sal_Int32 *nIndex );
    static bool isVersionGreater( const OUString& rVersion1, const OUString& rVersion2 );

    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer > m_xContainer;
    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer > m_xAvailableUpdates;
    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer > m_xIgnoredUpdates;
    const ::rtl::Reference< UpdateCheckConfigListener > m_rListener;
};




template <typename T>
T getValue( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& rNamedValues, const sal_Char * pszName )
    throw (::com::sun::star::uno::RuntimeException)
{
    for( sal_Int32 n=0; n < rNamedValues.getLength(); n++ )
    {
        // Unfortunatly gcc-3.3 does not like Any.get<T>();
        if( rNamedValues[n].Name.equalsAscii( pszName ) )
        {
            T value = T();
            if( ! (rNamedValues[n].Value >>= value) )
                throw ::com::sun::star::uno::RuntimeException(
                    OUString(
                        cppu_Any_extraction_failure_msg(
                            &rNamedValues[n].Value,
                            ::cppu::getTypeFavourUnsigned(&value).getTypeLibType() ),
                            SAL_NO_ACQUIRE ),
                    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >() );

            return value;
        }
    }

    return T();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
