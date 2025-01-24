//
// Created by liyu on 3/7/23.
//

#ifndef PIXELS_COLUMNVECTOR_H
#define PIXELS_COLUMNVECTOR_H

/**
 * ColumnVector derived from org.apache.hadoop.hive.ql.exec.vector.
 * <p>
 * ColumnVector contains the shared structure for the sub-types,
 * including NULL information, and whether this vector
 * repeats, i.e. has all values the same, so only the first
 * one is set. This is used to accelerate query performance
 * by handling a whole vector in O(1) time when applicable.
 * <p>
 * The fields are public by design since this is a performance-critical
 * structure that is used in the inner loop of query execution.
 */

#include <iostream>
#include <memory>
#include "exception/InvalidArgumentException.h"

/**
 * ColumnVector derived from org.apache.hadoop.hive.ql.exec.vector.
 * <p>
 * ColumnVector contains the shared structure for the sub-types,
 * including NULL information, and whether this vector
 * repeats, i.e. has all values the same, so only the first
 * one is set. This is used to accelerate query performance
 * by handling a whole vector in O(1) time when applicable.
 * <p>
 * The fields are public by design since this is a performance-critical
 * structure that is used in the inner loop of query execution.
 */


//修改ColumnVector类
class ColumnVector {
public:
    uint64_t length;
    uint64_t writeIndex;
    uint64_t readIndex;
    uint64_t memoryUsage;
    bool closed;
    bool encoding;

    uint8_t * isNull;
    bool noNulls;
    uint64_t * isValid;

    explicit ColumnVector(uint64_t len, bool encoding);
    void increment(uint64_t size);
    bool isFull();
    uint64_t position();
    void resize(int size);
    virtual void close();
    virtual void reset();
    virtual void * current() = 0;
    uint64_t * currentValid();
    virtual void print(int rowCount);
    bool checkValid(int index);
    void addNull();
    virtual void ensureSize(uint64_t size, bool preserveData);

    // 增加 add 方法的纯虚函数定义
    virtual void add(const std::string& value) = 0;
    virtual void add(bool value) = 0;
    virtual void add(int64_t value) = 0;
    virtual void add(int value) = 0;
    virtual void add(double value) = 0;  // 新增 double 类型的 add 方法

    int getLength() {
        return length;
    }
};

#endif //PIXELS_COLUMNVECTOR_H
