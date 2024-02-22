# SPDX-License-Identifier: AGPL-3.0-or-later
# This file is part of yagips
# ©2024 Alex Pensinger (ArcticLuma113)
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
# You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

SUBDIRS = gnulib src
dist_doc_DATA = Readme.md Changelog License.md License-Contrib.md
nobase_dist_data_DATA = keys/regionListSeed.bin keys/signingKey.pem keys/authKey.pem
EXTRA_DIST = include m4/gnulib-cache.m4 $(top_srcdir)/.version
BUILT_SOURCES = $(top_srcdir)/.version

$(top_srcdir)/.version:
	echo '$(VERSION)' > $@-t
	mv $@-t $@

dist-hook:
	echo '$(VERSION)' > $(distdir)/.tarball-version
