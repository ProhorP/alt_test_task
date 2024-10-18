# dependencies build
- cmake
- ctest
- gcc(or any compiler)
- libssl-devel
- rpm-build
# Bulid
```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B ./cmake-build-release
cmake --build ./cmake-build-release -j$(nproc);
```
# test
```
cd ./cmake-build-release
ctest
```
# pack
```
cd ./cmake-build-release
cpack
```
# install
```
sudo rpm -i diff_branch_pack-1.0.0-Linux-alt.rpm
```
# remove
```
sudo rpm -e diff_branch_pack-alt
```