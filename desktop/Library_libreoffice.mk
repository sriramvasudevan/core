# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_StaticLibrary_StaticLibrary,libreoffice))

$(eval $(call gb_StaticLibrary_set_include,libreoffice,\
    $$(INCLUDE) \
    -I$(SRCDIR)/desktop/inc \
))

$(eval $(call gb_StaticLibrary_add_libs,libreoffice,\
	$(if $(filter $(OS),LINUX), \
		-ldl \
        -lpthread \
	) \
))

$(eval $(call gb_StaticLibrary_use_libraries,libreoffice,\
	$(gb_UWINAPI) \
))

$(eval $(call gb_StaticLibrary_add_cobjects,libreoffice,\
    desktop/source/lib/shim \
))

# vim: set noet sw=4 ts=4:
