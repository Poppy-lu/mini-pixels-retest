//
// Created by liyu on 12/23/23.
//

#include "vector/TimestampColumnVector.h"

TimestampColumnVector::TimestampColumnVector(int precision, bool encoding): ColumnVector(VectorizedRowBatch::DEFAULT_SIZE, encoding) {
    TimestampColumnVector(VectorizedRowBatch::DEFAULT_SIZE, precision, encoding);
}

TimestampColumnVector::TimestampColumnVector(uint64_t len, int precision, bool encoding): ColumnVector(len, encoding) {
    this->precision = precision;
    if(encoding) {
        posix_memalign(reinterpret_cast<void **>(&this->times), 64,
                       len * sizeof(long));
    } else {
        this->times = nullptr;
    }
}


void TimestampColumnVector::close() {
    if(!closed) {
        ColumnVector::close();
        if(encoding && this->times != nullptr) {
            free(this->times);
        }
        this->times = nullptr;
    }
}

void TimestampColumnVector::print(int rowCount) {
    throw InvalidArgumentException("not support print longcolumnvector.");
//    for(int i = 0; i < rowCount; i++) {
//        std::cout<<longVector[i]<<std::endl;
//		std::cout<<intVector[i]<<std::endl;
//    }
}

TimestampColumnVector::~TimestampColumnVector() {
    if(!closed) {
        TimestampColumnVector::close();
    }
}

void * TimestampColumnVector::current() {
    if(this->times == nullptr) {
        return nullptr;
    } else {
        return this->times + readIndex;
    }
}

/**
     * Set a row from a value, which is the days from 1970-1-1 UTC.
     * We assume the entry has already been isRepeated adjusted.
     *
     * @param elementNum
     * @param days
 */
void TimestampColumnVector::set(int elementNum, long ts) {
    if (elementNum >= writeIndex) {
        writeIndex = elementNum + 1;
    }
    times[elementNum] = ts;

    // 设置 isNull 数组
    if (isNull != nullptr) {
        isNull[elementNum] = false; // 标记为非空
    }

    // 更新 noNulls 标志
    if (noNulls) {
        noNulls = false; // 如果之前没有空值，现在可能有空值
    }
}


//更新add函数

void TimestampColumnVector::add(const std::string& value) {
    // 将字符串转换为时间戳并存储
    // 示例：假设字符串格式为 "YYYY-MM-DD HH:MM:SS"
    // 这里需要根据实际需求实现时间戳解析逻辑
    long ts = 0; // 将字符串解析为时间戳
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    times[writeIndex++] = ts;
    isNull[writeIndex - 1] = false;
}

void TimestampColumnVector::add(bool value) {
    // 布尔值无法直接转换为时间戳，可以抛出异常或忽略
    throw std::runtime_error("Cannot add boolean value to TimestampColumnVector");
}

void TimestampColumnVector::add(int64_t value) {
    // 将长整数转换为时间戳并存储
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    times[writeIndex++] = value;
    isNull[writeIndex - 1] = false;
}

void TimestampColumnVector::add(int value) {
    // 将整数转换为时间戳并存储
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    times[writeIndex++] = static_cast<long>(value);
    isNull[writeIndex - 1] = false;
}

void TimestampColumnVector::add(double value) {
    // 将浮点数转换为时间戳并存储
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    times[writeIndex++] = static_cast<long>(value);
    isNull[writeIndex - 1] = false;
}

void TimestampColumnVector::ensureSize(uint64_t size, bool preserveData) {
    if (length < size) {
        long *oldArray = times;
        times = new long[size];
        if (preserveData) {
            std::copy(oldArray, oldArray + length, times);
        }
        delete[] oldArray;
        length = size;
    }
}