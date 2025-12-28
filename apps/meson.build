project(
    'apps',
    ['c'],
    version: '0.1',
    meson_version: '>=0.64.0',
    default_options: ['c_std=gnu23', 'warning_level=2'],
)

app_sources = files(
    'init.c',
)

foreach src : app_sources
    exe_name = 'init'
    executable(
        exe_name,
        src,
        c_args: ['-O2', '-mgeneral-regs-only', '-g'],
        install: true,
        install_dir: 'bin',
    )
endforeach
