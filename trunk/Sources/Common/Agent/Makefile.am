lspcdir = ./../../Linux/LinuxSpec
bin_PROGRAMS = agent
agent_SOURCES = CAgent.h CTask.h ConnectionHandler.h MessageParser.h PluginContainer.h ServerHandler.h TaskHandler.h TempStorage.h\
				CAgent.cpp CTask.cpp ConnectionHandler.cpp MessageParser.cpp PluginContainer.cpp ServerHandler.cpp TaskHandler.cpp\
				TempStorage.cpp daemon.cpp precomp.h precomp.cpp

agent_CPPFLAGS = -I./../CommonFiles -I$(lspcdir) -I./../libCommon -I./../libNet
agent_LDADD = ../libCommon/libCommon.a\
			../libNet/libNet.a\
			../../Linux/LinuxSpec/libLinuxSpec.a\
			../tinyxml/libtinyxml.a
agent_LDFLAGS = -lpthread -ldl
