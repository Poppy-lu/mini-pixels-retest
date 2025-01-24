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

#ifndef DUCKDB_DECIMALCOLUMNWRITER_H
#define DUCKDB_DECIMALCOLUMNWRITER_H

#include "writer/ColumnWriter.h"
#include "encoding/RunLenIntEncoder.h"
#include "type/TypeDescription.h"
#include "option/PixelsWriterOption.h"
#include "vector/ColumnVector.h"
#include "utils/EncodingUtils.h"

class DecimalColumnWriter : public ColumnWriter {
public:
    DecimalColumnWriter(std::shared_ptr<TypeDescription> type, std::shared_ptr<PixelsWriterOption> writerOption);

    int write(std::shared_ptr<ColumnVector> vector, int length) override;
    void close() override;
    void newPixel() override;
    pixels::proto::ColumnEncoding getColumnChunkEncoding() const override;

private:
    void writeCurPartDecimal(std::shared_ptr<ColumnVector> columnVector, int64_t* values, int curPartLength, int curPartOffset);
    bool decideNullsPadding(std::shared_ptr<PixelsWriterOption> writerOption);

    bool runlengthEncoding; // 是否使用运行长度编码（RLE）
    std::unique_ptr<RunLenIntEncoder> encoder; // 运行长度编码器
    std::vector<int64_t> curPixelVector; // 当前像素的值向量
};

#endif // DUCKDB_DECIMALCOLUMNWRITER_H