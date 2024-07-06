# DO NOT EDIT! GENERATED AUTOMATICALLY!
# Copyright (C) 2002-2024 Free Software Foundation, Inc.
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this file.  If not, see <https://www.gnu.org/licenses/>.
#
# As a special exception to the GNU General Public License,
# this file may be distributed as part of a program that
# contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
#
# This file represents the compiled summary of the specification in
# gnulib-cache.m4. It lists the computed macro invocations that need
# to be invoked from configure.ac.
# In projects that use version control, this file can be treated like
# other built files.


# This macro should be invoked from ./configure.ac, in the section
# "Checks for programs", right after AC_PROG_CC, and certainly before
# any checks for libraries, header files, types and library functions.
AC_DEFUN([gl_EARLY],
[
  m4_pattern_forbid([^gl_[A-Z]])dnl the gnulib macro namespace
  m4_pattern_allow([^gl_ES$])dnl a valid locale name
  m4_pattern_allow([^gl_LIBOBJS$])dnl a variable
  m4_pattern_allow([^gl_LTLIBOBJS$])dnl a variable

  # Pre-early section.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_PROG_AR_RANLIB])

  # Code from module absolute-header:
  # Code from module accept:
  # Code from module alignasof:
  # Code from module alloca:
  # Code from module alloca-opt:
  # Code from module arpa_inet:
  # Code from module assert-h:
  # Code from module attribute:
  # Code from module bind:
  # Code from module builtin-expect:
  # Code from module c-ctype:
  # Code from module c99:
  # Code from module calloc-gnu:
  # Code from module calloc-posix:
  # Code from module cloexec:
  # Code from module close:
  # Code from module dup2:
  # Code from module errno:
  # Code from module extensions:
  # Code from module extern-inline:
  # Code from module fclose:
  # Code from module fcntl:
  # Code from module fcntl-h:
  # Code from module fd-hook:
  # Code from module fflush:
  AC_REQUIRE([gl_SET_LARGEFILE_SOURCE])
  # Code from module filename:
  # Code from module float:
  # Code from module fopen:
  # Code from module fopen-gnu:
  # Code from module fpieee:
  AC_REQUIRE([gl_FP_IEEE])
  # Code from module fprintf-gnu:
  # Code from module fprintf-posix:
  # Code from module fpucw:
  # Code from module fpurge:
  # Code from module freading:
  # Code from module free-posix:
  # Code from module frexp-nolibm:
  # Code from module frexpl-nolibm:
  # Code from module fseek:
  # Code from module fseeko:
  AC_REQUIRE([gl_SET_LARGEFILE_SOURCE])
  # Code from module fseterr:
  # Code from module fstat:
  # Code from module ftell:
  # Code from module ftello:
  AC_REQUIRE([gl_SET_LARGEFILE_SOURCE])
  # Code from module gen-header:
  # Code from module getdtablesize:
  # Code from module getopt-gnu:
  # Code from module getopt-posix:
  # Code from module getpeername:
  # Code from module getrandom:
  # Code from module gettext-h:
  # Code from module gettimeofday:
  # Code from module idx:
  # Code from module include_next:
  # Code from module inet_ntop:
  # Code from module inet_pton:
  # Code from module intprops:
  # Code from module inttypes-incomplete:
  # Code from module isnand-nolibm:
  # Code from module isnanf-nolibm:
  # Code from module isnanl-nolibm:
  # Code from module largefile:
  AC_REQUIRE([AC_SYS_LARGEFILE])
  # Code from module limits-h:
  # Code from module listen:
  # Code from module lseek:
  # Code from module malloc-gnu:
  # Code from module malloc-posix:
  # Code from module malloca:
  # Code from module math:
  # Code from module mbszero:
  # Code from module memchr:
  # Code from module minmax:
  # Code from module mixin/printf-gnu:
  # Code from module mixin/printf-posix:
  # Code from module msvc-inval:
  # Code from module msvc-nothrow:
  # Code from module multiarch:
  # Code from module nanosleep:
  # Code from module netinet_in:
  # Code from module nocrash:
  # Code from module open:
  # Code from module pathmax:
  # Code from module printf-frexp:
  # Code from module printf-frexpl:
  # Code from module printf-gnu:
  # Code from module printf-posix:
  # Code from module printf-safe:
  # Code from module pselect:
  # Code from module pthread-h:
  gl_ANYTHREADLIB_EARLY
  # Code from module pthread-thread:
  # Code from module pthread_sigmask:
  # Code from module raise:
  # Code from module recv:
  # Code from module recvfrom:
  # Code from module sched:
  # Code from module select:
  # Code from module send:
  # Code from module sendto:
  # Code from module setsockopt:
  # Code from module sigaction:
  # Code from module signal-h:
  # Code from module signbit:
  # Code from module sigprocmask:
  # Code from module size_max:
  # Code from module snippet/_Noreturn:
  # Code from module snippet/arg-nonnull:
  # Code from module snippet/c++defs:
  # Code from module snippet/warn-on-use:
  # Code from module snprintf:
  # Code from module snprintf-gnu:
  # Code from module snprintf-posix:
  # Code from module socket:
  # Code from module socketlib:
  # Code from module sockets:
  # Code from module socklen:
  # Code from module sprintf-gnu:
  # Code from module sprintf-posix:
  # Code from module ssize_t:
  # Code from module stat:
  # Code from module stat-time:
  # Code from module std-gnu11:
  # Code from module stdbool:
  # Code from module stdckdint:
  # Code from module stddef:
  # Code from module stdint:
  # Code from module stdio:
  gl_STDIO_H_EARLY
  # Code from module stdlib:
  # Code from module strcase:
  # Code from module strerror:
  # Code from module strerror-override:
  # Code from module string:
  # Code from module strings:
  # Code from module strnlen:
  # Code from module strsep:
  # Code from module strstr:
  # Code from module strstr-simple:
  # Code from module strtod:
  # Code from module strtold:
  # Code from module sys_random:
  # Code from module sys_select:
  # Code from module sys_socket:
  # Code from module sys_stat:
  # Code from module sys_time:
  # Code from module sys_types:
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])
  # Code from module sys_uio:
  # Code from module threadlib:
  gl_THREADLIB_EARLY
  # Code from module time:
  # Code from module time-h:
  # Code from module unistd:
  # Code from module vararrays:
  # Code from module vasnprintf:
  # Code from module vfprintf-posix:
  # Code from module vfzprintf:
  # Code from module vsnzprintf:
  # Code from module vszprintf:
  # Code from module wchar:
  # Code from module windows-once:
  # Code from module windows-thread:
  # Code from module windows-tls:
  # Code from module xalloc-oversized:
  # Code from module xsize:
])

