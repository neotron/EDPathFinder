all: 
	echo "Make download or parse"


#data/bodies.json.gz:
#	cd data && aria2c -k1M  -s16 -j16 -x 16 --optimize-concurrent-downloads  --file-allocation=none https://www.edsm.net/dump/bodies.json.gz
data/systemsPopulated.json.gz:
	cd data && aria2c  -k1M  -s16 -j16 -x 16 --optimize-concurrent-downloads  --file-allocation=none https://www.edsm.net/dump/systemsPopulated.json.gz

data/systemsWithCoordinates.json.gz:
	cd data && aria2c  -k1M  -s16 -j16 -x 16 --optimize-concurrent-downloads  --file-allocation=none https://www.edsm.net/dump/systemsWithCoordinates.json.gz


data/systemsPopulated.json: data/systemsPopulated.json.gz
	gunzip < data/systemsPopulated.json.gz > data/systemsPopulated.json

data/systemsWithCoordinates.json: data/systemsWithCoordinates.json.gz
	gunzip < data/systemsWithCoordinates.json.gz > data/systemsWithCoordinates.json

download-prepare:
	mkdir -p data
	cd data && rm -rf tmp systems*.gz
download:  data/systemsPopulated.json data/systemsWithCoordinates.json



swift/valuable-bodies.jsonl: data/tmp/bodies-aaaa
	echo "Finding valuable bodies..."
	(echo data/tmp/bodies-* | xargs -P 48 -n 1 ggrep  -i -E '((earth-like|water|ammonia) world)|(Candidate for terra)' ) | gsed 's/,$$//' > swift/valuable-bodies.jsonl

data/tmp/systems-aaaa: data/systemsWithCoordinates.json
	echo "Splitting systems file..."
	mkdir -p data/tmp
	rm -f data/tmp/systems-*
	cd data/tmp &&  gsed 's/,$$//' < ../systemsWithCoordinates.json | gsplit -l 50000  -a 4 - systems-

data/tmp/bodies-aaaa: data/bodies.json
	echo "Splitting bodies file."
	mkdir -p data/tmp
	rm -f data/tmp/bodies-*
	cd data/tmp &&  cat  ../../data/bodies.json| gsplit -u  -l 500000 -a 4 - bodies-

data/systems-populated.jsonl: data/systemsPopulated.json
	cat data/systemsPopulated.json |  gsed 's/,$$//'  > data/systems-populated.jsonl

data/bodies.jsonl: data/bodies.json.gz
	gsed 's/,$$//' < data/bodies.json > data/bodies.jsonl

parse: data/tmp/systems-aaaa swift/valuable-bodies.jsonl
# data/systems-populated.jsonl 

build-map-coords: parse
	cd swift && swift build --product coordinate-mapper -c release && ./.build/x86_64*/release/coordinate-mapper

map-coords: 
	rm ../resources/valsys/valsys-*.gz ../resources/valsys/*.qrc || true
	gsplit -n l/200 swift/valuable-systems.csv ../resources/valsys/valsys-
	pigz -9 ../resources/valsys/valsys-*
	zsh -c 'cd ../resources/valsys && for a in valsys-*.gz; do cat ../valsys.qrc.tmpl |sed "s/FILENAME/$$a/" > $${a:s/gz/qrc/}; done'
