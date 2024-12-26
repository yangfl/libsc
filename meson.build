project(
  'sc', 'c', version: '0.0.0',
  meson_version: '>= 0.60.0',
  default_options: [
    'c_std=c11',
    'cpp_std=c++11',
    'warning_level=everything',
  ],
)

src_orig = [
  'src/Closure.c',
  'src/Exception.c',
  'src/Refcnt.c',
  'src/SignalDebugger.c',
  'src/container/Array.c',
  'src/object/Trait.c',
  'src/object/trait/Allocator.c',
  'src/object/trait/Scope.c',
  'src/object/trait/Stream.c',
  'src/system/execinfo.c',
  'src/wrapper/stdio.c',
  'src/wrapper/stdlib.c',
  'src/wrapper/threads.c',
]

prog_python = find_program('python3')
src_comb = custom_target(
  'combine-src', input: ['tools/concat.py'] + src_orig, output: ['libsc.c'],
  command: [prog_python, '@INPUT@', '@OUTPUT@'])

src = [
  src_comb,
  'src/thread/nproc.c',
  'src/thread/threadname.c',
]

cc = meson.get_compiler('c')
in_debug = get_option('debug')
on_windows = host_machine.system() in ['windows', 'cygwin']

cpp_args = []
proj_cpp_args = []

# diagnostic flags
if meson.is_subproject()
  # suppress all compiler warnings when we're a subproject
  if cc.get_id() == 'msvc'
    add_project_arguments('/w', language: ['c', 'cpp'])
  else
    add_project_arguments('-w', language: ['c', 'cpp'])
  endif
else
  # enable compiler warnings
  if cc.get_id() != 'msvc'
    if get_option('warning_level') in ['3', 'everything']
      add_project_arguments(
        '-Werror=format-security',
        '-Wno-cast-function-type', '-Wno-cast-qual', '-Wno-cast-align',
        '-Wno-missing-field-initializers', '-Wno-packed', '-Wno-padded',
        '-Wno-conversion', '-Wno-unsuffixed-float-constants',
        '-Wno-vla', '-Wno-stack-protector',
        '-Wno-double-promotion',
        '-Wno-unused-macros',
        '-Wno-c++-compat',
        '-Wno-switch-default',
        '-Wno-format-nonliteral',
        '-Wno-inline',
        language: ['c', 'cpp'])
    endif
    if in_debug
      add_project_arguments(
        '-fanalyzer',
        '-Wno-analyzer-tainted-allocation-size', '-Wno-analyzer-too-complex',
        language: ['c', 'cpp'])
      if not on_windows
        add_project_link_arguments('-rdynamic', language: ['c', 'cpp'])
      endif
    endif
  endif
endif

# hardening
if cc.get_id() != 'msvc'
  cpp_args += '-D_FORTIFY_SOURCE=2'
  add_project_arguments(
    '-fstack-protector-strong', '-fstack-clash-protection',
    language: ['c', 'cpp'])
  if host_machine.cpu_family() in ['x86', 'x86_64']
    add_project_arguments('-fcf-protection', language: ['c', 'cpp'])
  endif
  add_project_link_arguments('-Wl,--gc-sections', language: ['c', 'cpp'])
  if not on_windows
    add_project_link_arguments(
      '-Wl,-z,relro', '-Wl,-z,now',
      language: ['c', 'cpp'])
  endif
endif

# macro definitions
cpp_args += '-D_DEFAULT_SOURCE'

if in_debug
  cpp_args += '-D_DEBUG'
endif

has_include_supported = cc.compiles('''
#ifndef __has_include
#error "No __has_include"
#endif
''', name: '__has_include')
if not has_include_supported
  if cc.has_header('byteswap.h')
    cpp_args += '-DSC_HAS_BYTESWAP_H'
  endif
  if cc.has_header('endian.h')
    cpp_args += '-DSC_HAS_ENDIAN_H'
  endif
  if cc.has_header('execinfo.h')
    cpp_args += '-DSC_HAS_EXECINFO_H'
  endif
  if cc.has_header('pthread_np.h')
    cpp_args += '-DSC_HAS_PTHREAD_NP_H'
  endif
  if cc.has_header('stdbit.h')
    cpp_args += '-DSC_HAS_STDBIT_H'
  endif
  if cc.has_header('sys/cdefs.h')
    cpp_args += '-DSC_HAS_SYS_CDEFS_H'
  endif
  if cc.has_header('sys/endian.h')
    cpp_args += '-DSC_HAS_SYS_ENDIAN_H'
  endif
  if cc.has_header('threads.h')
    cpp_args += '-DSC_HAS_THREADS_H'
  endif
endif

if on_windows
  cpp_args += ['-DUNICODE', '-D_UNICODE']
  cpp_args += '-DSC_LOG_NO_COLOR'
  proj_cpp_args += '-DSC_BUILDING_DLL'
endif

# dependencies
m_dep = cc.find_library('m', required: false)
thread_dep = []
iconv_dep = []

if not on_windows
  thread_dep = dependency('threads', required: false)
  if not thread_dep.found()
    cpp_args += '-DSC_NO_THREADS'
  else
    if cc.has_function(
        'pthread_setname_np',
        args: ['-D_GNU_SOURCE'],
        dependencies: thread_dep)
      cpp_args += '-DSC_HAS_PTHREAD_NAME'
    endif
  endif
endif

proj_deps = [m_dep, thread_dep]

incdir = ['include', 'src']

# target
add_project_arguments(cpp_args, language: ['c', 'cpp'])

library(
  'sc', src,
  c_args: proj_cpp_args,
  dependencies: proj_deps,
  include_directories: incdir,
  soversion: '0',
  gnu_symbol_visibility: 'hidden',
)
