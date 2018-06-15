QT_MA=5
QT_MI=11
QT_PA=0
QT=${QT_MA}.${QT_MI}.${QT_PA}
QTUS=${QT_MA}_${QT_MI}_${QT_PA}

all:
	@echo mac-package or win-package

OUT := $(shell echo "output/EDPathFinder-`/usr/libexec/PlistBuddy -c 'Print CFBundleVersion' cmake-build-minsizerel/EDPathFinder.app/Contents/Info.plist`.dmg")

mac-package:  
	/workspace/Qt/${QT}/clang_64/bin/macdeployqt cmake-build-minsizerel/EDPathFinder.app   -always-overwrite
	rm -rf cmake-build-minsizerel/Contents/Frameworks/QtSql.framework
	rm -f "${OUT}" ; appdmg  cmake/appdmg.json "${OUT}"

win-package: 
	rm -rf output/EDPathFinder
	mkdir -p output/EDPathFinder
	cp cmake-build-minsizerel/EDPathFinder.exe output/EDPathFinder
	cp cmake-build-minsizerel/ssleay32.dll output/EDPathFinder
	cp cmake-build-minsizerel/libeay32.dll output/EDPathFinder
	rm -f wininstall/*.exe
	env VCINSTALLDIR="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC" /cygdrive/c/User\ Programs/Qt/${QT}/msvc2017_64/bin/windeployqt.exe output/EDPathFinder/EDPathFinder.exe
	(cd wininstall && /cygdrive/c/Program\ Files\ \(x86\)/NSIS/makensis.exe installer.nsi)
