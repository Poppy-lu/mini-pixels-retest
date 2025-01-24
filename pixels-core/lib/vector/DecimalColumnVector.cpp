//
// Created by yuly on 05.04.23.
//

#include "vector/DecimalColumnVector.h"
#include "duckdb/common/types/decimal.hpp"

/**
 * The decimal column vector with precision and scale.
 * The values of this column vector are the unscaled integer value
 * of the decimal. For example, the unscaled value of 3.14, which is
 * of the type decimal(3,2), is 314. While the precision and scale
 * of this decimal are 3 and 2, respectively.
 *
 * <p><b>Note: it only supports short decimals with max precision
 * and scale 18.</b></p>
 *
 * Created at: 05/03/2022
 * Author: hank
 */

DecimalColumnVector::DecimalColumnVector(int precision, int scale, bool encoding): ColumnVector(VectorizedRowBatch::DEFAULT_SIZE, encoding) {
    DecimalColumnVector(VectorizedRowBatch::DEFAULT_SIZE, precision, scale, encoding);
}

DecimalColumnVector::DecimalColumnVector(uint64_t len, int precision, int scale,
                                         bool encoding)
    : ColumnVector(len, encoding) {
    // decimal column vector has no encoding so we don't allocate memory to
    // this->vector
    this->vector = nullptr;
    this->precision = precision;
    this->scale = scale;

    using duckdb::Decimal;
    if (precision <= Decimal::MAX_WIDTH_INT16) {
        physical_type_ = PhysicalType::INT16;
        posix_memalign(reinterpret_cast<void **>(&vector), 32,
                       len * sizeof(int16_t));
        memoryUsage += (uint64_t)sizeof(int16_t) * len;
    } else if (precision <= Decimal::MAX_WIDTH_INT32) {
        physical_type_ = PhysicalType::INT32;
        posix_memalign(reinterpret_cast<void **>(&vector), 32,
                       len * sizeof(int32_t));
        memoryUsage += (uint64_t)sizeof(int32_t) * len;
    } else if (precision <= Decimal::MAX_WIDTH_INT64) {
        physical_type_ = PhysicalType::INT64;
        memoryUsage += (uint64_t)sizeof(uint64_t) * len;
    } else if (precision <= Decimal::MAX_WIDTH_INT128) {
        physical_type_ = PhysicalType::INT128;
        memoryUsage += (uint64_t)sizeof(uint64_t) * len;
    } else {
        throw std::runtime_error(
            "Decimal precision is bigger than the maximum supported width");
    }
}

void DecimalColumnVector::close() {
    if (!closed) {
        ColumnVector::close();
        if (physical_type_ == PhysicalType::INT16 ||
            physical_type_ == PhysicalType::INT32) {
            free(vector);
        }
        vector = nullptr;
    }
}

void DecimalColumnVector::print(int rowCount) {
//    throw InvalidArgumentException("not support print Decimalcolumnvector.");
    for(int i = 0; i < rowCount; i++) {
        std::cout<<vector[i]<<std::endl;
    }
}

DecimalColumnVector::~DecimalColumnVector() {
    if(!closed) {
        DecimalColumnVector::close();
    }
}

void * DecimalColumnVector::current() {
    if(vector == nullptr) {
        return nullptr;
    } else {
        return vector + readIndex;
    }
}

int DecimalColumnVector::getPrecision() {
	return precision;
}


int DecimalColumnVector::getScale() {
	return scale;
}


//添加add函数

void DecimalColumnVector::add(const std::string& value) {
    // 将字符串转换为小数并存储
    // 示例：假设字符串格式为 "123.456"
    // 这里需要根据实际需求实现小数解析逻辑
    long unscaledValue = 0; // 将字符串解析为未缩放的小数值
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    vector[writeIndex++] = unscaledValue;
    isNull[writeIndex - 1] = false;
}

void DecimalColumnVector::add(bool value) {
    // 布尔值无法直接转换为小数，可以抛出异常或忽略
    throw std::runtime_error("Cannot add boolean value to DecimalColumnVector");
}

void DecimalColumnVector::add(int64_t value) {
    // 将长整数转换为小数并存储
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    vector[writeIndex++] = value;
    isNull[writeIndex - 1] = false;
}

void DecimalColumnVector::add(int value) {
    // 将整数转换为小数并存储
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    vector[writeIndex++] = static_cast<long>(value);
    isNull[writeIndex - 1] = false;
}

void DecimalColumnVector::add(double value) {
    // 将浮点数转换为小数并存储
    if (writeIndex >= length) {
        ensureSize(writeIndex * 2, true);
    }
    vector[writeIndex++] = static_cast<long>(value * std::pow(10, scale));
    isNull[writeIndex - 1] = false;
}

void DecimalColumnVector::ensureSize(uint64_t size, bool preserveData) {
    if (length < size) {
        long *oldArray = vector;
        vector = new long[size];
        if (preserveData) {
            std::copy(oldArray, oldArray + length, vector);
        }
        delete[] oldArray;
        length = size;
    }
}