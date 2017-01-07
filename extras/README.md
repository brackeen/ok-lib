The `build` folder is ignored via .gitignore, so it is a good place to to test and generate projects:

##  Test
```
mkdir tmp && cd tmp && cmake .. && cmake --build . && ./ok-lib-test; cd .. && rm -rf tmp
```

## Generate Xcode project
```
mkdir build && cd build && cmake -G Xcode ..
```

## Generate Visual Studio project
```
mkdir build && cd build && cmake -G "Visual Studio 14 2015" ..
```

