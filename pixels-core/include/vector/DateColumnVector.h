//
// Created by yuly on 06.04.23.
//

#ifndef DUCKDB_DATECOLUMNVECTOR_H
#define DUCKDB_DATECOLUMNVECTOR_H

#include "vector/ColumnVector.h"
#include "vector/VectorizedRowBatch.h"

class DateColumnVector: public ColumnVector {
public:
    int * dates;

    explicit DateColumnVector(uint64_t len = VectorizedRowBatch::DEFAULT_SIZE, bool encoding = false);
    ~DateColumnVector();
    void * current() override;
    void print(int rowCount) override;
    void close() override;
    void set(int elementNum, int days);

    // 添加 add 方法和 ensureSize 方法
    void add(const std::string& value) override;
    void add(bool value) override;
    void add(int64_t value) override;
    void add(int value) override;
    void add(double value) override;
    void ensureSize(uint64_t size, bool preserveData) override;
};
#endif // DUCKDB_DATECOLUMNVECTOR_H
