all: 
	echo "Make download or parse"

download:
	mkdir -p data
	cd data && \
	rm -f * && \
	aria2c -Z -k1M  -s10 -j30 -x 16 --optimize-concurrent-downloads  --file-allocation=none https://www.edsm.net/dump/bodies.json https://www.edsm.net/dump/systemsPopulated.json https://www.edsm.net/dump/systemsWithCoordinates.json


swift/valuable-bodies.jsonl: data/bodies.json
	mkdir -p data/tmp
	rm -f data/tmp/bodies-*
	cd data/tmp &&  gsplit -u --verbose -l 500000 -a 4 ../../data/bodies.json bodies-
	(echo data/tmp/bodies-* | xargs -P 12 -n 1 egrep -i '((earth-like|water|ammonia) world)|(Candidate for terra)' ) | gsed 's/,$$//' > swift/valuable-systems.jsonl
	rm -rf data/tmp/bodies-*

data/tmp/systems-aaaa: data/systemsWithCoordinates.json
	mkdir -p data/tmp
	rm -f data/tmp/systems-*
	cd data/tmp && gsed 's/,$$//' < ../systemsWithCoordinates.json | gsplit -l 50000 --verbose -a 4 - systems-

data/systems-populated.jsonl: data/systemsPopulated.json
	gsed 's/,$$//' < data/systemsPopulated.json > data/systems-populated.jsonl

data/bodies.jsonl: data/bodies.json
	gsed 's/,$$//' < data/bodies.json > data/bodies.jsonl

parse: data/tmp/systems-aaa swift/valuable-bodies.jsonl data/systems-populated.jsonl 

map-coords: 
	cd swift && swift build --product coordinate-mapper -c release && ./.build/x86_64-apple-macosx10.10/release/coordinate-mapper
	gzip < swift/valuable-systems.csv > ../resources/valuable-systems.csv.gz
