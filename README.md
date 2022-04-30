### CMake build:
```shell
export ORANGE_CROSS_TOOLCHAIN_ROOT=/home/valenti/Development/Embedded/exercise8/build_orange_zero_image/host/

cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=${ORANGE_CROSS_TOOLCHAIN_ROOT}/share/buildroot/
toolchainfile.cmake ..

cmake --build .
```

### Deploy:
```scp app_test_drogon_server  user@IPADDR:~/test_server/```


