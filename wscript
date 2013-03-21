#!/usr/bin/env python
# encoding: utf-8

APPNAME = "ramcore"
VERSION = "0.0.1"

top, out = ".", "build"

def options(opt):
    opt.load('compiler_cxx')

def configure(conf):
    conf.load('compiler_cxx')
    conf.env.CXXFLAGS = ['-std=c++11']

    conf.check_cfg(package = 'libzmq', args = '--cflags --libs',
                   uselib_store = 'zeromq')
    conf.check_cfg(package = 'jsoncpp', args = '--cflags --libs',
                   uselib_store = 'jsoncpp')

def build(bld):
    bld.stlib(name = 'ramcore',
        target = 'ramcore',
        includes = 'ramcore/include',
        source = 'ramcore/src/Message.cpp ramcore/src/Subsystem.cpp',
        use = 'zeromq jsoncpp')

    bld.program(name = 'example',
        target = 'example',
        includes = 'ramcore/include',
        source = 'ramcore/src/Example.cpp',
        use = 'zeromq jsoncpp ramcore')
