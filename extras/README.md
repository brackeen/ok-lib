The `build` folder is ignored via .gitignore, so it is a good place to to test and generate projects:

##  Test (Linux, Mac)
```
mkdir tmp && cd tmp && cmake .. && cmake --build . && ctest --verbose; cd .. && rm -Rf tmp
```

##  Test (Windows PowerShell)

```
mkdir tmp | Out-Null; if($?) { cd tmp }; if($?) { cmake .. }; if($?) { cmake --build . }; if($?) { ctest -C Debug --verbose }; cd .. ; rm -r tmp
```

## Test (Emscripten)

Assuming `EMSCRIPTEN_ROOT_PATH` points to active installed version of Emscripten.

```
mkdir tmp && cd tmp && cmake -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN_ROOT_PATH/cmake/Modules/Platform/Emscripten.cmake .. && cmake --build . && node ok-lib-test.js && cd .. && rm -Rf tmp
```

## Generate Xcode project
```
mkdir build && cd build && cmake -G Xcode ..
```

## Generate Visual Studio project (Windows PowerShell)
```
mkdir build | Out-Null; if($?) { cd build }; if($?) { cmake -G "Visual Studio 14 2015" .. }
```

