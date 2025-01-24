//
// Created by yuly on 05.04.23.
//

#ifndef PIXELS_DECIMALCOLUMNVECTOR_H
#define PIXELS_DECIMALCOLUMNVECTOR_H

#include "duckdb/common/types.hpp"
#include "vector/ColumnVector.h"
#include "vector/VectorizedRowBatch.h"

using PhysicalType = duckdb::PhysicalType;

//更新函数
class DecimalColumnVector : public ColumnVector {
public:
    long *vector;
    int precision;
    int scale;
    PhysicalType physical_type_;

    DecimalColumnVector(int precision, int scale, bool encoding = false);
    DecimalColumnVector(uint64_t len, int precision, int scale, bool encoding = false);
    ~DecimalColumnVector();
    void print(int rowCount) override;
    void close() override;
    void * current() override;
    int getPrecision();
    int getScale();

    // 添加 add 方法和 ensureSize 方法
    void add(const std::string& value) override;
    void add(bool value) override;
    void add(int64_t value) override;
    void add(int value) override;
    void add(double value) override;
    void ensureSize(uint64_t size, bool preserveData) override;
};

#endif // PIXELS_DECIMALCOLUMNVECTOR_H
