'use strict';

var mb = require("./build/Release/modbus_binding");

//var DBG = true;
var DBG = false;

var log = console.log;

function dataChange(a, args) {
	var func = args.func; // вызываемая функция
	var jso = args.jso; // для локальных
	var ctx = args.ctx; // для удаленных
	var adr = args.adr; // адрес данных
	var count = args.count; // количество значений
	var val = args.val; // значения для записи
	
	var isLocal = null;
	if (jso) {
		isLocal = true;
	} else if (ctx) {
		isLocal = false;
	} else {
		a.err('dataChange: invalid arguments');
		return null;
	}
	
	var funcs = {};
	
	// получить один бит
	// params: adr
	funcs['getBit'] = function () {
		if (isLocal) {
			if (adr < 0 || adr >= jso.nb_bits) {
				a.err(func + ': invalid address');
				return null;
			}
		
			if (jso.tab_bits[adr]) return true;
			else return false;
		} else {
			var val = [];
			
			if (mb.read_bits(ctx, adr, 1, val) == -1) {
				a.err(func + ': ' + mb.strerror());
				return null;
			}
			
			if (val[0]) return true;
			else return false;
		}
	};
	
	// получить один бит для чтения
	// params: adr
	funcs['getInputBit'] = function () {
		if (isLocal) {
			if (adr < 0 || adr >= jso.nb_input_bits) {
				a.err(func + ': invalid address');
				return null;
			}
			
			if (jso.tab_input_bits[adr]) return true;
			else return false;
		} else {
			var val = [];
			
			if (mb.read_input_bits(ctx, adr, 1, val) == -1) {
				a.err(func + ': ' + mb.strerror());
				return null;
			}
			
			if (val[0]) return true;
			else return false;
		}
	};
	
	// получить один регистр
	// params: adr
	funcs['getReg'] = function () {
		if (isLocal) {
			if (adr < 0 || adr >= jso.nb_registers) {
				a.err(func + ': invalid address');
				return null;
			}
			
			return jso.tab_registers[adr];
		} else {
			var val = [];
			
			if (mb.read_registers(ctx, adr, 1, val) == -1) {
				a.err(func + ': ' + mb.strerror());
				return null;
			}
			
			return val[0];
		}
	};
	
	// получить один регистр для чтения
	// params: adr
	funcs['getInputReg'] = function () {
		if (isLocal) {
			if (adr < 0 || adr >= jso.nb_input_registers) {
				a.err(func + ': invalid address');
				return null;
			}
			
			return jso.tab_input_registers[adr];
		} else {
			var val = [];
			
			if (mb.read_input_registers(ctx, adr, 1, val) == -1) {
				a.err(func + ': ' + mb.strerror());
				return null;
			}
			
			return val[0];
		}
	};
	
	// получить список бит
	// params: adr, count
	funcs['getBits'] = function () {
		if (isLocal) {
			var end = adr + count;
			
			if (adr < 0 || adr >= jso.nb_bits || end > jso.nb_bits) {
				a.err(func + ': invalid address');
				return null;
			}
			
			var ret = [], j = 0;
			for (var i = adr; i < end; i++) {
				if (jso.tab_bits[i]) ret[j] = true;
				else ret[j] = false;
				j++;
			}
			
			return ret;
		} else {
			var val = [];
			
			if (mb.read_bits(ctx, adr, count, val) == -1) {
				a.err(func + ': ' + mb.strerror());
				return null;
			}
			
			var ret = [];
			for (var i in val) {
				if (val[i]) ret[i] = true;
				else ret[i] = false;
			}
			
			return ret;
		}
	};
	
	// получить список бит для чтения
	// params: adr, count
	funcs['getInputBits'] = function () {
		if (isLocal) {
			var end = adr + count;
			
			if (adr < 0 || adr >= jso.nb_input_bits || end > jso.nb_input_bits) {
				a.err(func + ': invalid address');
				return null;
			}
			
			var ret = [], j = 0;
			for (var i = adr; i < end; i++) {
				if (jso.tab_input_bits[i]) ret[j] = true;
				else ret[j] = false;
				j++;
			}
			
			return ret;
		} else {
			var val = [];
		
			if (mb.read_input_bits(ctx, adr, count, val) == -1) {
				a.err(func + ': ' + mb.strerror());
				return null;
			}
			
			var ret = [];
			for (var i in val) {
				if (val[i]) ret[i] = true;
				else ret[i] = false;
			}
			
			return ret;
		}
	};
	
	// получить список регистров
	// params: adr, count
	funcs['getRegs'] = function () {
		if (isLocal) {
			var end = adr + count;
			
			if (adr < 0 || adr >= jso.nb_registers || end > jso.nb_registers) {
				a.err(func + ': invalid address');
				return null;
			}
			
			var ret = [], j = 0;
			for (var i = adr; i < end; i++) {
				ret[j] = jso.tab_registers[i];
				j++;
			}
			
			return ret;
		} else {
			var ret = [];
		
			if (mb.read_registers(ctx, adr, count, ret) == -1) {
				a.err(func + ': ' + mb.strerror());
				return null;
			}
			
			return ret;
		}
	};
	
	// получить список регистров для чтения
	// params: adr, count
	funcs['getInputRegs'] = function () {
		if (isLocal) {
			var end = adr + count;
			
			if (adr < 0 || adr >= jso.nb_input_registers || end > jso.nb_input_registers) {
				a.err(func + ': invalid address');
				return null;
			}
			
			var ret = [], j = 0;
			for (var i = adr; i < end; i++) {
				ret[j] = jso.tab_input_registers[i];
				j++;
			}
			
			return ret;
		} else {
			var ret = [];
		
			if (mb.read_input_registers(ctx, adr, count, ret) == -1) {
				a.err(func + ': ' + mb.strerror());
				return null;
			}
			
			return ret;
		}
	};
	
	// установить один бит
	// params: adr, val
	funcs['setBit'] = function () {
		if (isLocal) {
			if (adr < 0 || adr >= jso.nb_bits) {
				a.err(func + ': invalid address');
				return;
			}
			
			if (val) jso.tab_bits[adr] = mb.ON;
			else jso.tab_bits[adr] = mb.OFF;
		} else {
			if (val) val = mb.ON;
			else val = mb.OFF;
			
			if (mb.write_bit(ctx, adr, val) == -1) {
				a.err(func + ': ' + mb.strerror());
			}
		}
	};
	
	// установить один бит для чтения
	// params: adr, val
	funcs['setInputBit'] = function () {
		if (isLocal) {
			if (adr < 0 || adr >= jso.nb_input_bits) {
				a.err(func + ': invalid address');
				return;
			}
			
			if (val) jso.tab_input_bits[adr] = mb.ON;
			else jso.tab_input_bits[adr] = mb.OFF;
		} else {
			a.err(func + ': permission denied');
		}
	};
	
	// установить один регистр
	// params: adr, val
	funcs['setReg'] = function () {
		if (val < 0 || val > 0xFFFF) {
			a.err(func + ': invalid value ' + val);
			val = 0;
		}
		
		if (isLocal) {
			if (adr < 0 || adr >= jso.nb_registers) {
				a.err(func + ': invalid address');
				return;
			}
			
			jso.tab_registers[adr] = val;
		} else {
			if (mb.write_register(ctx, adr, val) == -1) {
				a.err(func + ': ' + mb.strerror());
			}
		}
	};
	
	// установить один регистр для чтения
	// params: adr, val
	funcs['setInputReg'] = function () {
		if (val < 0 || val > 0xFFFF) {
			a.err(func + ': invalid value ' + val);
			val = 0;
		}
		
		if (isLocal) {
			if (adr < 0 || adr >= jso.nb_input_registers) {
				a.err(func + ': invalid address');
				return;
			}
			
			jso.tab_input_registers[adr] = val;
		} else {
			a.err(func + ': permission denied');
		}
	};
	
	// установить список бит
	// params: adr, val
	funcs['setBits'] = function () {
		var count = val.length;
		
		if (isLocal) {
			var end = adr + count;
			
			if (adr < 0 || adr >= jso.nb_bits || end > jso.nb_bits) {
				a.err(func + ': invalid address');
				return null;
			}
			
			var j = 0;
			for (var i = adr; i < end; i++) {
				if (val[j]) jso.tab_bits[i] = mb.ON;
				else jso.tab_bits[i] = mb.OFF;
				j++;
			}
		} else {
			var ar = [];
			
			for (var i in val) {
				if (val[i]) ar[i] = mb.ON;
				else ar[i] = mb.OFF;
			}
			
			if (mb.write_bits(ctx, adr, count, ar) == -1) {
				a.err(func + ': ' + mb.strerror());
			}
		}
	};
	
	// установить список бит для чтения
	// params: adr, val
	funcs['setInputBits'] = function () {
		var count = val.length;
		
		if (isLocal) {
			var end = adr + count;
			
			if (adr < 0 || adr >= jso.nb_input_bits || end > jso.nb_input_bits) {
				a.err(func + ': invalid address');
				return null;
			}
			
			var j = 0;
			for (var i = adr; i < end; i++) {
				if (val[j]) jso.tab_input_bits[i] = mb.ON;
				else jso.tab_input_bits[i] = mb.OFF;
				j++;
			}
		} else {
			a.err(func + ': permission denied');
		}
	};
	
	// установить список регистров
	// params: adr, val
	funcs['setRegs'] = function () {
		var count = val.length;
		
		if (isLocal) {
			var end = adr + count;
			
			if (adr < 0 || adr >= jso.nb_registers || end > jso.nb_registers) {
				a.err(func + ': invalid address');
				return null;
			}
			
			var j = 0;
			for (var i = adr; i < end; i++) {
				if (val[j] < 0 || val[j] > 0xFFFF) {
					a.err(func + ': invalid value ' + val[j]);
					val[j] = 0;
				}
				
				jso.tab_registers[i] = val[j];
				j++;
			}
		} else {
			var ar = [];
		
			for (var i in val) {
				if (val[i] < 0 || val[i] > 0xFFFF) {
					a.err(func + ': invalid value ' + val[i]);
					val[i] = 0;
				}
				
				ar[i] = val[i];
			}
			
			if (mb.write_registers(ctx, adr, count, ar) == -1) {
				a.err(func + ': ' + mb.strerror());
			}
		}
	};
	
	// установить список регистров для чтения
	// params: adr, val
	funcs['setInputRegs'] = function () {
		var count = val.length;
		
		if (isLocal) {
			var end = adr + count;
			
			if (adr < 0 || adr >= jso.nb_input_registers || end > jso.nb_input_registers) {
				a.err(func + ': invalid address');
				return null;
			}
			
			var j = 0;
			for (var i = adr; i < end; i++) {
				if (val[j] < 0 || val[j] > 0xFFFF) {
					a.err(func + ': invalid value ' + val[j]);
					val[j] = 0;
				}
				
				jso.tab_input_registers[i] = val[j];
				j++;
			}
		} else {
			a.err(func + ': permission denied');
		}
	};
	
	funcs['setSlave'] = function () {
		//console.log("setting slave to:" + val);
		if (mb.set_slave(ctx, val) == -1) {
		    a.err(func + ': ' + mb.strerror());
		}
	};
	
	return funcs[func]();
}

