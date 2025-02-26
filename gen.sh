#!/bin/zsh

# 编译 C++ 程序
if command -v clang++ &> /dev/null; then
    CXX=clang++
    STACK_SIZE_OPTION="-Xlinker -stack_size -Xlinker 0x8000000"
elif command -v g++ &> /dev/null; then
    CXX=g++
    # 自行查阅如何开大占空间
    STACK_SIZE_OPTION=""
else
    echo "No suitable compiler found!"
    exit 1
fi

# 检查是否传递了足够的参数
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 l r"
    echo "l: the starting number of test cases"
    echo "r: the ending number of test cases"
    exit 1
fi

# 获取参数 l 和 r
l=$1
r=$2

# 编译 C++ 程序
$CXX $STACK_SIZE_OPTION -o main main.cpp -std=c++20 -O2

# 检查编译是否成功
if [ ! -f main ]; then
    echo "Compilation main failed!"
    exit 1
fi

# 编译生成器程序
$CXX $STACK_SIZE_OPTION -o gen gen.cpp -std=c++20 -O2

# 检查编译是否成功
if [ ! -f gen ]; then
    echo "Compilation gen failed!"
    exit 1
fi

# 生成数据
./gen $l $r

for (( count=$l; count<=$r; count++ )); do
    # 运行待测程序并保存输出
    ./main < "$count.in" > "$count.out"

    echo "Generated test case $count: $count.in -> $count.out"
done
