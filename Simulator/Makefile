all:
	cmake -S . -B build -DENABLE_LOGGING=OFF
	cmake --build build

log:
	cmake -S . -B build -DENABLE_LOGGING=ON
	cmake --build build

clean:
	rm -rf build
