all: build

build:
	mkdir build -p
	cd build; cmake .. ; make -j4

clean:
	rm -rf build

.PHONY: build clean

