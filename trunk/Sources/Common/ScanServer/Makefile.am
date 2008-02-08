lspcdir = ./../../Linux/LinuxSpec
bin_PROGRAMS = scanserver
scanserver_SOURCES = AgentsLoadStrategy.cpp AgentsLoadStrategy.h CAgentHandler.cpp CAgentHandler.h\
					CScheduler.cpp CScheduler.h CStartTrigger.cpp CStartTrigger.h DbProviderFactory.cpp\
					DbProviderFactory.h precomp.h precomp.c daemon.cpp

scanserver_CPPFLAGS = -I./../CommonFiles -I$(lspcdir) -I./../libCommon -I./../libNet
scanserver_LDADD = ../libCommon/libCommon.a\
			../libNet/libNet.a\
			../../Linux/LinuxSpec/libLinuxSpec.a\
			../tinyxml/libtinyxml.a
scanserver_LDFLAGS = -lpthread -ldl