# This macro should be invoked from ./configure.ac, in the section
# "Check for header files, types and library functions".
AC_DEFUN([gl_INIT],
[
  AC_CONFIG_LIBOBJ_DIR([gnulib])
  AM_CONDITIONAL([GL_COND_LIBTOOL], [false])
  gl_cond_libtool=false
  gl_libdeps=
  gl_ltlibdeps=
  gl_m4_base='m4'
  m4_pushdef([AC_LIBOBJ], m4_defn([gl_LIBOBJ]))
  m4_pushdef([AC_REPLACE_FUNCS], m4_defn([gl_REPLACE_FUNCS]))
  m4_pushdef([AC_LIBSOURCES], m4_defn([gl_LIBSOURCES]))
  m4_pushdef([gl_LIBSOURCES_LIST], [])
  m4_pushdef([gl_LIBSOURCES_DIR], [])
  m4_pushdef([GL_MACRO_PREFIX], [gl])
  m4_pushdef([GL_MODULE_INDICATOR_PREFIX], [GL])
  gl_COMMON
  gl_source_base='gnulib'
  gl_source_base_prefix=
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_ACCEPT], [test "$ac_cv_header_winsock2_h" = yes])
  gl_SYS_SOCKET_MODULE_INDICATOR([accept])
  gl_ALIGNASOF
  gl_FUNC_ALLOCA
  gl_CONDITIONAL_HEADER([alloca.h])
  AC_PROG_MKDIR_P
  gl_ARPA_INET_H
  gl_ARPA_INET_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_ASSERT_H
  gl_CONDITIONAL_HEADER([assert.h])
  AC_PROG_MKDIR_P
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_BIND], [test "$ac_cv_header_winsock2_h" = yes])
  gl_SYS_SOCKET_MODULE_INDICATOR([bind])
  gl___BUILTIN_EXPECT
  gl_FUNC_CALLOC_GNU
  if test $REPLACE_CALLOC_FOR_CALLOC_GNU = 1; then
    AC_LIBOBJ([calloc])
  fi
  gl_STDLIB_MODULE_INDICATOR([calloc-gnu])
  gl_FUNC_CALLOC_POSIX
  if test $REPLACE_CALLOC_FOR_CALLOC_POSIX = 1; then
    AC_LIBOBJ([calloc])
  fi
  gl_STDLIB_MODULE_INDICATOR([calloc-posix])
  gl_MODULE_INDICATOR_FOR_TESTS([cloexec])
  gl_FUNC_CLOSE
  gl_CONDITIONAL([GL_COND_OBJ_CLOSE], [test $REPLACE_CLOSE = 1])
  gl_UNISTD_MODULE_INDICATOR([close])
  gl_FUNC_DUP2
  gl_CONDITIONAL([GL_COND_OBJ_DUP2], [test $REPLACE_DUP2 = 1])
  AM_COND_IF([GL_COND_OBJ_DUP2], [
    gl_PREREQ_DUP2
  ])
  gl_UNISTD_MODULE_INDICATOR([dup2])
  gl_HEADER_ERRNO_H
  gl_CONDITIONAL_HEADER([errno.h])
  AC_PROG_MKDIR_P
  AC_REQUIRE([gl_EXTERN_INLINE])
  gl_FUNC_FCLOSE
  gl_CONDITIONAL([GL_COND_OBJ_FCLOSE], [test $REPLACE_FCLOSE = 1])
  gl_STDIO_MODULE_INDICATOR([fclose])
  gl_FUNC_FCNTL
  gl_CONDITIONAL([GL_COND_OBJ_FCNTL],
                 [test $HAVE_FCNTL = 0 || test $REPLACE_FCNTL = 1])
  gl_FCNTL_MODULE_INDICATOR([fcntl])
  gl_FCNTL_H
  gl_FCNTL_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_FUNC_FFLUSH
  gl_CONDITIONAL([GL_COND_OBJ_FFLUSH], [test $REPLACE_FFLUSH = 1])
  AM_COND_IF([GL_COND_OBJ_FFLUSH], [
    gl_PREREQ_FFLUSH
  ])
  gl_MODULE_INDICATOR([fflush])
  gl_STDIO_MODULE_INDICATOR([fflush])
  gl_FLOAT_H
  gl_CONDITIONAL_HEADER([float.h])
  AC_PROG_MKDIR_P
  gl_CONDITIONAL([GL_COND_OBJ_FLOAT], [test $REPLACE_FLOAT_LDBL = 1])
  gl_CONDITIONAL([GL_COND_OBJ_ITOLD], [test $REPLACE_ITOLD = 1])
  gl_FUNC_FOPEN
  if test $REPLACE_FOPEN = 1; then
    AC_LIBOBJ([fopen])
    gl_PREREQ_FOPEN
  fi
  gl_STDIO_MODULE_INDICATOR([fopen])
  gl_FUNC_FOPEN_GNU
  if test $REPLACE_FOPEN_FOR_FOPEN_GNU = 1; then
    AC_LIBOBJ([fopen])
    gl_PREREQ_FOPEN
  fi
  gl_MODULE_INDICATOR([fopen-gnu])
  gl_STDIO_MODULE_INDICATOR([fopen-gnu])
  gl_FUNC_FPRINTF_GNU
  gl_FUNC_FPRINTF_POSIX
  gl_STDIO_MODULE_INDICATOR([fprintf-posix])
  gl_FUNC_FPURGE
  gl_CONDITIONAL([GL_COND_OBJ_FPURGE],
                 [test $HAVE_FPURGE = 0 || test $REPLACE_FPURGE = 1])
  gl_STDIO_MODULE_INDICATOR([fpurge])
  gl_FUNC_FREADING
  gl_FUNC_FREE
  gl_CONDITIONAL([GL_COND_OBJ_FREE], [test $REPLACE_FREE = 1])
  AM_COND_IF([GL_COND_OBJ_FREE], [
    gl_PREREQ_FREE
  ])
  gl_STDLIB_MODULE_INDICATOR([free-posix])
  gl_FUNC_FREXP_NO_LIBM
  if test $gl_func_frexp_no_libm != yes; then
    AC_LIBOBJ([frexp])
  fi
  gl_MATH_MODULE_INDICATOR([frexp])
  gl_FUNC_FREXPL_NO_LIBM
  if test $HAVE_DECL_FREXPL = 0 || test $gl_func_frexpl_no_libm = no; then
    AC_LIBOBJ([frexpl])
  fi
  gl_MATH_MODULE_INDICATOR([frexpl])
  gl_FUNC_FSEEK
  gl_CONDITIONAL([GL_COND_OBJ_FSEEK], [test $REPLACE_FSEEK = 1])
  gl_STDIO_MODULE_INDICATOR([fseek])
  gl_FUNC_FSEEKO
  gl_CONDITIONAL([GL_COND_OBJ_FSEEKO],
                 [test $HAVE_FSEEKO = 0 || test $REPLACE_FSEEKO = 1])
  AM_COND_IF([GL_COND_OBJ_FSEEKO], [
    gl_PREREQ_FSEEKO
  ])
  gl_STDIO_MODULE_INDICATOR([fseeko])
  gl_FUNC_FSETERR
  gl_CONDITIONAL([GL_COND_OBJ_FSETERR], [test $ac_cv_func___fseterr = no])
  gl_FUNC_FSTAT
  gl_CONDITIONAL([GL_COND_OBJ_FSTAT], [test $REPLACE_FSTAT = 1])
  AM_COND_IF([GL_COND_OBJ_FSTAT], [
    case "$host_os" in
      mingw* | windows*)
        AC_LIBOBJ([stat-w32])
        ;;
    esac
    gl_PREREQ_FSTAT
  ])
  gl_SYS_STAT_MODULE_INDICATOR([fstat])
  gl_FUNC_FTELL
  gl_CONDITIONAL([GL_COND_OBJ_FTELL], [test $REPLACE_FTELL = 1])
  gl_STDIO_MODULE_INDICATOR([ftell])
  gl_FUNC_FTELLO
  gl_CONDITIONAL([GL_COND_OBJ_FTELLO],
                 [test $HAVE_FTELLO = 0 || test $REPLACE_FTELLO = 1])
  AM_COND_IF([GL_COND_OBJ_FTELLO], [
    gl_PREREQ_FTELLO
  ])
  gl_STDIO_MODULE_INDICATOR([ftello])
  gl_FUNC_GETDTABLESIZE
  gl_CONDITIONAL([GL_COND_OBJ_GETDTABLESIZE],
                 [test $HAVE_GETDTABLESIZE = 0 || test $REPLACE_GETDTABLESIZE = 1])
  AM_COND_IF([GL_COND_OBJ_GETDTABLESIZE], [
    gl_PREREQ_GETDTABLESIZE
  ])
  gl_UNISTD_MODULE_INDICATOR([getdtablesize])
  gl_FUNC_GETOPT_GNU
  dnl Because of the way gl_FUNC_GETOPT_GNU is implemented (the gl_getopt_required
  dnl mechanism), there is no need to do any AC_LIBOBJ or AC_SUBST here; they are
  dnl done in the getopt-posix module.
  gl_FUNC_GETOPT_POSIX
  gl_CONDITIONAL_HEADER([getopt.h])
  gl_CONDITIONAL_HEADER([getopt-cdefs.h])
  AC_PROG_MKDIR_P
  gl_CONDITIONAL([GL_COND_OBJ_GETOPT], [test $REPLACE_GETOPT = 1])
  AM_COND_IF([GL_COND_OBJ_GETOPT], [
    dnl Define the substituted variable GNULIB_UNISTD_H_GETOPT to 1.
    gl_UNISTD_H_REQUIRE_DEFAULTS
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_UNISTD_H_GETOPT], [1])
  ])
  gl_UNISTD_MODULE_INDICATOR([getopt-posix])
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_GETPEERNAME],
                 [test "$ac_cv_header_winsock2_h" = yes])
  gl_SYS_SOCKET_MODULE_INDICATOR([getpeername])
  AC_REQUIRE([AC_CANONICAL_HOST])
  gl_FUNC_GETRANDOM
  gl_CONDITIONAL([GL_COND_OBJ_GETRANDOM],
                 [test $HAVE_GETRANDOM = 0 || test $REPLACE_GETRANDOM = 1])
  gl_SYS_RANDOM_MODULE_INDICATOR([getrandom])
  AC_SUBST([LIBINTL])
  AC_SUBST([LTLIBINTL])
  gl_FUNC_GETTIMEOFDAY
  gl_CONDITIONAL([GL_COND_OBJ_GETTIMEOFDAY],
                 [test $HAVE_GETTIMEOFDAY = 0 || test $REPLACE_GETTIMEOFDAY = 1])
  AM_COND_IF([GL_COND_OBJ_GETTIMEOFDAY], [
    gl_PREREQ_GETTIMEOFDAY
  ])
  gl_SYS_TIME_MODULE_INDICATOR([gettimeofday])
  gl_FUNC_INET_NTOP
  gl_CONDITIONAL([GL_COND_OBJ_INET_NTOP],
                 [test $HAVE_INET_NTOP = 0 || test $REPLACE_INET_NTOP = 1])
  AM_COND_IF([GL_COND_OBJ_INET_NTOP], [
    gl_PREREQ_INET_NTOP
  ])
  gl_ARPA_INET_MODULE_INDICATOR([inet_ntop])
  gl_FUNC_INET_PTON
  gl_CONDITIONAL([GL_COND_OBJ_INET_PTON],
                 [test $HAVE_INET_PTON = 0 || test $REPLACE_INET_PTON = 1])
  AM_COND_IF([GL_COND_OBJ_INET_PTON], [
    gl_PREREQ_INET_PTON
  ])
  gl_ARPA_INET_MODULE_INDICATOR([inet_pton])
  gl_INTTYPES_INCOMPLETE
  gl_INTTYPES_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_FUNC_ISNAND_NO_LIBM
  if test $gl_func_isnand_no_libm != yes; then
    AC_LIBOBJ([isnand])
    gl_PREREQ_ISNAND
  fi
  gl_FUNC_ISNANF_NO_LIBM
  if test $gl_func_isnanf_no_libm != yes; then
    AC_LIBOBJ([isnanf])
    gl_PREREQ_ISNANF
  fi
  gl_FUNC_ISNANL_NO_LIBM
  if test $gl_func_isnanl_no_libm != yes; then
    AC_LIBOBJ([isnanl])
    gl_PREREQ_ISNANL
  fi
  AC_REQUIRE([gl_LARGEFILE])
  gl_LIMITS_H
  gl_CONDITIONAL_HEADER([limits.h])
  AC_PROG_MKDIR_P
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_LISTEN], [test "$ac_cv_header_winsock2_h" = yes])
  gl_SYS_SOCKET_MODULE_INDICATOR([listen])
  gl_FUNC_LSEEK
  gl_CONDITIONAL([GL_COND_OBJ_LSEEK], [test $REPLACE_LSEEK = 1])
  gl_UNISTD_MODULE_INDICATOR([lseek])
  gl_FUNC_MALLOC_GNU
  if test $REPLACE_MALLOC_FOR_MALLOC_GNU = 1; then
    AC_LIBOBJ([malloc])
  fi
  gl_STDLIB_MODULE_INDICATOR([malloc-gnu])
  AC_REQUIRE([gl_FUNC_MALLOC_POSIX])
  if test $REPLACE_MALLOC_FOR_MALLOC_POSIX = 1; then
    AC_LIBOBJ([malloc])
  fi
  gl_STDLIB_MODULE_INDICATOR([malloc-posix])
  gl_MALLOCA
  gl_MATH_H
  gl_MATH_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  gl_MBSTATE_T_BROKEN
  gl_MUSL_LIBC
  gl_WCHAR_MODULE_INDICATOR([mbszero])
  gl_FUNC_MEMCHR
  gl_CONDITIONAL([GL_COND_OBJ_MEMCHR], [test $REPLACE_MEMCHR = 1])
  AM_COND_IF([GL_COND_OBJ_MEMCHR], [
    gl_PREREQ_MEMCHR
  ])
  gl_STRING_MODULE_INDICATOR([memchr])
  gl_MINMAX
  # Conditionally invoke gl_PREREQ_VASNPRINTF_WITH_GNU_EXTRAS.
  # Conditionally invoke gl_PREREQ_VASNPRINTF_WITH_POSIX_EXTRAS.
  AC_REQUIRE([gl_MSVC_INVAL])
  gl_CONDITIONAL([GL_COND_OBJ_MSVC_INVAL],
                 [test $HAVE_MSVC_INVALID_PARAMETER_HANDLER = 1])
  AC_REQUIRE([gl_MSVC_NOTHROW])
  gl_CONDITIONAL([GL_COND_OBJ_MSVC_NOTHROW],
                 [test $HAVE_MSVC_INVALID_PARAMETER_HANDLER = 1])
  gl_MODULE_INDICATOR([msvc-nothrow])
  gl_MULTIARCH
  gl_FUNC_NANOSLEEP
  gl_CONDITIONAL([GL_COND_OBJ_NANOSLEEP],
                 [test $HAVE_NANOSLEEP = 0 || test $REPLACE_NANOSLEEP = 1])
  gl_TIME_MODULE_INDICATOR([nanosleep])
  gl_HEADER_NETINET_IN
  gl_CONDITIONAL_HEADER([netinet/in.h])
  AC_PROG_MKDIR_P
  gl_FUNC_OPEN
  gl_CONDITIONAL([GL_COND_OBJ_OPEN], [test $REPLACE_OPEN = 1])
  AM_COND_IF([GL_COND_OBJ_OPEN], [
    gl_PREREQ_OPEN
  ])
  gl_FCNTL_MODULE_INDICATOR([open])
  gl_PATHMAX
  gl_FUNC_PRINTF_FREXP
  gl_FUNC_PRINTF_FREXPL
  gl_FUNC_PRINTF_GNU
  gl_FUNC_PRINTF_POSIX
  gl_STDIO_MODULE_INDICATOR([printf-posix])
  m4_divert_text([INIT_PREPARE], [gl_printf_safe=yes])
  gl_FUNC_PSELECT
  gl_CONDITIONAL([GL_COND_OBJ_PSELECT],
                 [test $HAVE_PSELECT = 0 || test $REPLACE_PSELECT = 1])
  gl_SYS_SELECT_MODULE_INDICATOR([pselect])
  gl_PTHREAD_H
  gl_PTHREAD_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_PTHREAD_THREAD
  gl_CONDITIONAL([GL_COND_OBJ_PTHREAD_THREAD],
                 [test $HAVE_PTHREAD_CREATE = 0 || test $REPLACE_PTHREAD_CREATE = 1])
  gl_PTHREAD_MODULE_INDICATOR([pthread-thread])
  gl_FUNC_PTHREAD_SIGMASK
  gl_CONDITIONAL([GL_COND_OBJ_PTHREAD_SIGMASK],
                 [test $HAVE_PTHREAD_SIGMASK = 0 || test $REPLACE_PTHREAD_SIGMASK = 1])
  AM_COND_IF([GL_COND_OBJ_PTHREAD_SIGMASK], [
    gl_PREREQ_PTHREAD_SIGMASK
  ])
  gl_SIGNAL_MODULE_INDICATOR([pthread_sigmask])
  gl_FUNC_RAISE
  gl_CONDITIONAL([GL_COND_OBJ_RAISE],
                 [test $HAVE_RAISE = 0 || test $REPLACE_RAISE = 1])
  AM_COND_IF([GL_COND_OBJ_RAISE], [
    gl_PREREQ_RAISE
  ])
  gl_SIGNAL_MODULE_INDICATOR([raise])
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_RECV], [test "$ac_cv_header_winsock2_h" = yes])
  gl_SYS_SOCKET_MODULE_INDICATOR([recv])
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_RECVFROM], [test "$ac_cv_header_winsock2_h" = yes])
  gl_SYS_SOCKET_MODULE_INDICATOR([recvfrom])
  gl_SCHED_H
  gl_SCHED_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_FUNC_SELECT
  gl_CONDITIONAL([GL_COND_OBJ_SELECT], [test $REPLACE_SELECT = 1])
  gl_SYS_SELECT_MODULE_INDICATOR([select])
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_SEND], [test "$ac_cv_header_winsock2_h" = yes])
  gl_SYS_SOCKET_MODULE_INDICATOR([send])
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_SENDTO], [test "$ac_cv_header_winsock2_h" = yes])
  gl_SYS_SOCKET_MODULE_INDICATOR([sendto])
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_SETSOCKOPT],
                 [test "$ac_cv_header_winsock2_h" = yes])
  gl_SYS_SOCKET_MODULE_INDICATOR([setsockopt])
  gl_SIGACTION
  gl_CONDITIONAL([GL_COND_OBJ_SIGACTION], [test $HAVE_SIGACTION = 0])
  AM_COND_IF([GL_COND_OBJ_SIGACTION], [
    gl_PREREQ_SIGACTION
  ])
  gl_SIGNAL_MODULE_INDICATOR([sigaction])
  gl_SIGNAL_H
  gl_SIGNAL_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_SIGNBIT
  gl_CONDITIONAL([GL_COND_OBJ_SIGNBIT3], [test $REPLACE_SIGNBIT = 1])
  gl_MATH_MODULE_INDICATOR([signbit])
  gl_SIGNALBLOCKING
  gl_CONDITIONAL([GL_COND_OBJ_SIGPROCMASK], [test $HAVE_POSIX_SIGNALBLOCKING = 0])
  AM_COND_IF([GL_COND_OBJ_SIGPROCMASK], [
    gl_PREREQ_SIGPROCMASK
  ])
  gl_SIGNAL_MODULE_INDICATOR([sigprocmask])
  gl_SIZE_MAX
  gl_FUNC_SNPRINTF
  gl_STDIO_MODULE_INDICATOR([snprintf])
  gl_MODULE_INDICATOR([snprintf])
  gl_FUNC_SNPRINTF_GNU
  gl_FUNC_SNPRINTF_POSIX
  AC_REQUIRE([gl_SYS_SOCKET_H])
  gl_CONDITIONAL([GL_COND_OBJ_SOCKET], [test "$ac_cv_header_winsock2_h" = yes])
  # When this module is used, sockets may actually occur as file descriptors,
  # hence it is worth warning if the modules 'close' and 'ioctl' are not used.
  m4_ifdef([gl_UNISTD_H_DEFAULTS], [gl_UNISTD_H_REQUIRE_DEFAULTS])
  m4_ifdef([gl_SYS_IOCTL_H_DEFAULTS], [gl_SYS_IOCTL_H_REQUIRE_DEFAULTS])
  AC_REQUIRE([gl_PREREQ_SYS_H_WINSOCK2])
  if test "$ac_cv_header_winsock2_h" = yes; then
    UNISTD_H_HAVE_WINSOCK2_H_AND_USE_SOCKETS=1
    SYS_IOCTL_H_HAVE_WINSOCK2_H_AND_USE_SOCKETS=1
  fi
  gl_SYS_SOCKET_MODULE_INDICATOR([socket])
  AC_REQUIRE([gl_SOCKETLIB])
  AC_REQUIRE([gl_SOCKETS])
  gl_TYPE_SOCKLEN_T
  gl_FUNC_SPRINTF_GNU
  gl_FUNC_SPRINTF_POSIX
  gl_STDIO_MODULE_INDICATOR([sprintf-posix])
  gt_TYPE_SSIZE_T
  gl_FUNC_STAT
  gl_CONDITIONAL([GL_COND_OBJ_STAT], [test $REPLACE_STAT = 1])
  AM_COND_IF([GL_COND_OBJ_STAT], [
    case "$host_os" in
      mingw* | windows*)
        AC_LIBOBJ([stat-w32])
        ;;
    esac
    gl_PREREQ_STAT
  ])
  gl_SYS_STAT_MODULE_INDICATOR([stat])
  gl_STAT_TIME
  gl_STAT_BIRTHTIME
  gl_C_BOOL
  AC_CHECK_HEADERS_ONCE([stdckdint.h])
  if test $ac_cv_header_stdckdint_h = yes; then
    GL_GENERATE_STDCKDINT_H=false
  else
    GL_GENERATE_STDCKDINT_H=true
  fi
  gl_CONDITIONAL_HEADER([stdckdint.h])
  AC_PROG_MKDIR_P
  gl_STDDEF_H
  gl_STDDEF_H_REQUIRE_DEFAULTS
  gl_CONDITIONAL_HEADER([stddef.h])
  AC_PROG_MKDIR_P
  gl_STDINT_H
  gl_CONDITIONAL_HEADER([stdint.h])
  dnl Because of gl_REPLACE_LIMITS_H:
  gl_CONDITIONAL_HEADER([limits.h])
  AC_PROG_MKDIR_P
  gl_STDIO_H
  gl_STDIO_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_CONDITIONAL([GL_COND_OBJ_STDIO_READ], [test $REPLACE_STDIO_READ_FUNCS = 1])
  gl_CONDITIONAL([GL_COND_OBJ_STDIO_WRITE], [test $REPLACE_STDIO_WRITE_FUNCS = 1])
  dnl No need to create extra modules for these functions. Everyone who uses
  dnl <stdio.h> likely needs them.
  gl_STDIO_MODULE_INDICATOR([fscanf])
  gl_MODULE_INDICATOR([fscanf])
  gl_STDIO_MODULE_INDICATOR([scanf])
  gl_MODULE_INDICATOR([scanf])
  gl_STDIO_MODULE_INDICATOR([fgetc])
  gl_STDIO_MODULE_INDICATOR([getc])
  gl_STDIO_MODULE_INDICATOR([getchar])
  gl_STDIO_MODULE_INDICATOR([fgets])
  gl_STDIO_MODULE_INDICATOR([fread])
  dnl No need to create extra modules for these functions. Everyone who uses
  dnl <stdio.h> likely needs them.
  gl_STDIO_MODULE_INDICATOR([fprintf])
  gl_STDIO_MODULE_INDICATOR([printf])
  gl_STDIO_MODULE_INDICATOR([vfprintf])
  gl_STDIO_MODULE_INDICATOR([vprintf])
  gl_STDIO_MODULE_INDICATOR([fputc])
  gl_STDIO_MODULE_INDICATOR([putc])
  gl_STDIO_MODULE_INDICATOR([putchar])
  gl_STDIO_MODULE_INDICATOR([fputs])
  gl_STDIO_MODULE_INDICATOR([puts])
  gl_STDIO_MODULE_INDICATOR([fwrite])
  gl_STDLIB_H
  gl_STDLIB_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_STRCASE
  gl_CONDITIONAL([GL_COND_OBJ_STRCASECMP], [test $HAVE_STRCASECMP = 0])
  AM_COND_IF([GL_COND_OBJ_STRCASECMP], [
    gl_PREREQ_STRCASECMP
  ])
  gl_CONDITIONAL([GL_COND_OBJ_STRNCASECMP], [test $HAVE_STRNCASECMP = 0])
  AM_COND_IF([GL_COND_OBJ_STRNCASECMP], [
    gl_PREREQ_STRNCASECMP
  ])
  gl_FUNC_STRERROR
  gl_CONDITIONAL([GL_COND_OBJ_STRERROR], [test $REPLACE_STRERROR = 1])
  gl_MODULE_INDICATOR([strerror])
  gl_STRING_MODULE_INDICATOR([strerror])
  AC_REQUIRE([gl_HEADER_ERRNO_H])
  AC_REQUIRE([gl_FUNC_STRERROR_0])
  gl_CONDITIONAL([GL_COND_OBJ_STRERROR_OVERRIDE],
                 [test -n "$ERRNO_H" || test $REPLACE_STRERROR_0 = 1])
  AM_COND_IF([GL_COND_OBJ_STRERROR_OVERRIDE], [
    gl_PREREQ_SYS_H_WINSOCK2
  ])
  gl_STRING_H
  gl_STRING_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_STRINGS_H
  gl_STRINGS_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_FUNC_STRNLEN
  gl_CONDITIONAL([GL_COND_OBJ_STRNLEN],
                 [test $HAVE_DECL_STRNLEN = 0 || test $REPLACE_STRNLEN = 1])
  AM_COND_IF([GL_COND_OBJ_STRNLEN], [
    gl_PREREQ_STRNLEN
  ])
  gl_STRING_MODULE_INDICATOR([strnlen])
  gl_FUNC_STRSEP
  gl_CONDITIONAL([GL_COND_OBJ_STRSEP], [test $HAVE_STRSEP = 0])
  AM_COND_IF([GL_COND_OBJ_STRSEP], [
    gl_PREREQ_STRSEP
  ])
  gl_STRING_MODULE_INDICATOR([strsep])
  gl_FUNC_STRSTR
  if test $REPLACE_STRSTR = 1; then
    AC_LIBOBJ([strstr])
  fi
  gl_FUNC_STRSTR_SIMPLE
  if test $REPLACE_STRSTR = 1; then
    AC_LIBOBJ([strstr])
  fi
  gl_STRING_MODULE_INDICATOR([strstr])
  gl_FUNC_STRTOD
  gl_CONDITIONAL([GL_COND_OBJ_STRTOD],
                 [test $HAVE_STRTOD = 0 || test $REPLACE_STRTOD = 1])
  AM_COND_IF([GL_COND_OBJ_STRTOD], [
    gl_PREREQ_STRTOD
  ])
  gl_STDLIB_MODULE_INDICATOR([strtod])
  gl_FUNC_STRTOLD
  gl_CONDITIONAL([GL_COND_OBJ_STRTOLD],
                 [test $HAVE_STRTOLD = 0 || test $REPLACE_STRTOLD = 1])
  AM_COND_IF([GL_COND_OBJ_STRTOLD], [
    gl_PREREQ_STRTOLD
  ])
  gl_STDLIB_MODULE_INDICATOR([strtold])
  gl_SYS_RANDOM_H
  gl_SYS_RANDOM_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_SYS_SELECT_H
  gl_SYS_SELECT_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_SYS_SOCKET_H
  gl_SYS_SOCKET_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_SYS_STAT_H
  gl_SYS_STAT_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_SYS_TIME_H
  gl_SYS_TIME_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_SYS_TYPES_H
  gl_SYS_TYPES_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_SYS_UIO_H
  gl_SYS_UIO_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  AC_REQUIRE([gl_THREADLIB])
  gl_FUNC_TIME
  gl_CONDITIONAL([GL_COND_OBJ_TIME], [test $REPLACE_TIME = 1])
  AM_COND_IF([GL_COND_OBJ_TIME], [
    gl_PREREQ_TIME
  ])
  gl_TIME_MODULE_INDICATOR([time])
  gl_TIME_H
  gl_TIME_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  gl_UNISTD_H
  gl_UNISTD_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  AC_C_VARARRAYS
  AC_REQUIRE([AC_C_RESTRICT])
  gl_FUNC_VASNPRINTF
  gl_FUNC_VFPRINTF_POSIX
  gl_STDIO_MODULE_INDICATOR([vfprintf-posix])
  gl_MODULE_INDICATOR([vfprintf-posix])
  gl_STDIO_MODULE_INDICATOR([vfzprintf])
  gl_STDIO_MODULE_INDICATOR([vsnzprintf])
  gl_STDIO_MODULE_INDICATOR([vszprintf])
  gl_WCHAR_H
  gl_WCHAR_H_REQUIRE_DEFAULTS
  AC_PROG_MKDIR_P
  AC_REQUIRE([AC_CANONICAL_HOST])
  gl_CONDITIONAL([GL_COND_OBJ_WINDOWS_ONCE],
                 [case "$host_os" in mingw* | windows*) true;; *) false;; esac])
  AC_REQUIRE([AC_CANONICAL_HOST])
  gl_CONDITIONAL([GL_COND_OBJ_WINDOWS_THREAD],
                 [case "$host_os" in mingw* | windows*) true;; *) false;; esac])
  AC_REQUIRE([AC_CANONICAL_HOST])
  gl_CONDITIONAL([GL_COND_OBJ_WINDOWS_TLS],
                 [case "$host_os" in mingw* | windows*) true;; *) false;; esac])
  gl_XSIZE
  # End of code from modules
  m4_ifval(gl_LIBSOURCES_LIST, [
    m4_syscmd([test ! -d ]m4_defn([gl_LIBSOURCES_DIR])[ ||
      for gl_file in ]gl_LIBSOURCES_LIST[ ; do
        if test ! -r ]m4_defn([gl_LIBSOURCES_DIR])[/$gl_file ; then
          echo "missing file ]m4_defn([gl_LIBSOURCES_DIR])[/$gl_file" >&2
          exit 1
        fi
      done])dnl
      m4_if(m4_sysval, [0], [],
        [AC_FATAL([expected source file, required through AC_LIBSOURCES, not found])])
  ])
  m4_popdef([GL_MODULE_INDICATOR_PREFIX])
  m4_popdef([GL_MACRO_PREFIX])
  m4_popdef([gl_LIBSOURCES_DIR])
  m4_popdef([gl_LIBSOURCES_LIST])
  m4_popdef([AC_LIBSOURCES])
  m4_popdef([AC_REPLACE_FUNCS])
  m4_popdef([AC_LIBOBJ])
  AC_CONFIG_COMMANDS_PRE([
    gl_libobjs=
    gl_ltlibobjs=
    gl_libobjdeps=
    if test -n "$gl_LIBOBJS"; then
      # Remove the extension.
changequote(,)dnl
      sed_drop_objext='s/\.o$//;s/\.obj$//'
      sed_dirname1='s,//*,/,g'
      sed_dirname2='s,\(.\)/$,\1,'
      sed_dirname3='s,^[^/]*$,.,'
      sed_dirname4='s,\(.\)/[^/]*$,\1,'
      sed_basename1='s,.*/,,'
changequote([, ])dnl
      for i in `for i in $gl_LIBOBJS; do echo "$i"; done | sed -e "$sed_drop_objext" | sort | uniq`; do
        gl_libobjs="$gl_libobjs $i.$ac_objext"
        gl_ltlibobjs="$gl_ltlibobjs $i.lo"
        i_dir=`echo "$i" | sed -e "$sed_dirname1" -e "$sed_dirname2" -e "$sed_dirname3" -e "$sed_dirname4"`
        i_base=`echo "$i" | sed -e "$sed_basename1"`
        gl_libobjdeps="$gl_libobjdeps $i_dir/\$(DEPDIR)/$i_base.Po"
      done
    fi
    AC_SUBST([gl_LIBOBJS], [$gl_libobjs])
    AC_SUBST([gl_LTLIBOBJS], [$gl_ltlibobjs])
    AC_SUBST([gl_LIBOBJDEPS], [$gl_libobjdeps])
  ])
  gltests_libdeps=
  gltests_ltlibdeps=
  m4_pushdef([AC_LIBOBJ], m4_defn([gltests_LIBOBJ]))
  m4_pushdef([AC_REPLACE_FUNCS], m4_defn([gltests_REPLACE_FUNCS]))
  m4_pushdef([AC_LIBSOURCES], m4_defn([gltests_LIBSOURCES]))
  m4_pushdef([gltests_LIBSOURCES_LIST], [])
  m4_pushdef([gltests_LIBSOURCES_DIR], [])
  m4_pushdef([GL_MACRO_PREFIX], [gltests])
  m4_pushdef([GL_MODULE_INDICATOR_PREFIX], [GL])
  gl_COMMON
  gl_source_base='tests'
  gl_source_base_prefix=
