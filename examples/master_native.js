'use strict';

var log = console.log;
var native = require('../modbus.js').native;

// create context
var ctx = native.new_tcp("127.0.0.1", 1502);

// connect to slave device
native.connect(ctx);

// get value
var result = [];
native.read_registers(ctx, 2, 1, result);
log(result[0]);

// set value
native.write_bit(ctx, 1, native.OFF);

// close context
native.close(ctx);
native.free(ctx);
