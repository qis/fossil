all: build/Release/main

run: build/Debug/main
	build/Debug/main

build/%/main: build/build.ninja
	cmake --build build --config $*

build/build.ninja: CMakeLists.txt CMakePresets.json
	cmake --preset default

clean:
	rm -rf build

.PHONY: all run clean
