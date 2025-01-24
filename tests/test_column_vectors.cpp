#include <iostream>
#include "vector/DateColumnVector.h"
#include "vector/DecimalColumnVector.h"
#include "vector/TimestampColumnVector.h"

void testDateColumnVector() {
    DateColumnVector dateVector(10, true); // 长度为 10，启用编码

    // 测试 add 方法
    dateVector.add("2023-10-01"); // 添加日期字符串
    dateVector.add(19458);        // 添加天数（从 1970-01-01 开始）
    dateVector.add(19459);        // 添加天数

    // 打印结果
    std::cout << "DateColumnVector values:" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "Index " << i << ": " << static_cast<int*>(dateVector.current())[i] << std::endl;
        dateVector.increment(1); // 移动到下一个元素
    }
}

void testDecimalColumnVector() {
    DecimalColumnVector decimalVector(10, 18, 2, true); // 长度为 10，精度 18，小数位数 2，启用编码

    // 测试 add 方法
    decimalVector.add("123.45"); // 添加小数字符串
    decimalVector.add(67890);    // 添加整数值
    decimalVector.add(3.14159);  // 添加浮点值

    // 打印结果
    std::cout << "DecimalColumnVector values:" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "Index " << i << ": " << static_cast<long*>(decimalVector.current())[i] << std::endl;
        decimalVector.increment(1); // 移动到下一个元素
    }
}

void testTimestampColumnVector() {
    TimestampColumnVector timestampVector(10, 3, true); // 长度为 10，精度 3，启用编码

    // 测试 add 方法
    timestampVector.add("2023-10-01 12:00:00"); // 添加时间戳字符串
    timestampVector.add(1696156800);           // 添加时间戳（秒数）
    timestampVector.add(1696243200);           // 添加时间戳

    // 打印结果
    std::cout << "TimestampColumnVector values:" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "Index " << i << ": " << static_cast<long*>(timestampVector.current())[i] << std::endl;
        timestampVector.increment(1); // 移动到下一个元素
    }
}

int main() {
    std::cout << "Testing DateColumnVector:" << std::endl;
    testDateColumnVector();

    std::cout << "\nTesting DecimalColumnVector:" << std::endl;
    testDecimalColumnVector();

    std::cout << "\nTesting TimestampColumnVector:" << std::endl;
    testTimestampColumnVector();

    return 0;
}