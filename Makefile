mac-package: 
	/workspace/Qt/5.7/clang_64/bin/macdeployqt /Users/neotron/Library/Caches/CLion2016.1/cmake/generated/EDPathFinder-3f52e3e9/3f52e3e9/MinSizeRel/EDPathFinder.app   -always-overwrite
	rm output/EDPathFinder.dmg; appdmg cmake/appdmg.json output/EDPathFinder.dmg;
