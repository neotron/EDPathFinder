all:
	@echo mac-package or win-package

OUT := $(shell echo "../EDPathFinder/releases/EDPathFinder-`/usr/libexec/PlistBuddy -c 'Print CFBundleVersion' cmake-build-minsizerel/EDPathFinder.app/Contents/Info.plist`.dmg")

mac-package:  
	/workspace/Qt/5.8/clang_64/bin/macdeployqt cmake-build-minsizerel/EDPathFinder.app   -always-overwrite
	rm -rf cmake-build-minsizerel/Contents/Frameworks/QtSql.framework
	rm -f "${OUT}" ; appdmg  cmake/appdmg.json "${OUT}"

win-package: 
	rm -rf output/EDPathFinder
	mkdir -p output/EDPathFinder
	cp ../build-EliteTSP-Desktop_Qt_5_8_0_MSVC2015_64bit-Minimum\ Size\ Release/EDPathFinder.exe output/EDPathFinder
	rm -f wininstall/*.exe
	env VCINSTALLDIR="c:/User Programs/Visual Studio Community 2015/VC" /cygdrive/c/User\ Programs/Qt/5.8/msvc2015_64/bin/windeployqt.exe output/EDPathFinder/EDPathFinder.exe
	(cd wininstall && /cygdrive/c/Program\ Files\ \(x86\)/NSIS/makensis.exe installer.nsi)

win-package-bash: 
	rm -rf output/EDPathFinder
	mkdir -p output/EDPathFinder
	cp ../build-EliteTSP-Desktop_Qt_5_8_0_MSVC2015_64bit-Minimum\ Size\ Release/EDPathFinder.exe output/EDPathFinder
	rm -f wininstall/*.exe
	(export VCINSTALLDIR="c:/User Programs/Visual Studio Community 2015/VC" ; /mnt/c/User\ Programs/Qt/5.8/msvc2015_64/bin/windeployqt.exe output/EDPathFinder/EDPathFinder.exe)
	(cd wininstall && /mnt/c/Program\ Files\ \(x86\)/NSIS/makensis.exe installer.nsi)
