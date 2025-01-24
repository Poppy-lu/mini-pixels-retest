/*
 * Copyright 2024 PixelsDB.
 *
 * This file is part of Pixels.
 *
 * Pixels is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Pixels is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 *
 * You should have received a copy of the Affero GNU General Public
 * License along with Pixels.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#include "writer/FloatColumnWriter.h"
#include "utils/BitUtils.h"
#include "encoding/EncodingUtils.h"
#include "io/ByteBuffer.h"
#include <iostream>

// 构造函数：初始化 FloatColumnWriter
FloatColumnWriter::FloatColumnWriter(std::shared_ptr<TypeDescription> type, std::shared_ptr<PixelsWriterOption> writerOption)
    : ColumnWriter(type, writerOption), curPixelVector(pixelStride) {
    // 根据编码级别决定是否使用运行长度编码
    runlengthEncoding = writerOption->getEncodingLevel().ge(EncodingLevel::Level::EL2);
    if (runlengthEncoding) {
        encoder = std::make_unique<RunLenIntEncoder>();
    }
}

// 写入数据：将 ColumnVector 中的 Float 数据写入当前像素
int FloatColumnWriter::write(std::shared_ptr<ColumnVector> vector, int size) {
    std::cout << "In FloatColumnWriter" << std::endl;
    auto columnVector = std::static_pointer_cast<FloatColumnVector>(vector);
    if (!columnVector) {
        throw std::invalid_argument("Invalid vector type");
    }

    float* values = columnVector->floatVector; // 获取 Float 数据

    int curPartLength;         // 当前像素分区的长度
    int curPartOffset = 0;     // 当前像素分区的起始偏移量
    int nextPartLength = size; // 下一个像素分区的长度

    // 分区逻辑：将数据划分为当前像素和下一个像素
    while ((curPixelIsNullIndex + nextPartLength) >= pixelStride) {
        curPartLength = pixelStride - curPixelIsNullIndex;
        writeCurPartFloat(columnVector, values, curPartLength, curPartOffset);
        newPixel();
        curPartOffset += curPartLength;
        nextPartLength = size - curPartOffset;
    }

    curPartLength = nextPartLength;
    writeCurPartFloat(columnVector, values, curPartLength, curPartOffset);

    return outputStream->getWritePos();
}

// 关闭写入器：清理资源
void FloatColumnWriter::close() {
    if (runlengthEncoding && encoder) {
        encoder->clear(); // 清理编码器资源
    }
    ColumnWriter::close();
}

// 写入当前分区 Float 数据
void FloatColumnWriter::writeCurPartFloat(std::shared_ptr<ColumnVector> columnVector, float* values, int curPartLength, int curPartOffset) {
    for (int i = 0; i < curPartLength; i++) {
        curPixelEleIndex++;
        if (columnVector->isNull[i + curPartOffset]) {
            hasNull = true;
            if (nullsPadding) {
                // 对空值进行填充处理
                curPixelVector[curPixelVectorIndex++] = 0.0f;
            }
        } else {
            curPixelVector[curPixelVectorIndex++] = values[i + curPartOffset];
        }
    }
    // 将空值标记复制到当前像素的空值数组中
    std::copy(columnVector->isNull + curPartOffset, columnVector->isNull + curPartOffset + curPartLength, isNull.begin() + curPixelIsNullIndex);
    curPixelIsNullIndex += curPartLength;
}

// 决定是否填充空值
bool FloatColumnWriter::decideNullsPadding(std::shared_ptr<PixelsWriterOption> writerOption) {
    if (writerOption->getEncodingLevel().ge(EncodingLevel::Level::EL2)) {
        return false; // 如果编码级别高于 EL2，则不填充空值
    }
    return writerOption->isNullsPadding();
}

// 创建新像素：将当前像素数据写入输出流
void FloatColumnWriter::newPixel() {
    if (runlengthEncoding) {
        std::vector<byte> buffer(curPixelVectorIndex * sizeof(float));
        int resLen;
        encoder->encode(curPixelVector.data(), buffer.data(), curPixelVectorIndex, resLen);
        outputStream->putBytes(buffer.data(), resLen);
    } else {
        std::shared_ptr<ByteBuffer> curVecPartitionBuffer = std::make_shared<ByteBuffer>(curPixelVectorIndex * sizeof(float));
        EncodingUtils encodingUtils;
        if (byteOrder == ByteOrder::PIXELS_LITTLE_ENDIAN) {
            for (int i = 0; i < curPixelVectorIndex; i++) {
                encodingUtils.writeFloatLE(curVecPartitionBuffer, curPixelVector[i]);
            }
        } else {
            for (int i = 0; i < curPixelVectorIndex; i++) {
                encodingUtils.writeFloatBE(curVecPartitionBuffer, curPixelVector[i]);
            }
        }
        outputStream->putBytes(curVecPartitionBuffer->getPointer(), curVecPartitionBuffer->getWritePos());
    }

    ColumnWriter::newPixel();
}

// 获取列块编码方式
pixels::proto::ColumnEncoding FloatColumnWriter::getColumnChunkEncoding() const {
    pixels::proto::ColumnEncoding columnEncoding;
    if (runlengthEncoding) {
        columnEncoding.set_kind(pixels::proto::ColumnEncoding::Kind::ColumnEncoding_Kind_RUNLENGTH);
    } else {
        columnEncoding.set_kind(pixels::proto::ColumnEncoding::Kind::ColumnEncoding_Kind_NONE);
    }
    return columnEncoding;
}