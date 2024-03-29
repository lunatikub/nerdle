project(
  'nerdle',
  'c',
  default_options : [
    'c_std=gnu11',
  ]
)

c_compiler = meson.get_compiler('c')
root_dir = meson.source_root()

flags = c_compiler.get_supported_arguments(
  [
    '-Wall',
    '-Werror',
    '-D_GNU_SOURCE',
    '-Wunused',
    '-Wunused-value',
    '-Wunused-variable',
    '-Wunused-parameter',
    '-Wbool-compare',
    '-Wformat-security',
    '-Winit-self',
    '-Wlogical-op',
    '-Wtrampolines',
    '-Wmissing-format-attribute',
    '-Wno-error=deprecated-declarations',
    '-Wno-format-zero-length',
    '-Wshadow',
    '-Wmissing-declarations',
    '-Wpointer-arith',
    '-Wformat=2',
    '-pipe',
    '-pthread',
    '-g3',
  ]
)

inc = include_directories('src')

src = files(
  'src/utils.c',
  'src/equation.c',
  'src/check_equality.c',
  'src/nerdle.c',
  'src/interface.c',
)

cc = meson.get_compiler('c')
x11 = dependency('x11', required: false, disabler: true)
x11_test = dependency('xtst', required: false, disabler: true)

subdir('tests')

executable(
  'nerdle',
  src,
  'src/main.c',
  include_directories: inc,
  c_args: flags,
  dependencies : [ x11, x11_test ],
)
