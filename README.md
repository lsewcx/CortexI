## 配置环境
```bash
conan install . --output-folder=build --build=missing

source build/build/Release/generators/conanbuild.sh
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=build/build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
```

## 测试
· 运行测试
```bash
ctest --verbose
```