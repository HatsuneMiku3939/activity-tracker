all:
	cl User32.lib Ws2_32.lib Advapi32.lib /EHsc /Fetracker.exe *.cpp
