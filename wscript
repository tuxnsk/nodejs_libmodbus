#!/usr/bin/env python
# Davydov Denis <mail@tuxnsk.ru>

import Options, Utils

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
	opt.tool_options("compiler_cxx")

def configure(conf):
	conf.check_tool("compiler_cxx")
	conf.check_tool("node_addon")
	#conf.check_cxx(lib="modbus", errmsg="Missing libmodbus")
	conf.env.append_unique('CXXFLAGS', ["-Wall"])
	
	# static
	conf.env.append_unique('CXXFLAGS', ["-I../libmodbus/src/"])
	
	# shared
	#conf.env.append_unique('CXXFLAGS', Utils.cmd_output('pkg-config --cflags --libs libmodbus').split())

def build(bld):
	obj = bld.new_task_gen("cxx", "shlib", "node_addon")
	# add static
	bld.env.append_value('LINKFLAGS', '../libmodbus/src/.libs/modbus.o')
	bld.env.append_value('LINKFLAGS', '../libmodbus/src/.libs/modbus-data.o')
	bld.env.append_value('LINKFLAGS', '../libmodbus/src/.libs/modbus-rtu.o')
	bld.env.append_value('LINKFLAGS', '../libmodbus/src/.libs/modbus-tcp.o')
	obj.target = "modbus_binding"
	obj.source = "src/main.cpp"
	#obj.lib = 'modbus'
