releases/appcast.xml: releases/*.dmg
	./bin/generate_appcast ~/.appcast/dsa_priv.pem releases
