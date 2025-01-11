# Usage
```sh
git clone https://github.com/Fighohji/generator-for-CP.git

chomod -x gen.sh

# implement your generator in gen.cpp
# implement std in main.cpp

./gen.sh $l $r
```
# 栈空间
部分题目需要开大栈空间，否则会出现问题，clang++设置栈空间为128Mb，g++开栈空间得自行查阅或者直接开大shell中的栈空间
