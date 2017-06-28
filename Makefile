MAKE_FLAGS=-j5

release:
	mkdir -p build;
	cd build; cmake -DCMAKE_BUILD_TYPE=Release ..;
	cd build; ${MAKE} ${MAKE_FLAGS};

debug:
	mkdir -p build;
	cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..;
	cd build; ${MAKE} ${MAKE_FLAGS};

demo: release
	./build/osg-robot-visualizer -c data_files/hrp4_stairs.yaml -e

clean:
	rm -Rf ./build
	cd video_output; ${MAKE} ${MAKE_FLAGS} clean


.PHONY: release debug clean
