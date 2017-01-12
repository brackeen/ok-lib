The `build` folder is ignored via .gitignore, so it is a good place to to test and generate projects:

##  Test (Linux, Mac)
```
mkdir tmp && cd tmp && cmake .. && cmake --build . && ctest --verbose; cd .. && rm -Rf tmp
```

##  Test (Windows PowerShell)

```
mkdir tmp | Out-Null; if($?) { cd tmp }; if($?) { cmake .. }; if($?) { cmake --build . }; if($?) { ctest -C Debug --verbose }; cd .. ; rm -r tmp
```

## Generate Xcode project
```
mkdir build && cd build && cmake -G Xcode ..
```

## Generate Visual Studio project (Windows PowerShell)
```
mkdir build | Out-Null; if($?) { cd build }; if($?) { cmake -G "Visual Studio 14 2015" .. }
```

