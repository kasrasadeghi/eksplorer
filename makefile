.Default: run
run:
	cd build && rm -rf *
	cd build && cmake ..
	cd build && make
	cd build && ./Main
