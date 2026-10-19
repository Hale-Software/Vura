# Creating Installers

`cmake -S . -B build\release`

`cmake --build build\release --config Release`

`cd build\release`

`cpack -G NSIS -C Release`
