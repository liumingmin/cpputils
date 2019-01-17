REVISION=`svn info $(top_srcdir)|grep Revision|cut -d\  -f2`

INCLUDES = \
	-I$(top_srcdir)/include

METASOURCES = AUTO

ALL_SOURCES=\
	memwatch.c \
	app_fifo.c \
	app_objPool.c \
	app_util.c \
	app_timer.c \
	port_condv.c \
	port_list.c \
	port_port.c \
	port_sema.c \
	port_socket.c \
	port_thread.c \
	sys_dictionary.c \
	sys_iniparser.c \
	port_time.c

lib_LIBRARIES=libSystem_mw.a libSystem.a
libSystem_mw_a_SOURCES=${ALL_SOURCES}
libSystem_mw_a_CFLAGS=-DMEMWATCH -DMEMWATCH_STDIO -DHAVE_PTHREAD_H
libSystem_a_SOURCES=${ALL_SOURCES}

