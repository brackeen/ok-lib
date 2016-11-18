The `build` folder is ignored via .gitignore, so it is a good place to to test and generate projects:

##  Test
```
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
./ok-lib-test 
```

## Generate projects
```
mkdir build
cd build
cmake -G Xcode ..
cmake -G "Visual Studio 14 2015" ..
```
