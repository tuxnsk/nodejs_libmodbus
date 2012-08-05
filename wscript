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
	conf.check_cxx(lib="modbus", errmsg="Missing libmodbus")
	conf.env.append_unique('CXXFLAGS', ["-Wall"])
	conf.env.append_unique('CXXFLAGS', Utils.cmd_output('pkg-config --cflags --libs libmodbus').split())

def build(bld):
	obj = bld.new_task_gen("cxx", "shlib", "node_addon")
	obj.target = "modbus_binding"
	obj.source = "src/main.cpp"
	obj.lib = 'modbus'