function createData(a, args) {
	var onError = null;
	
	var dataBit = args.dataBit || [];
	var dataInputBit = args.dataInputBit || [];
	var dataReg = args.dataReg || [];
	var dataInputReg = args.dataInputReg || [];
	
	// если указанна длинна больше массива, то расширяем
	if (args.countBit > dataBit.length) {
		for (var i = dataBit.length; i < args.countBit; i++) {
			dataBit[i] = false;
		}
	}
	if (args.countInputBit > dataInputBit.length) {
		for (var i = dataInputBit.length; i < args.countInputBit; i++) {
			dataInputBit[i] = false;
		}
	}
	if (args.countReg > dataReg.length) {
		for (var i = dataReg.length; i < args.countReg; i++) {
			dataReg[i] = 0;
		}
	}
	if (args.countInputReg > dataInputReg.length) {
		for (var i = dataInputReg.length; i < args.countInputReg; i++) {
			dataInputReg[i] = 0;
		}
	}
	
	// валидация данных
	for (var i in dataBit) {
		if (dataBit[i]) dataBit[i] = mb.ON;
		else dataBit[i] = mb.OFF;
	}
	
	for (var i in dataInputBit) {
		if (dataInputBit[i]) dataInputBit[i] = mb.ON;
		else dataInputBit[i] = mb.OFF;
	}
	
	for (var i in dataReg) {
		if (dataReg[i] < 0 || dataReg[i] > 0xFFFF) {
			a.err('createData: invalid value ' + dataReg[i]);
			dataReg[i] = 0;
		}
	}
	
	for (var i in dataInputReg) {
		if (dataInputReg[i] < 0 || dataInputReg[i] > 0xFFFF) {
			a.err('createData: invalid value ' + dataInputReg[i]);
			dataInputReg[i] = 0;
		}
	}
	
	// создаем структуру данных
	var map = mb.mapping_new(dataBit.length, dataInputBit.length, dataReg.length, dataInputReg.length);
	var jso = {
		nb_bits: dataBit.length,
		nb_input_bits: dataInputBit.length,
		nb_input_registers: dataInputReg.length,
		nb_registers: dataReg.length,
		tab_bits: dataBit,
		tab_input_bits: dataInputBit,
		tab_input_registers: dataInputReg,
		tab_registers: dataReg
	};
	
	// TODO: оптимизировать процесс синхронизации jso и map
	
	// соханить в нативной структуре
	function write() {
		mb.json_to_map(jso, map);
	}
	
	// прочитать из нативной структуры
	function read() {
		mb.map_to_json(map, jso);
	}
	
	// сохраняем созданные данные
	write();
	
	var api = {};
	
	api.getBit = function (adr) {
		read();
		return dataChange(a, { func: 'getBit', jso: jso, adr: adr });
	};
	
	api.getInputBit = function (adr) {
		read();
		return dataChange(a, { func: 'getInputBit', jso: jso, adr: adr });
	};
	
	api.getReg = function (adr) {
		read();
		return dataChange(a, { func: 'getReg', jso: jso, adr: adr });
	};
	
	api.getInputReg = function (adr) {
		read();
		return dataChange(a, { func: 'getInputReg', jso: jso, adr: adr });
	};
	
	api.getBits = function (adr, count) {
		read();
		return dataChange(a, { func: 'getBits', jso: jso, adr: adr, count: count });
	};
	
	api.getInputBits = function (adr, count) {
		read();
		return dataChange(a, { func: 'getInputBits', jso: jso, adr: adr, count: count });
	};
	
	api.getRegs = function (adr, count) {
		read();
		return dataChange(a, { func: 'getRegs', jso: jso, adr: adr, count: count });
	};
	
	api.getInputRegs = function (adr, count) {
		read();
		return dataChange(a, { func: 'getInputRegs', jso: jso, adr: adr, count: count });
	};
	
	api.setBit = function (adr, val) {
		dataChange(a, { func: 'setBit', jso: jso, adr: adr, val: val });
		write();
	};
	
	api.setInputBit = function (adr, val) {
		dataChange(a, { func: 'setInputBit', jso: jso, adr: adr, val: val });
		write();
	};
	
	api.setReg = function (adr, val) {
		dataChange(a, { func: 'setReg', jso: jso, adr: adr, val: val });
		write();
	};
	
	api.setInputReg = function (adr, val) {
		dataChange(a, { func: 'setInputReg', jso: jso, adr: adr, val: val });
		write();
	};
	
	api.setBits = function (adr, val) {
		dataChange(a, { func: 'setBits', jso: jso, adr: adr, val: val });
		write();
	};
	
	api.setInputBits = function (adr, val) {
		dataChange(a, { func: 'setInputBits', jso: jso, adr: adr, val: val });
		write();
	};
	
	api.setRegs = function (adr, val) {
		dataChange(a, { func: 'setRegs', jso: jso, adr: adr, val: val });
		write();
	};
	
	api.setInputRegs = function (adr, val) {
		dataChange(a, { func: 'setInputRegs', jso: jso, adr: adr, val: val });
		write();
	};
	
	// вывести дамп всех значений в терминал
	api.dumpData = function () {
		console.dir(jso);
	};
	
	// private methods
	api._copyApi = function (obj) {
		obj.getBit = api.getBit;
		obj.getReg = api.getReg;
		obj.setBit = api.setBit;
		obj.setReg = api.setReg;
		obj.getInputBit = api.getInputBit;
		obj.getInputReg = api.getInputReg;
		obj.setInputBit = api.setInputBit;
		obj.setInputReg = api.setInputReg;
		
		obj.getBits = api.getBits;
		obj.getRegs = api.getRegs;
		obj.setBits = api.setBits;
		obj.setRegs = api.setRegs;
		obj.getInputBits = api.getInputBits;
		obj.getInputRegs = api.getInputRegs;
		obj.setInputBits = api.setInputBits;
		obj.setInputRegs = api.setInputRegs;
		
		obj.dumpData = api.dumpData;
	};
	
	// внутренние функции
	api._reply = function (ctx, query, len) {
		mb.reply(ctx, query, len, map);
		read(); // FIXME: делается в геттерах
	};
	
	// очистить данные
	api._freeMap = function () {
		mb.mapping_free(map);
	};
	
	return api;
};

