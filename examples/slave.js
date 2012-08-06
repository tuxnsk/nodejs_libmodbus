'use strict';

var log = console.log;
var mb = require('../modbus.js').create();

mb.onError(function (msg) {
  log('ERROR', msg);
});

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
//	ctx.destroy();
//}, 5000);
