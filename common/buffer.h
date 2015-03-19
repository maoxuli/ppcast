//
//  buffer.h
//
//  Created by Maoxu Li on 8/10/13.
//  Copyright (c) 2013 Limlabs. All rights reserved.
//

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "network.h"
#include <string>
#include <vector>
#include <cassert>

//
// Buffer of bytes
// Based on std::vector of unsigned char
//

class Buffer
{
public:
    Buffer(size_t limit = 0); // limit the max size of buffer
    Buffer(const unsigned char* b, size_t n); // Create with a memory block
    ~Buffer();
    
    // Empty the buffer
    void clear()
    {
        _read_index = 0;
        _write_index = 0; // ?Will thrink capacity?
    }
    
    // Swap two buffers without copy
    void swap(Buffer& b)
    {
        _v.swap(b._v);
        std::swap(_read_index, b._read_index);
        std::swap(_write_index, b._write_index);
        std::swap(_max_size, b._max_size);
    }
    
    // Swap two buffers without copy
    void swap(Buffer* b)
    {
        _v.swap(b->_v);
        std::swap(_read_index, b->_read_index);
        std::swap(_write_index, b->_write_index);
        std::swap(_max_size, b->_max_size);
    }
    
    // Available bytes to read
    size_t readable() const
    {
        return _write_index - _read_index;
    }
    
    // Discard bytes from the begining of readalbe bytes
    void remove(size_t n)
    {
        if(readable() > n)
        {
            _read_index += n;
        }
        else
        {
            _read_index = 0;
            _write_index = 0;
        }
    }
    
    // Free bytes to write
    size_t writable() const
    {
        return _v.size() - _write_index;
    }
    
    // Ensure a writable bytes
    size_t ensure(size_t n)
    {
        if(n == 0 || writable() >= n)
        {
            return writable();
        }
        
        if(_max_size > 0 && (_v.size() + n) > _max_size)
        {
            _v.resize(_max_size);
        }
        else
        {
            _v.resize(_v.size() + n);
        }
        
        return writable();
    }
    
    //
    // Write data to buffer
    // Always append data immediately after readable data
    //
    
    // unsigned char src[100];
    // buf.Reserve(100);
    // memcpy(buf.Write(), src, 100);
    // buf.write(100);
    //
    // or:
    // unsigned char src[100];
    // buf.write(src, 100);
    
    const unsigned char* write() const
    {
        return _begin() + _write_index;
    }
    
    unsigned char* write()
    {
        return _begin() + _write_index;
    }
    
    void write(size_t n)
    {
        assert(_write_index + n <= _v.size());
        _write_index += n;
    }
        
    size_t write(const unsigned char* b, size_t n);
    size_t write(Buffer* buffer);
    size_t write(const Buffer& buffer);
    
    // Write particular data type
    size_t write8(int8_t v);
    size_t write8u(uint8_t v);
    
    size_t write16(int16_t v);
    size_t write16u(uint16_t v);
    
    size_t write32(int32_t v);
    size_t write32u(uint32_t v);
    
    size_t write64(int64_t v);
    size_t write64u(uint64_t v);
    
    size_t writeBool(bool v);
    size_t writeFloat(float v);
    size_t writeDouble(double v);
    
    size_t writeString(const std::string& s);
    size_t writeString(const std::string& s, const char delim);
    size_t writeString(const std::string& s, const std::string& delim);

    //
    // Read data in buffer
    // Always read from the first readable byte
    // The data will be removed from buffer after read
    //
    
    // unsigned char dst[20];
    // memcpy(dst, buf.read(), 20);
    // buf.read(20);
    // [or buf.remove(20);]
    //
    // or:
    // unsigned char dst[20];
    // buf.read(dst, 20);
    
    const unsigned char* read() const
    {
        return _begin() + _read_index;
    }
    
    void read(size_t n)
    {
        remove(n);
    }
    
    bool read(unsigned char* b, size_t n);

    // Read with particular data type
    int8_t read8();
    uint8_t read8u();
    
    int16_t read16();
    uint16_t read16u();
    
    int32_t read32();
    uint32_t read32u();
    
    int64_t read64();
    uint64_t read64u();
    
    bool readBool();
    float readFloat();
    double readDouble();
    
    bool readString(std::string& s, size_t n);
    bool readString(std::string& s, const char delim);
    bool readString(std::string& s, const std::string& delim);
        
    //
    // Peek data in buffer
    // Data is not removed from the buffer
    // Can peek data at a random position
    //
    
    const unsigned char* peek(size_t offset = 0) const
    {
        return readable() > offset ? read() + offset : NULL;
    }
    
    // Peek particular data type
    int8_t peek8(size_t offset = 0) const;
    uint8_t peek8u(size_t offset = 0) const;
    
    int16_t peek16(size_t offset = 0) const;
    uint16_t peek16u(size_t offset = 0) const;
    
    int32_t peek32(size_t offset = 0) const;
    uint32_t peek32u(size_t offset = 0) const;
    
    int64_t peek64(size_t offset = 0) const;
    uint64_t peek64u(size_t offset = 0) const;
    
    bool peekBool(size_t offset = 0) const;
    float peekFloat(size_t offset = 0) const;
    double peekDouble(size_t offset = 0) const;
    
    bool peekString(std::string& s, size_t n, size_t offset = 0) const;
    bool peekString(std::string& s, const char delim, size_t offset = 0) const;
    bool peekString(std::string& s, const std::string& delim, size_t offset = 0) const;
    
    //
    // Update data in buffer
    // Particular data type and offset determine the bytes that will be modified
    // data is updated based on the same structure (size)
    //
    
    // Update particular data type
    void update8(int8_t v, size_t offset = 0);
    void update8u(uint8_t v, size_t offset = 0);
    
    void update16(int16_t v, size_t offset = 0);
    void update16u(uint16_t v, size_t offset = 0);
    
    void update32(int32_t v, size_t offset = 0);
    void update32u(uint32_t v, size_t offset = 0);
    
    void update64(int64_t v, size_t offset = 0);
    void update64u(uint64_t v, size_t offset = 0);
    
    void updateBool(bool v, size_t offset = 0);
    void updateFloat(float v, size_t offset = 0);
    void updateDouble(double v, size_t offset = 0);
    
    // Receive data from socket and write into buffer
    ssize_t receive(SOCKET fd);
    ssize_t receiveFrom(SOCKET fd, struct sockaddr_storage* from);
    
    // Send data in buffer to socket
    ssize_t send(SOCKET fd);
    ssize_t sendTo(SOCKET fd, struct sockaddr_storage* to);
    
private:
    
    //
    //        |                         size()                           |
    // vector |##########################################################|-------------|
    //      begin()                                                     end()       capacity
    //
    //
    //                         readable()               writable()
    //                 |                                                 |
    // Buffer |--------|#########################|***********************|-------------|
    //                                     
    //     _begin() _read_index             _write_index                end()       capacity
    //
    //
    
    unsigned char* _begin()
    {
        return &_v[0];
    }
    
    const unsigned char* _begin() const
    {
        return &_v[0];
    }

    std::vector<unsigned char> _v;
    size_t _max_size; // Limitation of the size of the container
    size_t _read_index; // Index of first readable
    size_t _write_index; // Index of first writable
};

#endif 