changequote(,)dnl
  gltests_WITNESS=IN_`echo "${PACKAGE-$PACKAGE_TARNAME}" | LC_ALL=C tr abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ | LC_ALL=C sed -e 's/[^A-Z0-9_]/_/g'`_GNULIB_TESTS
changequote([, ])dnl
  AC_SUBST([gltests_WITNESS])
  gl_module_indicator_condition=$gltests_WITNESS
  m4_pushdef([gl_MODULE_INDICATOR_CONDITION], [$gl_module_indicator_condition])
  m4_popdef([gl_MODULE_INDICATOR_CONDITION])
  m4_ifval(gltests_LIBSOURCES_LIST, [
    m4_syscmd([test ! -d ]m4_defn([gltests_LIBSOURCES_DIR])[ ||
      for gl_file in ]gltests_LIBSOURCES_LIST[ ; do
        if test ! -r ]m4_defn([gltests_LIBSOURCES_DIR])[/$gl_file ; then
          echo "missing file ]m4_defn([gltests_LIBSOURCES_DIR])[/$gl_file" >&2
          exit 1
        fi
      done])dnl
      m4_if(m4_sysval, [0], [],
        [AC_FATAL([expected source file, required through AC_LIBSOURCES, not found])])
  ])
  m4_popdef([GL_MODULE_INDICATOR_PREFIX])
  m4_popdef([GL_MACRO_PREFIX])
  m4_popdef([gltests_LIBSOURCES_DIR])
  m4_popdef([gltests_LIBSOURCES_LIST])
  m4_popdef([AC_LIBSOURCES])
  m4_popdef([AC_REPLACE_FUNCS])
  m4_popdef([AC_LIBOBJ])
  AC_CONFIG_COMMANDS_PRE([
    gltests_libobjs=
    gltests_ltlibobjs=
    gltests_libobjdeps=
    if test -n "$gltests_LIBOBJS"; then
      # Remove the extension.
changequote(,)dnl
      sed_drop_objext='s/\.o$//;s/\.obj$//'
      sed_dirname1='s,//*,/,g'
      sed_dirname2='s,\(.\)/$,\1,'
      sed_dirname3='s,^[^/]*$,.,'
      sed_dirname4='s,\(.\)/[^/]*$,\1,'
      sed_basename1='s,.*/,,'
changequote([, ])dnl
      for i in `for i in $gltests_LIBOBJS; do echo "$i"; done | sed -e "$sed_drop_objext" | sort | uniq`; do
        gltests_libobjs="$gltests_libobjs $i.$ac_objext"
        gltests_ltlibobjs="$gltests_ltlibobjs $i.lo"
        i_dir=`echo "$i" | sed -e "$sed_dirname1" -e "$sed_dirname2" -e "$sed_dirname3" -e "$sed_dirname4"`
        i_base=`echo "$i" | sed -e "$sed_basename1"`
        gltests_libobjdeps="$gltests_libobjdeps $i_dir/\$(DEPDIR)/$i_base.Po"
      done
    fi
    AC_SUBST([gltests_LIBOBJS], [$gltests_libobjs])
    AC_SUBST([gltests_LTLIBOBJS], [$gltests_ltlibobjs])
    AC_SUBST([gltests_LIBOBJDEPS], [$gltests_libobjdeps])
  ])
  AC_REQUIRE([gl_CC_GNULIB_WARNINGS])
  LIBGNU_LIBDEPS="$gl_libdeps"
  AC_SUBST([LIBGNU_LIBDEPS])
  LIBGNU_LTLIBDEPS="$gl_ltlibdeps"
  AC_SUBST([LIBGNU_LTLIBDEPS])
])