function createConTcp(ip, port, max) {
	if (!ip) ip = '127.0.0.1';
	if (!port) port = mb.MODBUS_TCP_DEFAULT_PORT;
	if (!max) max = 1;
	
	return {
		type: 'TCP',
		ip: ip,
		port: port,
		max: max
	};
};

function createConRtu(id, device, baud, parity, dataBit, stopBit) {
	if (!id) id = 1;
	if (!device) device = '/dev/ttyS0';
	if (!baud) baud = 115200;
	if (!parity) parity = 'N';
	if (!dataBit) dataBit = 8;
	if (!stopBit) stopBit = 1;
	
	return {
		type: 'RTU',
		id: id,
		device: device,
		baud: baud,
		parity: parity,
		dataBit: dataBit,
		stopBit: stopBit
	};
};

function makeSlaveApi(ctx, data) {
	var api = {};
	
	data._copyApi(api);
	
	api.getContext = function () {
		return ctx;
	};
	
	return api;
}

function createSlaveTcp(a, con, data, cbs) {
	var isWorking = false;
	
	// создаем контекст подключения
	var ctx = mb.new_tcp(con.ip, con.port);
	if (!ctx) {
		a.err('createSlaveTcp: ' + mb.strerror());
		return null;
	}
	
	if (DBG) mb.set_debug(ctx, 1);
	
	// создаем сокет для ожидания соединения
	var sock = mb.tcp_listen(ctx, con.max);
	if (sock == -1) {
		a.err('createSlaveTcp: ' + mb.strerror());
		return null;
	}
	
	// returned api
	var api = makeSlaveApi(ctx, data);
	
	api.destroy = function () {
		isWorking = false;
		
		mb.close_mt(ctx);
		data._freeMap();
		data = null;
		mb.free(ctx);
		
		ctx = null;
		
		// чтобы не повадно было
		for (var i in api) api[i] = null;
		
		if (cbs.onDestroy) cbs.onDestroy(api);
	};
	
	isWorking = true;
	
	// ждем соединения
	function accept(ctx, sock) {
		mb.tcp_accept_async(ctx, sock, function (newSock) {
			// если слушающий сокет был закрыт
			if (newSock == -1) return;
			
			if (isWorking) accept(ctx, sock);
			
			function recive() {
				mb.receive_async(ctx, function (query, len) {
					// клиент закрыл соединение
					if (len == -1) return;
					
					// обрабатываем данные и отвечаем клиенту
					data._reply(ctx, query, len);
			
					if (cbs.onQuery) cbs.onQuery(api);
					
					if (isWorking) recive();
				});
			}
			
			// принимаем данные, пока клиент не закроет соединение
			if (isWorking) recive();
			
		});
	}
	
	if (isWorking) accept(ctx, sock);
	
	return api;
}

