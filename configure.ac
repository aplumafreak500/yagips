# SPDX-License-Identifier: AGPL-3.0-or-later
# This file is part of yagips
# ©2023 Alex Pensinger (ArcticLuma113)
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
# You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

AC_INIT([yagips], [0.0a-dev])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-xz no-dist-gzip subdir-objects])
AM_MAINTAINER_MODE([disable])
AC_PROG_CC
AC_PROG_CXX
AC_ARG_VAR([PROTOC], [Protobuf compiler command])
AC_PATH_PROG([PROTOC], [protoc])
AS_IF([test "x${PROTOC}" == "x"], [AC_MSG_ERROR([Required program 'protoc' not found.])])
PKG_CHECK_MODULES(PROTOBUF, protobuf >= 3.12.4)
AC_SUBST(PROTOBUF_LIBS)
AC_SUBST(PROTOBUF_CFLAGS)
PKG_CHECK_MODULES(LIBCRYPTO, libcrypto >= 3.0.5)
AC_SUBST(LIBCRYPTO_LIBS)
AC_SUBST(LIBCRYPTO_CFLAGS)
PKG_CHECK_MODULES(SQLITE3, sqlite3 >= 3.37.2)
AC_SUBST(SQLITE3_LIBS)
AC_SUBST(SQLITE3_CFLAGS)
PKG_CHECK_MODULES(JSON_C, json-c >= 0.15)
AC_SUBST(JSON_C_LIBS)
AC_SUBST(JSON_C_CFLAGS)
# TODO: do we need libssl at all?
# TODO: Check required functions
# Note that we have our own config.h which is a C++ class definition for setting runtime configuration, so we need to use a different name for the config.h that Autotools generates.
AC_CONFIG_HEADERS([buildconf.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
