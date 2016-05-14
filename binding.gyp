{
    'conditions': [
        [
            'OS=="mac"', {
                'targets': [
                    {
                        'target_name': 'modbus_binding',
                        'cflags': [
                            '-Wall',
                        ],
                    'xcode_settings': {
                        'OTHER_CFLAGS': [ '-ObjC' ],
                        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++', '-v'],
                        'OTHER_LDFLAGS': ['-stdlib=libc++'],
                        'MACOSX_DEPLOYMENT_TARGET': '10.11',
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES', 
                        },
                    'libraries': [
                        '../libmodbus/src/.libs/libmodbus.dylib',
                    ],
                        'include_dirs': [
                            './libmodbus/src/',
                        ],
                        'sources': [
                            './src/main.cpp'
                        ],
                    },
                ]
            },
        ],
        [
            'OS=="linux"', {
                'targets': [
                    {
                        'target_name': 'modbus_binding',
                        'cflags': [
                            '-Wall',
                        ],
                        'ldflags': [
                            '../libmodbus/src/.libs/modbus.o',
                            '../libmodbus/src/.libs/modbus-data.o',
                            '../libmodbus/src/.libs/modbus-rtu.o',
                            '../libmodbus/src/.libs/modbus-tcp.o',
                        ],
                        'include_dirs': [
                            './libmodbus/src/',
                        ],
                        'sources': [
                            './src/main.cpp'
                        ],
                    },
                ]
            },
        ]
    ]
}