function createSlaveRtu(a, con, data, cbs) {
	var isWorking = false;
	
	// создаем контекст подключения
	var ctx = mb.new_rtu(con.device, con.baud, con.parity, con.dataBit, con.stopBit);
	if (!ctx) {
		a.err('createSlaveRtu: ' + mb.strerror());
		return null;
	}
	
	if (DBG) mb.set_debug(ctx, 1);
	
	mb.set_slave(ctx, con.id);
	
	// returned api
	var api = makeSlaveApi(ctx, data);
	
	api.destroy = function () {
		isWorking = false;
		
		mb.close_mt(ctx);
		data._freeMap();
		data = null;
		mb.free(ctx);
		
		ctx = null;
		
		// чтобы не повадно было
		for (var i in api) api[i] = null;
		
		if (cbs.onDestroy) cbs.onDestroy(api);
	};
	
	mb.connect_async(ctx, function (errCode) {
		if (errCode == -1) {
			a.err('createSlaveRtu: ' + mb.strerror());
			return;
		}
		
		// очищаем буфер
		mb.flush(ctx);
		
		isWorking = true;
		
		function recive() {
			mb.receive_async(ctx, function (query, len) {
				if (len == -1) { // пока нету данных
					if (isWorking) recive();
					return;
				}
				
				// обрабатываем данные и отвечаем клиенту
				data._reply(ctx, query, len);
		
				if (cbs.onQuery) cbs.onQuery(api);
				
				// принимаем данные, пока клиент не закроет соединение
				if (isWorking) recive();
			});
		}
		
		if (isWorking) recive();
	});
	
	return api;
}

