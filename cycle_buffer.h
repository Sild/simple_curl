#pragma once
#include <string>
#include <stdexcept>
#include <cstring>

namespace NCustom {

template<size_t TSize>
class TCycleBuffer {
public:
    TCycleBuffer() = default;
    TCycleBuffer(const TCycleBuffer&) = delete;
    TCycleBuffer& operator=(const TCycleBuffer) = delete;

    size_t GetData(char *buffer, size_t requestedSize) {
        size_t availableSize = requestedSize > TSize ? TSize : requestedSize;
        if(!CycleReady) {
            availableSize = requestedSize > static_cast<size_t>(CurrentEnd - Begin) ? (CurrentEnd - Begin) : requestedSize;
            std::memcpy(buffer, Begin, availableSize);
        } else {
            size_t toEnd = TSize - (CurrentEnd - Begin);
            if(availableSize < toEnd) {
                std::memcpy(buffer, CurrentEnd, availableSize);
            } else {
                std::memcpy(buffer, CurrentEnd, toEnd);
                std::memcpy(buffer + toEnd, Begin, availableSize - toEnd);
            }
        }
        return availableSize;
    }

    void Append(char* buffer, size_t size) {
        if(size > TSize) {
            throw std::logic_error("Incorrect usage: TCycleBuffer overflow.");
        }
        size_t toEnd = TSize - (CurrentEnd - Begin);
        if(toEnd < size) {
            std::memcpy(CurrentEnd, buffer, toEnd);
            std::memcpy(Begin, buffer + toEnd, size - toEnd);
            CurrentEnd = Begin + size - toEnd;
            CycleReady = true;
        } else {
            std::memcpy(CurrentEnd, buffer, size);
            CurrentEnd += size;
        }
    }

private:
    char Begin[TSize];
    char* CurrentEnd = Begin;
    bool CycleReady = false;
};
}