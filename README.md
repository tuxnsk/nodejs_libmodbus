
Sorry for my bad english. I need help to correct text. My contacts: http://tuxnsk.ru/

# About #
This is binding to native library libmodbus (http://libmodbus.org/).

Now working only in *nix system.

Module have native function and framework.

# Framework examples #
## Creating slave device (server) ##

```javascript
var log = console.log;
var mb = require('modbus').create();

// create device memory map
var data = mb.createData({ countReg: 5, countBit: 2 });
data.setReg(2, 321);
data.setBit(1, true);
data.dumpData(); // show memory map

// create slave device
var ctx = mb.createSlave({

  // connection type and params
  con: mb.createConTcp('127.0.0.1', 1502),
  //con: mb.createConRtu(1, '/dev/ttyS0', 9600),
  
  // data map
  data: data,

  // callback functions
  onQuery: function () {
    log('onQuery');
    //ctx.dumpData();
    log(ctx.getBits(0, 2));
  },
  onDestroy: function () {
    log('onDestroy');
  }
});

// destroy device
//setTimeout(function () {
//  ctx.destroy();
//}, 5000);
```

## Creating master device (client) ##

```javascript
var log = console.log;
var mb = require('modbus').create();

mb.onError(function (msg) {
  log('ERROR', msg);
});

// create master device
var ctx = mb.createMaster({

  // connection type and params
  con: mb.createConTcp('127.0.0.1', 1502),
  //con: mb.createConRtu(1, '/dev/ttyS1', 9600),

  // callback functions
  onConnect: function () {
    log('onConnect');
    log(ctx.getReg(2));
    ctx.setBit(1, false);
    ctx.destroy();
  },
  onDestroy: function () {
    log('onDestroy');
  }
});
```

# Creating master device use native function #
WARNING: native function blocking all processes!

```javascript
var log = console.log;
var native = require('modbus').native;

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
```

# API #

## createData(arg) ##
Create data map (use only slave device).

arg[Object] - having next fields:
* dataBit[Array] - array boolean values of read/write bits
* dataInputBit[Array] - array boolean values of read only bits
* dataReg[Array] - array 16bit unsigned integer values of read/write register
* dataInputReg[Array] - array 16bit unsigned integer values of read only register
* countBit[Number] - created empty dataBit array
* countInputBit[Number] - created empty dataInputBit array
* countReg[Number] - created empty dataReg array
* countInputReg[Number] - created empty dataInputReg array
* retur[Object] - data object with methods:

getBit(adr) - get read/write bit value
* adr[Number] - bit address
* return[Boolean] - bit value

getInputBit(adr) - get read only bit value

getReg(adr) - get read/write register value

getInputReg(adr) - get read only register value

getBits(adr, count) - get read/write bits array value
* adr[Number] - first bit address
* count[Number] - count readed bits
* return[Array] - array boolean values readed bits

getInputBits(adr, count) - get read only bits array value

getRegs(adr, count) - get read/write registers array value

getInputRegs(adr, count) - get read only registers array value

setBit(adr, val) - set read/write bit value
* adr[Number] - address bit
* val[Boolean] - value bit

setInputBit(adr, val) - set read only bit value

setReg(adr, val) - set read/write register value

setInputReg(adr, val) - set read only register value

setBits(adr, val) - set read/write bits array value
* adr[Number] - first bit address
* val[Array] - array boolean values

setInputBits(adr, val) - set read only bits array value

setRegs(adr, val) - set read/write registers array value

setInputRegs(adr, val) - set read only registers array value

dumpData() - show memory dump

## createConTcp(ip, port, max) ##
Create connection object for tcp connection.
* ip[String] - ip address modbus slave device, default "127.0.0.1"
* port[Number] - tcp port modbus slave device, default 502
* max[Number] - max connections for slave device, default 1

## createConRtu(id, device, baud, parity, dataBit, stopBit) ##
Create connection object for serial port connection.
* id[Number] - id slave device on line, default 1
* device[String] - name of serial port, default "/dev/ttyS0"
* baud[Number] - using baudrate speed, the allowed values are 9600, 19200, 57600, 115200, etc, default 115200
* parity[String] - using parity control, the allowed values are "N", "E", "O", default "N"
* dataBit[Number] - count data bit, the allowed values are 5, 6, 7 and 8, default 8
* stopBit[Number] - using stop bit, the allowed values are 1 and 2, default 1

## createSlave(arg) ##
Create slave modbus device

arg[Object] - having next fields:
* con[Object] - connect object, created createConTcp/createConRtu function
* data[Object] - memory map object, created createData function
* onQuery[Function] - callback function, called if master device send any query request
* onDestroy[Function] - callback function, called if device destroyed
* return[Object] - data object (context) with methods:

destroy() - destroyed slave device

and all data object api

## createMaster(arg) ##
Create master modbus device
* con[Object] - connect object, created createConTcp/createConRtu function
* onConnect[Function] - callback function, called if connect complete to slave device
* onDestroy[Function] - callback function, called if device destroyed
* return[Object] - data object (context) with methods:

destroy() - destroyed slave device

and all data object api without setInput*