// создать сетевое подчиненное устройство (сервер)
function createSlave(a, args) {
	// callback functions
	var cbs = {};
	// onConnect
	cbs.onQuery = args.onQuery;
	// onDiconnect
	cbs.onDestroy = args.onDestroy;
	
	switch (args.con.type) {
		case 'TCP': return createSlaveTcp(a, args.con, args.data, cbs);
		case 'RTU': return createSlaveRtu(a, args.con, args.data, cbs);
	}
}

function makeMasterApi(a, ctx) {
	var api = {};
	
	api.getBit = function (adr) {
		return dataChange(a, { func: 'getBit', ctx: ctx, adr: adr });
	};
	
	api.getInputBit = function (adr) {
		return dataChange(a, { func: 'getInputBit', ctx: ctx, adr: adr });
	};
	
	api.getReg = function (adr) {
		return dataChange(a, { func: 'getReg', ctx: ctx, adr: adr });
	};
	
	api.getInputReg = function (adr) {
		return dataChange(a, { func: 'getInputReg', ctx: ctx, adr: adr });
	};
	
	api.getBits = function (adr, count) {
		return dataChange(a, { func: 'getBits', ctx: ctx, adr: adr, count: count });
	};
	
	api.getInputBits = function (adr, count) {
		return dataChange(a, { func: 'getInputBits', ctx: ctx, adr: adr, count: count });
	};
	
	api.getRegs = function (adr, count) {
		return dataChange(a, { func: 'getRegs', ctx: ctx, adr: adr, count: count });
	};
	
	api.getInputRegs = function (adr, count) {
		return dataChange(a, { func: 'getInputRegs', ctx: ctx, adr: adr, count: count });
	};
	
	api.setBit = function (adr, val) {
		dataChange(a, { func: 'setBit', ctx: ctx, adr: adr, val: val });
	};
	
	api.setReg = function (adr, val) {
		dataChange(a, { func: 'setReg', ctx: ctx, adr: adr, val: val });
	};
	
	api.setBits = function (adr, val) {
		dataChange(a, { func: 'setBits', ctx: ctx, adr: adr, val: val });
	};
	
	api.setRegs = function (adr, val) {
		dataChange(a, { func: 'setRegs', ctx: ctx, adr: adr, val: val });
	};
	
	api.setSlave = function(val) {
		dataChange(a, { func: 'setSlave', ctx: ctx, val: val });
	};
	
	api.getContext = function () {
		return ctx;
	};
	
	return api;
}

