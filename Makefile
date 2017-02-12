all:
	@echo mac-package or win-package

mac-package: 
	/workspace/Qt/5.7/clang_64/bin/macdeployqt /Users/neotron/Library/Caches/CLion2016.1/cmake/generated/EDPathFinder-3f52e3e9/3f52e3e9/MinSizeRel/EDPathFinder.app   -always-overwrite
	rm output/EDPathFinder.dmg; appdmg cmake/appdmg.json output/EDPathFinder.dmg;

win-package: 
	rm -rf ../build-EliteTSP-Desktop_Qt_5_7_0_MSVC2015_64bit-Minimum\ Size\ Release/EDPathFinder
	mkdir ../build-EliteTSP-Desktop_Qt_5_7_0_MSVC2015_64bit-Minimum\ Size\ Release/EDPathFinder
	cp ../build-EliteTSP-Desktop_Qt_5_7_0_MSVC2015_64bit-Minimum\ Size\ Release/EDPathFinder.exe ../build-EliteTSP-Desktop_Qt_5_7_0_MSVC2015_64bit-Minimum\ Size\ Release/EDPathFinder
	rm -f wininstall/*.exe
	/cygdrive/c/User\ Programs/Qt/5.7/msvc2015_64/bin/windeployqt.exe ../build-EliteTSP-Desktop_Qt_5_7_0_MSVC2015_64bit-Minimum\ Size\ Release/EDPathFinder/EDPathFinder.exe
	(cd wininstall && /cygdrive/c/Program\ Files\ \(x86\)/NSIS/makensis.exe installer.nsi)
