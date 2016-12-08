all:
	-@ IF NOT EXIST "obj" MKDIR "obj"
	@cl User32.lib Ws2_32.lib Advapi32.lib /EHsc /Fo"./obj/" /Fe"tracker.exe" /I"src" /I"src/jsoncpp-1.7.7" /I"src/logger" src/*.cpp src/logger/*.cpp src/jsoncpp-1.7.7/*.cpp

style:
	@AStyle.exe -r --style=attach --indent=spaces=2 --unpad-paren --pad-header --pad-oper --close-templates *.cpp *.h >> nul 2>&1

clean:
	@DEL /F obj\*.obj >> nul 2>&1

distclean: clean
	@RMDIR /Q obj >> nul 2>&1
	@DEL /F tracker.exe >> nul 2>&1
