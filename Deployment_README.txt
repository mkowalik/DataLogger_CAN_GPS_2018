From: https://www.walletfox.com/course/qtinstallerframeworkexample.php

1. Open your project in Qt Creator and compile it in the Release mode.
2. Go to the release folder and delete everything except for the executable.
3. Go to C:\Users\kowal\Desktop\AGHRacingRepos\GitHub_DAQSoft\build-DataLoggerPC-Desktop_Qt_5_12_0_MinGW_64_bit-Release\release
4. Run the win deployment tool, this should be something similar to:

C:\Qt\5.6\mingw49_32\bin\windeployqt.exe .

5. The release folder should now contain most of the necessary DLLs.
Additionally (if you are using MinGW/gcc), copy the following dlls from the location of your compiler (C:\Qt\5.6\mingw49_32\bin) into the release folder: libgcc_s_dw2-1.dll, libwinpthread-1.dll, libstdc++-6.dll.
6. Try to run the executable directly from the release folder, make sure it does not complain about any missing libraries.
---
7. Go to the following url: https://download.qt.io/official_releases/qt-installer-framework/ and download the latest edition release of the Qt Installer Framework for MS Windows.
8. Install the framework. If the installation was successful, you should find an executable binarycreator.exe in the folder C:\Qt\QtIFW-3.1.1\bin
9. Go to the folder C:\Qt\QtIFW-3.1.1\examples, copy the directory startmenu into one of your working folders, e.g. Desktop or Documents.
10. Open your copy of the folder startmenu and go to startmenu -> config -> config.xml.
11. Open config.xml in any text editor and modify the text to match your project. 

12. Go to the folder startmenu -> packages -> org.qtproject.ifw.example -> data.
13. Delete README.txt from the data folder.
14. Move all the contents from your project release folder into this data folder.
15. Go to the folder startmenu -> packages -> org.qtproject.ifw.example -> meta -> package.xml.
16. Open package.xml in any text editor and modify the text to match your project data. 

17. Go startmenu -> packages -> org.qtproject.ifw.example -> meta and open installscript.qs in Qt Creator.
18. Modify the installscript.qs in the following manner: Change "@TargetDir@/README.txt" to @TargetDir@/ + the name of your executable (in this case, rpn_calculator.exe).
19. Change "@StartMenuDir@/README.lnk" to @@StartMenuDir@/ + whatever you would like to appear in the Start Menu of MS Windows(in this case, RPN Calculator.lnk)
20. Remove the keywords which handle icons, i.e. "iconPath=%SystemRoot%/system32/SHELL32.dll", "iconId=2". 


21. In the command prompt go to the folder startmenu.
22. Paste the following line into the command prompt:

C:\QT\QtIFW-3.1.1\bin\binarycreator.exe --offline-only -c config/config.xml -p packages AGHRacingLogger_ver-0.2.1_installer.exe

23. Executable of the installer should now appear in the startmenu project folder.
24. Try to run the installer. After successful installation, you should find the application shortcut in the start menu of Windows. You should also find the executable and dlls in the folder where you installed the application.
25. You can uninstall the application via the Control Panel of MS Windows or via maintenancetool.exe that can be found in the folder where you installed the application.
