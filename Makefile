include ../automation/compiler.mk
include ../automation/lang.mk
include ../automation/diag.mk
include ../automation/exec_target.mk

libdummy.so: dummy_dll.cpp
	$(CC) $(LANG_VER) -fPIC -shared -o libdummy.so dummy_dll.cpp 

basic_resource_test: file_resource.hpp

basic_plugin_test: basic_plugin_test.cpp basic_plugin.hpp

all: basic_resource_test basic_register_test basic_plugin_test