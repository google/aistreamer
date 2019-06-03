# Copyright (c) 2019 Google LLC
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Build for Google logging module

package(default_visibility = ["//visibility:private"])
cc_library(
  name = "glog",
  srcs = glob(
    [
      "src/*.cc",
    ],
    exclude = ["src/*test*.cc"],
  ),
  hdrs = glob([
    "src/*.h",
    "src/base/*.h",
    "src/glog/*.h",
  ]) + [
    ":glog-config",
    ":logging_h",
    ":raw_logging_h",
    ":stl_logging_h",
    ":vlog_is_on_h",
  ],
  copts = [
    "-D_START_GOOGLE_NAMESPACE_='namespace google {'",
    "-D_END_GOOGLE_NAMESPACE_='}'",
    "-DGOOGLE_NAMESPACE='google'",
    #"-DHAVE_DLADDR_H",
    "-DHAVE_DLFCN_H",
    "-DHAVE_EXECINFO_H",
    "-DHAVE_FCNTL",
    "-DHAVE_GLOB_H",
    "-DHAVE_INTTYPES_H",
    "-DHAVE_LIBPTHREAD",
    #"-DHAVE_LIBUNWIND_H",
    "-DHAVE_LIB_GFLAGS",
    #"-DHAVE_LIB_GMOCK",
    "-DHAVE_LIB_GTEST",
    #"-DHAVE_LIB_UNWIND",
    "-DHAVE_MEMORY_H",
    "-DHAVE_NAMESPACES",
    "-DHAVE_PREAD",
    "-DHAVE_PTHREAD",
    "-DHAVE_PWD_H",
    "-DHAVE_PWRITE",
    "-DHAVE_RWLOCK",
    "-DHAVE_SIGACTION",
    "-DHAVE_SIGALTSTACK",
    "-DHAVE_STDINT_H",
    "-DHAVE_STDLIB_H",
    "-DHAVE_STRINGS_H",
    "-DHAVE_STRING_H",
    "-DHAVE_SYSCALL_H",
    "-DHAVE_SYSLOG_H",
    "-DHAVE_SYS_STAT_H",
    "-DHAVE_SYS_SYSCALL_H",
    "-DHAVE_SYS_TIME_H",
    "-DHAVE_SYS_TYPES_H",
    "-DHAVE_SYS_UCONTEXT_H",
    "-DHAVE_SYS_UTSNAME_H",
    "-DHAVE_UCONTEXT_H",
    "-DHAVE_UNISTD_H",
    "-DHAVE_UNWIND_H",
    "-DHAVE_USING_OPERATOR",
    "-DHAVE___ATTRIBUTE___",
    "-DHAVE___BUILTIN_EXPECT",
    "-DHAVE___SYNC_VAL_COMPARE_AND_SWAP",
    "-Wno-sign-compare"
  ],
  includes = ["src"],
  linkopts = ["-pthread"],
  visibility = ["//visibility:public"],
  deps = [
    "//external:gflags",
  ],
)
genrule(
  name = "glog-config",
  srcs = ["src/config.h.in"],
  outs = ["src/config.h"],
  cmd = "cp $(<) $(@)",
)
genrule(
  name = "logging_h",
  srcs = [
    "src/glog/logging.h.in",
  ],
  outs = [
    "glog/logging.h",
  ],
  cmd = "$(location :replace_sh) < $(<) > $(@)",
  tools = [":replace_sh"],
)
genrule(
  name = "raw_logging_h",
  srcs = [
    "src/glog/raw_logging.h.in",
  ],
  outs = [
    "glog/raw_logging.h",
  ],
  cmd = "$(location :replace_sh) < $(<) > $(@)",
  tools = [":replace_sh"],
)
genrule(
  name = "stl_logging_h",
  srcs = [
    "src/glog/stl_logging.h.in",
  ],
  outs = [
    "glog/stl_logging.h",
  ],
  cmd = "$(location :replace_sh) < $(<) > $(@)",
  tools = [":replace_sh"],
)
genrule(
  name = "vlog_is_on_h",
  srcs = [
    "src/glog/vlog_is_on.h.in",
  ],
  outs = [
    "glog/vlog_is_on.h",
  ],
  cmd = "$(location :replace_sh) < $(<) > $(@)",
  tools = [":replace_sh"],
)
genrule(
  name = "replace_sh",
  outs = [
    "replace.sh",
  ],
  cmd = """
cat > $@ <<"EOF"
#! /bin/sh
sed -e 's/@ac_cv_cxx_using_operator@/1/g' \
  -e 's/@ac_cv_have_inttypes_h@/1/g' \
  -e 's/@ac_cv_have_libgflags@/1/g' \
  -e 's/@ac_cv_have_stdint_h@/1/g' \
  -e 's/@ac_cv_have_systypes_h@/1/g' \
  -e 's/@ac_cv_have_unistd_h@/1/g' \
  -e 's/@ac_cv_have_uint16_t@/1/g' \
  -e 's/@ac_cv_have_u_int16_t@/1/g' \
  -e 's/@ac_cv_have___builtin_expect@/1/g' \
  -e 's/@ac_cv_have___uint16@/0/g' \
  -e 's/@ac_cv___attribute___noinline@/__attribute__((noinline))/g' \
  -e 's/@ac_cv___attribute___noreturn@/__attribute__((noreturn))/g' \
  -e 's/@ac_cv___attribute___printf_4_5@/__attribute__((__format__ (__printf__, 4, 5)))/g' \
  -e 's/@ac_google_start_namespace@/namespace google {/g' \
  -e 's/@ac_google_end_namespace@/}/g' \
  -e 's/@ac_google_namespace@/google/g'
EOF""",
)
