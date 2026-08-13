echo "Сборка RELEASE версии"
rm -rf build-release
cmake -B build-release -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel