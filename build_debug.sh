echo "Сборка DEBUG версии"
rm -rf build-debug
cmake -B build-debug -S . -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
cmake --build build-debug --parallel