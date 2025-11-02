project(
    'kernel',
    ['c', 'nasm'],
    version: '0.1',
    meson_version: '>=0.62.0',
    default_options: ['c_std=gnu23', 'warning_level=2'],
)

target_arch = get_option('target_arch')

nasm = find_program('nasm', required: true)

c_flags = [
    # Warnings
    '-Werror',
    '-Wall',
    '-Wextra',
    '-Wno-unused-parameter',
    '-Wno-unused-function',
    '-Wvla',
    '-Wimplicit-fallthrough',
    # Environment
    '-ffreestanding',
    '-fno-stack-protector',
    '-fno-stack-check',
    '-fPIE',
    # Optimization
    '-flto',
    '-O2',
    '-g',
    # Features
    '-DPRINTF_DISABLE_SUPPORT_FLOAT',
    '-DLIMINE_API_REVISION=3',
]

as_flags = [
    '-g',
]

nasm_flags = [
    '-f', 'elf64',
    '-g',
]

ld_flags = [
    '-nostdlib',
    '-static',
    '-gc-sections',
    '-z', 'max-page-size=0x1000',
    '-g',
    '-T', meson.current_source_dir() / f'linker-scripts/@target_arch@.ld',
]

if target_arch == 'x86_64'
    c_flags += [
        '-target', 'x86_64-elf',
        '-m64',
        '-mgeneral-regs-only',
        '-mno-red-zone',
        '-mcmodel=kernel',
    ]
    ld_flags += [
        '-m', 'elf_x86_64',
    ]
elif target_arch == 'aarch64'
    c_flags += [
        '-target', 'aarch64-unknown-none',
        '-mgeneral-regs-only',

    ]
    ld_flags += [
        '-m', 'aarch64elf',
    ]
endif

c_files = []
as_files = []
asm_files = []

limine_header = custom_target(
    'limine.h',
    output: 'limine.h',
    command: [
        'curl',
        '-L',
        '-o', 'limine.h',
        'https://codeberg.org/Limine/Limine/raw/branch/v10.x-binary/limine.h',
    ],
)

include_directories = include_directories('include')

subdir('source')

asm_objs = []
foreach f : asm_files
    asm_objs += custom_target(
        f + '_obj',
        input: f,
        output: f + '.o',
        command: [nasm, nasm_flags, '-o', '@OUTPUT@', '@INPUT@'],
        depend_files: f,
    )
endforeach

sources = c_files + as_files + asm_objs + [limine_header]

output_static_lib = static_library(
    'kernel',
    sources: sources,
    include_directories: include_directories,
    c_args: c_flags,
    install: false,
)

custom_target(
    input: output_static_lib,
    output: f'kernel-@target_arch@.elf',
    command: [
        meson.get_external_property('ld'),
        ld_flags,
        '--whole-archive', '@INPUT@',
        '--no-whole-archive',
        '-o', '@OUTPUT@',
    ],
    install: true,
    install_dir: 'bin',
    install_mode: 'rw-r--r--',
)
