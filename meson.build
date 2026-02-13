project('crprintf', 'c',
  version: '0.0.1',
  license: 'MIT',
  default_options: ['warning_level=2']
)

inc = include_directories('src')
sources = files('src/crprintf.c')

libcrprintf = library('crprintf',
  sources,
  install: true,
  include_directories: inc
)

crprintf_dep = declare_dependency(
  link_with: libcrprintf,
  include_directories: inc
)

install_headers('src/crprintf.h')
pkg = import('pkgconfig')

pkg.generate(libcrprintf,
  name: 'crprintf',
  description: 'printf with inline color tags and variables',
  version: meson.project_version()
)

if get_option('examples')
  executable('example_basic',
    'examples/basic.c',
    include_directories: inc,
    link_with: libcrprintf
  )
endif

if get_option('tests')
  test_exe = executable('test_crprintf',
    'tests/test.c',
    include_directories: inc,
    link_with: libcrprintf
  )
  test('crprintf', test_exe)
endif
