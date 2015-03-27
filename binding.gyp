{
    'targets': [{
        'target_name': 'modbus_binding',
        'cflags': [
            '-Wall'
        ],
        'ldflags': [
            '../libmodbus/src/.libs/modbus.o',
            '../libmodbus/src/.libs/modbus-data.o',
            '../libmodbus/src/.libs/modbus-rtu.o',
            '../libmodbus/src/.libs/modbus-tcp.o'
        ],
        'include_dirs': [
            './libmodbus/src/'
        ],
        'sources': [
            './src/main.cpp'
        ]
    }]
}
