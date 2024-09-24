# SPDX-License-Identifier: AGPL-3.0-or-later
# This file is part of yagips
# ©2024 Alex Pensinger (ArcticLuma113)
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
# You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

AC_INIT(
	[yagips],
	m4_esyscmd([./git-version-gen .tarball-version])
)
printf "Start configuration...\n"
AC_CONFIG_MACRO_DIRS([m4])
printf "\nAutomake init...\n\n"
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-xz no-dist-gzip subdir-objects])
AM_MAINTAINER_MODE([disable])
AM_SILENT_RULES([yes])
printf "\nChecking C and C++ compilers...\n\n"
AC_PROG_CC
AC_PROG_CXX
printf "\nChecking for required programs...\n\n"
AC_ARG_VAR([PROTOC], [Protobuf compiler command])
AC_PATH_PROG([PROTOC], [protoc])
AS_IF([test "x${PROTOC}" == "x"], [
	AC_MSG_ERROR([Required program 'protoc' not found.])
])
printf "\nGnulib early init...\n\n"
gl_EARLY
printf "\nGnulib init...\n\n"
gl_INIT

AC_C_CONST
AC_C_VOLATILE

printf "\nChecking for required libraries...\n\n"
PKG_CHECK_MODULES(PROTOBUF, protobuf >= 3.12.4, [], [
	# TODO use a suitable fallback
	AC_MSG_ERROR([Required library 'protobuf' not found.])
])
AC_SUBST(PROTOBUF_LIBS)
AC_SUBST(PROTOBUF_CFLAGS)
PKG_CHECK_MODULES(LIBCRYPTO, libcrypto >= 3.0.0, [], [
	AC_CHECK_LIB(crypto, OSSL_DECODER_CTX_new, [
		LIBCRYPTO_LIBS = "-lcrypto"
	], [
		# TODO only do this check on Windows
		AC_CHECK_LIB(eay32, OSSL_DECODER_CTX_new, [
			LIBCRYPTO_LIBS = "-leay32"
		], [
			AC_MSG_ERROR([Required library 'openssl' not found.])
		])
	])
])
AC_SUBST(LIBCRYPTO_LIBS)
AC_SUBST(LIBCRYPTO_CFLAGS)
# TODO: do we need libssl at all?
PKG_CHECK_MODULES(SQLITE3, sqlite3 >= 3.7.14, [], [
	AC_CHECK_LIB(sqlite3, sqlite3_close_v2, [
		SQLITE3_LIBS = "-lsqlite3"
	], [
		AC_MSG_ERROR([Required library 'sqlite3' not found.])
	])
])
AC_SUBST(SQLITE3_LIBS)
AC_SUBST(SQLITE3_CFLAGS)
PKG_CHECK_MODULES(JSON_C, json-c >= 0.15, [], [
	AC_CHECK_LIB(json-c, json_tokener_new, [
		JSON_C_LIBS = "-ljson-c"
	], [
		AC_MSG_ERROR([Required library 'json-c' not found.])
	])
])
AC_SUBST(JSON_C_LIBS)
AC_SUBST(JSON_C_CFLAGS)
# LevelDB does not use pkg-config (as of 2024-02-23)
AC_CHECK_LIB(leveldb, leveldb_open, [], [
	AC_MSG_ERROR([Required library 'leveldb' not found.])
])
# printf "\nChecking for required functions...\n\n"
# TODO Check more required functions, possibly from the dependencies
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile gnulib/Makefile])
printf "\nFinal output...\n\n"
AC_OUTPUT
printf "\ndone.\n"
