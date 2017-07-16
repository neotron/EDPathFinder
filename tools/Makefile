all: 
	echo "Make download or parse"

download:
	mkdir -p data
	cd data && \
	rm -f * && \
	aria2c  -Z --header="Accept-Encoding: gzip, deflate, sdch" -s10 -j10 -x 10 --file-allocation=none https://eddb.io/archive/v5/systems.csv https://eddb.io/archive/v5/systems_populated.jsonl  https://eddb.io/archive/v5/bodies.jsonl

parse:
	egrep -i "((earth-like|water|ammonia) world)|(Candidate for terra)" data/bodies.jsonl > coordinate-mapper/valuable-bodies.jsonl
	mkdir -p coordinate-mapper/tmp
	rm -f coordinate-mapper/tmp/*
	cd coordinate-mapper/tmp && split -l 500000  ../../data/systems.csv

map-coords: 
	cd coordinate-mapper && ./.build/release/coordinate-mapper
	gzip < coordinate-mapper/valuable-systems.csv > ../resources/valuable-systems.csv.gz
