all: 
	echo "Make download or parse"

download:
	mkdir -p data
	cd data && \
	rm -f * && \
	aria2c  -Z --header="Accept-Encoding: gzip, deflate, sdch" -s10 -j10 -x 10 --file-allocation=none https://eddb.io/archive/v5/systems.csv https://eddb.io/archive/v5/systems_populated.jsonl  https://eddb.io/archive/v5/bodies.jsonl


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

parse: data/tmp/systems-aaa swift/valuable-bodies.jsonl

map-coords: 
	cd swift && swift build --product coordinate-mapper -c release && ./.build/x86_64-apple-macosx10.10/release/coordinate-mapper
	gzip < swift/valuable-systems.csv > ../resources/valuable-systems.csv.gz
