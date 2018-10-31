all: 
	echo "Make download or parse"

download:
	mkdir -p data
	cd data && \
	rm -f * && \
	aria2c -Z -k1M  -s10 -j30 -x 16 --optimize-concurrent-downloads  --file-allocation=none https://www.edsm.net/dump/bodies.json https://www.edsm.net/dump/systemsPopulated.json https://www.edsm.net/dump/systemsWithCoordinates.json


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

map-coords: parse
	cd swift && swift build --product coordinate-mapper -c release && ./.build/x86_64-apple-macosx10.10/release/coordinate-mapper
	gzip < swift/valuable-systems.csv > ../resources/valuable-systems.csv.gz
