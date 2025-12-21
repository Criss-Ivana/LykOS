as_files += files(
    'userspace.S',
)

asm_files += files(
    'syscall.asm',
    'thread.asm',
)

c_files += files(
    'entry.c',
    'ioport.c',
    'lcpu.c',
    'msr.c',
    'paging.c',
    'serial.c',
    'syscall.c',
    'thread.c',
)

subdir('abi')
subdir('devices')
subdir('tables')