# Like AC_LIBOBJ, except that the module name goes
# into gl_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gl_LIBOBJ], [
  AS_LITERAL_IF([$1], [gl_LIBSOURCES([$1.c])])dnl
  gl_LIBOBJS="$gl_LIBOBJS $1.$ac_objext"
])

# Like AC_REPLACE_FUNCS, except that the module name goes
# into gl_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gl_REPLACE_FUNCS], [
  m4_foreach_w([gl_NAME], [$1], [AC_LIBSOURCES(gl_NAME[.c])])dnl
  AC_CHECK_FUNCS([$1], , [gl_LIBOBJ($ac_func)])
])

# Like AC_LIBSOURCES, except the directory where the source file is
# expected is derived from the gnulib-tool parameterization,
# and alloca is special cased (for the alloca-opt module).
# We could also entirely rely on EXTRA_lib..._SOURCES.
AC_DEFUN([gl_LIBSOURCES], [
  m4_foreach([_gl_NAME], [$1], [
    m4_if(_gl_NAME, [alloca.c], [], [
      m4_define([gl_LIBSOURCES_DIR], [gnulib])
      m4_append([gl_LIBSOURCES_LIST], _gl_NAME, [ ])
    ])
  ])
])

# Like AC_LIBOBJ, except that the module name goes
# into gltests_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gltests_LIBOBJ], [
  AS_LITERAL_IF([$1], [gltests_LIBSOURCES([$1.c])])dnl
  gltests_LIBOBJS="$gltests_LIBOBJS $1.$ac_objext"
])

