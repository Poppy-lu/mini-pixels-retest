//
// Created by yuly on 06.04.23.
//

#include "vector/DateColumnVector.h"

DateColumnVector::DateColumnVector(uint64_t len, bool encoding): ColumnVector(len, encoding) {
	if(encoding) {
        posix_memalign(reinterpret_cast<void **>(&dates), 32,
                       len * sizeof(int32_t));
	} else {
		this->dates = nullptr;
	}
	memoryUsage += (long) sizeof(int) * len;
}

void DateColumnVector::close() {
	if(!closed) {
		if(encoding && dates != nullptr) {
			free(dates);
		}
		dates = nullptr;
		ColumnVector::close();
	}
}

void DateColumnVector::print(int rowCount) {
	for(int i = 0; i < rowCount; i++) {
		std::cout<<dates[i]<<std::endl;
	}
}

DateColumnVector::~DateColumnVector() {
	if(!closed) {
		DateColumnVector::close();
	}
}

/**
     * Set a row from a value, which is the days from 1970-1-1 UTC.
     * We assume the entry has already been isRepeated adjusted.
     *
     * @param elementNum
     * @param days
 */

//更新函数
void DateColumnVector::set(int elementNum, int days) {
    if (elementNum >= writeIndex) {
        writeIndex = elementNum + 1;
    }
    dates[elementNum] = days;

    // 设置 isNull 数组
    if (isNull != nullptr) {
        isNull[elementNum] = false; // 标记为非空
    }

    // 更新 noNulls 标志
    if (noNulls) {
        noNulls = false; // 如果之前没有空值，现在可能有空值
    }
}



void * DateColumnVector::current() {
    if(dates == nullptr) {
        return nullptr;
    } else {
        return dates + readIndex;
    }
}

//添加add部分
void DateColumnVector::add(const std::string& value) {
    // 将字符串转换为日期并存储
    // 示例：假设字符串格式为 "YYYY-MM-DD"
    // 这里需要根据实际需求实现日期解析逻辑
    int days = 0; // 将字符串解析为天数
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    dates[writeIndex++] = days;
    isNull[writeIndex - 1] = false;
}

void DateColumnVector::add(bool value) {
    // 布尔值无法直接转换为日期，可以抛出异常或忽略
    throw std::runtime_error("Cannot add boolean value to DateColumnVector");
}

void DateColumnVector::add(int64_t value) {
    // 将长整数转换为日期并存储
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    dates[writeIndex++] = static_cast<int>(value);
    isNull[writeIndex - 1] = false;
}

void DateColumnVector::add(int value) {
    // 将整数转换为日期并存储
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    dates[writeIndex++] = value;
    isNull[writeIndex - 1] = false;
}

void DateColumnVector::add(double value) {
    // 将浮点数转换为日期并存储
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    dates[writeIndex++] = static_cast<int>(value);
    isNull[writeIndex - 1] = false;
}

void DateColumnVector::ensureSize(uint64_t size, bool preserveData) {
    if (length < size) {
        int *oldArray = dates;
        dates = new int[size];
        if (preserveData) {
            std::copy(oldArray, oldArray + length, dates);
        }
        delete[] oldArray;
        length = size;
    }
}

