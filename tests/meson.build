
test_inc = include_directories('.')

tests = [
  'utils',
  'equation',
]

foreach t : tests
  src_test = 'test_' + t + '.c'
  test_exec = executable(
    t,
    'test.c',
    src_test,
    src,
    include_directories: [
      inc,
      test_inc,
    ],
    c_args: flags + ['-DUNIT_TEST_TARGET'],
    dependencies: [ x11, x11_test ],
  )

  test(t, test_exec)
endforeach