# Like AC_REPLACE_FUNCS, except that the module name goes
# into gltests_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gltests_REPLACE_FUNCS], [
  m4_foreach_w([gl_NAME], [$1], [AC_LIBSOURCES(gl_NAME[.c])])dnl
  AC_CHECK_FUNCS([$1], , [gltests_LIBOBJ($ac_func)])
])

# Like AC_LIBSOURCES, except the directory where the source file is
# expected is derived from the gnulib-tool parameterization,
# and alloca is special cased (for the alloca-opt module).
# We could also entirely rely on EXTRA_lib..._SOURCES.
AC_DEFUN([gltests_LIBSOURCES], [
  m4_foreach([_gl_NAME], [$1], [
    m4_if(_gl_NAME, [alloca.c], [], [
      m4_define([gltests_LIBSOURCES_DIR], [tests])
      m4_append([gltests_LIBSOURCES_LIST], _gl_NAME, [ ])
    ])
  ])
])

# This macro records the list of files which have been installed by
# gnulib-tool and may be removed by future gnulib-tool invocations.
AC_DEFUN([gl_FILE_LIST], [
  lib/_Noreturn.h
  lib/accept.c
  lib/alloca.c
  lib/alloca.in.h
  lib/arg-nonnull.h
  lib/arpa_inet.in.h
  lib/asnprintf.c
  lib/assert.in.h
  lib/attribute.h
  lib/bind.c
  lib/c++defs.h
  lib/c-ctype.c
  lib/c-ctype.h
  lib/calloc.c
  lib/cloexec.c
  lib/cloexec.h
  lib/close.c
  lib/dup2.c
  lib/errno.in.h
  lib/fclose.c
  lib/fcntl.c
  lib/fcntl.in.h
  lib/fd-hook.c
  lib/fd-hook.h
  lib/fflush.c
  lib/filename.h
  lib/float+.h
  lib/float.c
  lib/float.in.h
  lib/fopen.c
  lib/fprintf.c
  lib/fpucw.h
  lib/fpurge.c
  lib/freading.c
  lib/freading.h
  lib/free.c
  lib/frexp.c
  lib/frexpl.c
  lib/fseek.c
  lib/fseeko.c
  lib/fseterr.c
  lib/fseterr.h
  lib/fstat.c
  lib/ftell.c
  lib/ftello.c
  lib/getdtablesize.c
  lib/getopt-cdefs.in.h
  lib/getopt-core.h
  lib/getopt-ext.h
  lib/getopt-pfx-core.h
  lib/getopt-pfx-ext.h
  lib/getopt.c
  lib/getopt.in.h
  lib/getopt1.c
  lib/getopt_int.h
  lib/getpeername.c
  lib/getrandom.c
  lib/gettext.h
  lib/gettimeofday.c
  lib/glthread/threadlib.c
  lib/idx.h
  lib/inet_ntop.c
  lib/inet_pton.c
  lib/intprops-internal.h
  lib/intprops.h
  lib/inttypes.in.h
  lib/isnan.c
  lib/isnand-nolibm.h
  lib/isnand.c
  lib/isnanf-nolibm.h
  lib/isnanf.c
  lib/isnanl-nolibm.h
  lib/isnanl.c
  lib/itold.c
  lib/limits.in.h
  lib/listen.c
  lib/lseek.c
  lib/malloc.c
  lib/malloca.c
  lib/malloca.h
  lib/math.c
  lib/math.in.h
  lib/mbszero.c
  lib/memchr.c
  lib/memchr.valgrind
  lib/minmax.h
  lib/msvc-inval.c
  lib/msvc-inval.h
  lib/msvc-nothrow.c
  lib/msvc-nothrow.h
  lib/nanosleep.c
  lib/netinet_in.in.h
  lib/open.c
  lib/pathmax.h
  lib/printf-args.c
  lib/printf-args.h
  lib/printf-frexp.c
  lib/printf-frexp.h
  lib/printf-frexpl.c
  lib/printf-frexpl.h
  lib/printf-parse.c
  lib/printf-parse.h
  lib/printf.c
  lib/pselect.c
  lib/pthread-thread.c
  lib/pthread.in.h
  lib/pthread_sigmask.c
  lib/raise.c
  lib/recv.c
  lib/recvfrom.c
  lib/sched.in.h
  lib/select.c
  lib/send.c
  lib/sendto.c
  lib/setsockopt.c
  lib/sig-handler.c
  lib/sig-handler.h
  lib/sigaction.c
  lib/signal.in.h
  lib/signbitd.c
  lib/signbitf.c
  lib/signbitl.c
  lib/sigprocmask.c
  lib/size_max.h
  lib/snprintf.c
  lib/socket.c
  lib/sockets.c
  lib/sockets.h
  lib/sprintf.c
  lib/stat-time.c
  lib/stat-time.h
  lib/stat-w32.c
  lib/stat-w32.h
  lib/stat.c
  lib/stdckdint.in.h
  lib/stddef.in.h
  lib/stdint.in.h
  lib/stdio-impl.h
  lib/stdio-read.c
  lib/stdio-write.c
  lib/stdio.in.h
  lib/stdlib.in.h
  lib/str-two-way.h
  lib/strcasecmp.c
  lib/strerror-override.c
  lib/strerror-override.h
  lib/strerror.c
  lib/string.in.h
  lib/strings.in.h
  lib/strncasecmp.c
  lib/strnlen.c
  lib/strsep.c
  lib/strstr.c
  lib/strtod.c
  lib/strtold.c
  lib/sys_random.in.h
  lib/sys_select.in.h
  lib/sys_socket.c
  lib/sys_socket.in.h
  lib/sys_stat.in.h
  lib/sys_time.in.h
  lib/sys_types.in.h
  lib/sys_uio.in.h
  lib/time.c
  lib/time.in.h
  lib/unistd.c
  lib/unistd.in.h
  lib/vasnprintf.c
  lib/vasnprintf.h
  lib/verify.h
  lib/vfprintf.c
  lib/vfzprintf.c
  lib/vsnzprintf.c
  lib/vszprintf.c
  lib/w32sock.h
  lib/warn-on-use.h
  lib/wchar.in.h
  lib/windows-once.c
  lib/windows-once.h
  lib/windows-thread.c
  lib/windows-thread.h
  lib/windows-tls.c
  lib/windows-tls.h
  lib/xalloc-oversized.h
  lib/xsize.c
  lib/xsize.h
  m4/00gnulib.m4
  m4/absolute-header.m4
  m4/alloca.m4
  m4/arpa_inet_h.m4
  m4/asm-underscore.m4
  m4/assert_h.m4
  m4/builtin-expect.m4
  m4/c-bool.m4
  m4/calloc.m4
  m4/close.m4
  m4/codeset.m4
  m4/dup2.m4
  m4/eealloc.m4
  m4/errno_h.m4
  m4/exponentd.m4
  m4/exponentf.m4
  m4/exponentl.m4
  m4/extensions.m4
  m4/extern-inline.m4
  m4/fclose.m4
  m4/fcntl-o.m4
  m4/fcntl.m4
  m4/fcntl_h.m4
  m4/fflush.m4
  m4/float_h.m4
  m4/fopen.m4
  m4/fpieee.m4
  m4/fprintf-gnu.m4
  m4/fprintf-posix.m4
  m4/fpurge.m4
  m4/freading.m4
  m4/free.m4
  m4/frexp.m4
  m4/frexpl.m4
  m4/fseek.m4
  m4/fseeko.m4
  m4/fseterr.m4
  m4/fstat.m4
  m4/ftell.m4
  m4/ftello.m4
  m4/getdtablesize.m4
  m4/getopt.m4
  m4/getrandom.m4
  m4/gettimeofday.m4
  m4/gnulib-common.m4
  m4/include_next.m4
  m4/inet_ntop.m4
  m4/inet_pton.m4
  m4/intmax_t.m4
  m4/inttypes.m4
  m4/inttypes_h.m4
  m4/isnand.m4
  m4/isnanf.m4
  m4/isnanl.m4
  m4/largefile.m4
  m4/ldexp.m4
  m4/ldexpl.m4
  m4/limits-h.m4
  m4/locale-fr.m4
  m4/locale-ja.m4
  m4/locale-zh.m4
  m4/lseek.m4
  m4/malloc.m4
  m4/malloca.m4
  m4/math_h.m4
  m4/mbrtowc.m4
  m4/mbstate_t.m4
  m4/memchr.m4
  m4/minmax.m4
  m4/mmap-anon.m4
  m4/mode_t.m4
  m4/msvc-inval.m4
  m4/msvc-nothrow.m4
  m4/multiarch.m4
  m4/musl.m4
  m4/nanosleep.m4
  m4/netinet_in_h.m4
  m4/nocrash.m4
  m4/off64_t.m4
  m4/off_t.m4
  m4/open-cloexec.m4
  m4/open-slash.m4
  m4/open.m4
  m4/pathmax.m4
  m4/pid_t.m4
  m4/printf-frexp.m4
  m4/printf-frexpl.m4
  m4/printf-gnu.m4
  m4/printf-posix.m4
  m4/printf.m4
  m4/pselect.m4
  m4/pthread-spin.m4
  m4/pthread-thread.m4
  m4/pthread_h.m4
  m4/pthread_sigmask.m4
  m4/raise.m4
  m4/sched_h.m4
  m4/select.m4
  m4/sigaction.m4
  m4/signal_h.m4
  m4/signalblocking.m4
  m4/signbit.m4
  m4/size_max.m4
  m4/snprintf-gnu.m4
  m4/snprintf-posix.m4
  m4/snprintf.m4
  m4/socketlib.m4
  m4/sockets.m4
  m4/socklen.m4
  m4/sockpfaf.m4
  m4/sprintf-gnu.m4
  m4/sprintf-posix.m4
  m4/ssize_t.m4
  m4/stat-time.m4
  m4/stat.m4
  m4/std-gnu11.m4
  m4/stdalign.m4
  m4/stddef_h.m4
  m4/stdint.m4
  m4/stdint_h.m4
  m4/stdio_h.m4
  m4/stdlib_h.m4
  m4/strcase.m4
  m4/strerror.m4
  m4/string_h.m4
  m4/strings_h.m4
  m4/strnlen.m4
  m4/strsep.m4
  m4/strstr.m4
  m4/strtod.m4
  m4/strtold.m4
  m4/sys_cdefs_h.m4
  m4/sys_random_h.m4
  m4/sys_select_h.m4
  m4/sys_socket_h.m4
  m4/sys_stat_h.m4
  m4/sys_time_h.m4
  m4/sys_types_h.m4
  m4/sys_uio_h.m4
  m4/threadlib.m4
  m4/time.m4
  m4/time_h.m4
  m4/ungetc.m4
  m4/unistd_h.m4
  m4/vararrays.m4
  m4/vasnprintf.m4
  m4/vfprintf-posix.m4
  m4/warn-on-use.m4
  m4/wchar_h.m4
  m4/wint_t.m4
  m4/xsize.m4
  m4/zzgnulib.m4
])
