//
//  Copyright (C) 2016  David Hedbor <neotron@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#pragma once

#include <QtGui>
#include <QByteArray>
#ifdef Q_OS_WIN
#include <QtZlib/zlib.h>
#else

#include <zlib.h>

#endif

#define GZIP_WINDOWS_BIT 15 + 16
#define GZIP_CHUNK_SIZE 32 * 1024

class QCompressor : public QThread {
Q_OBJECT


public:
    virtual void run() override;

    explicit QCompressor(const QByteArray &input, bool compress = false)
            : QThread(), _input(input), _output(), _compress(compress), _level(-1) { }

    virtual ~QCompressor() override { }


signals:

    // For progress tracking - returns 0-100
    void progress(int progress);

    void complete(const QByteArray &output);

private:
    bool gzipCompress();

    bool gzipDecompress();


    const QByteArray _input;
    QByteArray       _output;
    bool             _compress;
    int              _level;
};
