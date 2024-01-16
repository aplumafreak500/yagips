# SPDX-License-Identifier: AGPL-3.0-or-later
# This file is part of yagips
# ©2023 Alex Pensinger (ArcticLuma113)
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
# You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(builddir)/proto -DCONFDIR=\"$(sysconfdir)/yagips\" -DTOPCONFDIR=\"$(sysconfdir)\" -DDATADIR=\"$(datadir)/yagips\" -DVARDIR=\"$(localstatedir)/lib/yagips\" -DLOGDIR=\"$(localstatedir)/log\"
AM_CFLAGS = $(PROTOBUF_CFLAGS) $(LIBCRYPTO_CFLAGS) $(SQLITE3_CFLAGS) $(JSON_C_CFLAGS)
AM_CXXFLAGS = $(PROTOBUF_CFLAGS) $(LIBCRYPTO_CFLAGS) $(SQLITE3_CFLAGS) $(JSON_C_CFLAGS)
bin_PROGRAMS = yagips
yagips_SOURCES = yagips.cpp gameserver.cpp session.cpp account.cpp kcpsession.cpp dispatch.cpp dbgate.cpp config.cpp ec2b.cpp util.cpp http.c toml.c mt19937-64.c crypt.c kcp.c keys.c
nodist_yagips_SOURCES = $(builddir)/proto/dispatch.pb.cc
yagips_LDADD = $(PROTOBUF_LIBS) $(LIBCRYPTO_LIBS) $(SQLITE3_LIBS) $(JSON_C_LIBS)
BUILT_SOURCES = $(builddir)/proto/dispatch.pb.h
CLEANFILES = $(builddir)/proto/dispatch.pb.cc $(builddir)/proto/dispatch.pb.h
dist_noinst_DATA = proto/dispatch.proto

%.pb.cc %.pb.h: %.proto
	$(PROTOC) --proto_path=$(srcdir)/proto --cpp_out=$(builddir)/proto $^
