all: 
	echo "Make download or parse"


data/bodies.json:
	cd data && aria2c -k1M  -s50 -j50 -x 50 --optimize-concurrent-downloads  --file-allocation=none https://www.edsm.net/dump/bodies.json
data/systemsPopulated.json:
	cd data && aria2c  -k1M  -s50 -j50 -x 50 --optimize-concurrent-downloads  --file-allocation=none https://www.edsm.net/dump/systemsPopulated.json

data/systemsWithCoordinates.json:
	cd data && aria2c  -k1M  -s50 -j50 -x 50 --optimize-concurrent-downloads  --file-allocation=none https://www.edsm.net/dump/systemsWithCoordinates.json


download-prepare:
	mkdir -p data
	cd data && rm -rf * 
download:  data/bodies.json data/systemsPopulated.json data/systemsWithCoordinates.json



swift/valuable-bodies.jsonl: data/tmp/bodies-aaaa
	echo "Finding valuable bodies..."
	(echo data/tmp/bodies-* | xargs -P 24 -n 1 ggrep  -i -E '((earth-like|water|ammonia) world)|(Candidate for terra)' ) | gsed 's/,$$//' > swift/valuable-bodies.jsonl

data/tmp/systems-aaaa: data/systemsWithCoordinates.json
	echo "Splitting systems file..."
	mkdir -p data/tmp
	rm -f data/tmp/systems-*
	cd data/tmp && gsed 's/,$$//' < ../systemsWithCoordinates.json | gsplit -l 50000  -a 4 - systems-

data/tmp/bodies-aaaa: data/bodies.json
	echo "Splitting bodies file."
	mkdir -p data/tmp
	rm -f data/tmp/bodies-*
	cd data/tmp &&  gsplit -u  -l 500000 -a 4 ../../data/bodies.json bodies-

data/systems-populated.jsonl: data/systemsPopulated.json
	gsed 's/,$$//' < data/systemsPopulated.json > data/systems-populated.jsonl

data/bodies.jsonl: data/bodies.json
	gsed 's/,$$//' < data/bodies.json > data/bodies.jsonl

parse: data/tmp/systems-aaaa swift/valuable-bodies.jsonl
# data/systems-populated.jsonl 

build-map-coords: parse swift/valuable-systems.csv
	cd swift && swift build --product coordinate-mapper -c release && ./.build/x86_64*/release/coordinate-mapper

map-coords: 
	rm ../resources/valsys-*.gz ../resources/resources-valsys-*.qrc || true
	split -n l/20 swift/valuable-systems.csv ../resources/valsys-
	pigz -9 ../resources/valsys-*
	zsh -c 'cd ../resources && for a in valsys-*.gz; do cat valsys.qrc.tmpl |sed "s/FILENAME/$$a/" > resources-$${a:s/gz/qrc/}; done'