function createMasterTcp(a, con, cbs) {
	var isWorking = false;
	
	// создаем контекст подключения
	var ctx = mb.new_tcp(con.ip, con.port);
	if (!ctx) {
		a.err('createMasterTcp: ' + mb.strerror());
		return null;
	}
	
	if (DBG) mb.set_debug(ctx, 1);
	
	var api = makeMasterApi(a, ctx);
	
	api.destroy = function () {
		isWorking = false;
		
		mb.close(ctx);
		mb.free(ctx);
		
		ctx = null;
		
		// чтобы не повадно было
		for (var i in api) api[i] = null;
		
		if (cbs.onDestroy) cbs.onDestroy(api);
	};
	
	mb.connect_async(ctx, function (errCode) {
		if (errCode == -1) {
			a.err('createMasterTcp: ' + mb.strerror());
			return;
		}
		
		isWorking = true;
		
		if (cbs.onConnect) cbs.onConnect(api);
	});
	
	return api;
}

function createMasterRtu(a, con, cbs) {
	var isWorking = false;
	
	// создаем контекст подключения
	var ctx = mb.new_rtu(con.device, con.baud, con.parity, con.dataBit, con.stopBit);
	if (!ctx) {
		a.err('createMasterRtu: ' + mb.strerror());
		return null;
	}
	
	if (DBG) mb.set_debug(ctx, 1);
	
	mb.set_slave(ctx, con.id);
	
	var api = makeMasterApi(a, ctx);
	
	api.destroy = function () {
		isWorking = false;
		
		mb.close(ctx);
		mb.free(ctx);
		
		ctx = null;
		
		// чтобы не повадно было
		for (var i in api) api[i] = null;
		
		if (cbs.onDestroy) cbs.onDestroy(api);
	};
	
	mb.connect_async(ctx, function (errCode) {
		if (errCode == -1) {
			a.err('createMasterRtu: ' + mb.strerror());
			return;
		}
		
		// очищаем буфер
		mb.flush(ctx);
		
		isWorking = true;
		
		if (cbs.onConnect) cbs.onConnect(api);
	});
	
	return api;
}

