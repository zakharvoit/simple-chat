all: build

build:
	mkdir build -p
	cp -R html/ build/
	cd build; cmake .. ; make -j4

start: build
	cd build; ./simple-chat

clean:
	rm -rf build

.PHONY: build clean

