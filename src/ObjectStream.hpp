#pragma once

#include <sstream>
#include <string>

class ObjectStream {
    std::stringstream m_stream;
    size_t m_objectCount = 0;

public:
    ObjectStream() = default;

    ObjectStream(const std::string& levelString) : m_stream(levelString) {
        m_objectCount = std::count(levelString.begin(), levelString.end(), ';');
        if (!levelString.empty() && levelString.back() != ';') {
            ++m_objectCount;
        }
    }

    std::optional<std::string> getNextObject() {
        std::string object;
        if(getline(m_stream, object, ';')) {
            return object;
        }
        return std::nullopt;
    }

    size_t getObjectCount() const {
        return m_objectCount;
    }
};