function createMaster(a, args) {
	// callback functions
	var cbs = {};
	cbs.onConnect = args.onConnect;
	// onDiconnect
	cbs.onDestroy = args.onDestroy;
	
	switch (args.con.type) {
		case 'TCP': return createMasterTcp(a, args.con, cbs);
		case 'RTU': return createMasterRtu(a, args.con, cbs);
	}
};

/*
reg2signed
reg2float
*/
// Decode a HEX-encoded float (2 regs) or double (4 regs) response
// params: resp (usually the response of the modbus device) (array)
function hexDecode(resp) {
	return mb.hex_decode.apply(null, resp);
}

function create() {
	var onError = null; // функция - обработчик ошибок
	
	function err(msg) {
		if (onError) onError(msg);
	}
	
	// параметры текущего контекста
	var ctxParam = {
		err: err
	};
	
	return {
		// установить функцию - обработчик ошибок
		onError: function (func) {
			onError = func;
		},
		
		// создать область с данными для сервера
		createData: function (args) {
			return createData(ctxParam, args);
		},
		
		// создать подключение
		createConTcp: createConTcp,
		createConRtu: createConRtu,
		
		// создать ведущее устройство (клиент)
		createMaster: function (args) {
			return createMaster(ctxParam, args);
		},
		
		// создать ведомое устройство (серер)
		createSlave: function (args) {			
			return createSlave(ctxParam, args);
		}
	};
}

module.exports = {
	native: mb,
	create: create
};
