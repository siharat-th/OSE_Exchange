
Instructions for building on the command line:

1. Run "nmake /f Makefile.windows"


Instructions for building in Visual Studio:

1. Copy the following files from the zip file to any location of your choice. 

quoteApiClientDemo.cc quoteSystemApi.cc thread.cc socket.cc socketProcessor.cc common_util.c sessionManager.c SpreadDefinition.cc CmdLineHandler.cc util.cc Price.cc timestamp.cc

2. In Project->Properties->C/C++->Preprocessor add WIN32;_CRT_SECURE_NO_DEPRECATE if these flags aren't already there.

3. In Project->Properties->C/C++->Compile As, change it to "Compile As C++ Code (/TP)".

4. In Project->Properties->Linker->Input, add ws2_32.lib to the list of libraries.

5. Now you should be able to build the solution and run the demo client. 



Instructions for building in Visual Studio using the packaged Makefile.windows:

1. After downloading the API, unzip it and copy the contents of the c++ directory to a directory of your choice. 

2. In Visual Studio, create a new Makefile project (File->New Project->Makefile Project). Give the project the same name as the directory you named above. Make sure "Create a directory for solution" is not checked. Click next.

3. Set build command line to "nmake /f Makefile.windows" and  set the clean command to "nmake /f Makefile.windows clean". Click finish.

4. You can now click Build->Build Solution to build the demo client that is shipped with the API. The demo client will be in the same directory that you copied all the files to earlier.

