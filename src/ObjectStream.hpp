#pragma once

#include <sstream>
#include <string>

class ObjectStream {
    std::stringstream m_stream;
    size_t m_objectCount = 0;

public:
    ObjectStream() = default;

    ObjectStream(const std::string& levelString) {
        m_stream = std::stringstream(levelString);
        m_objectCount = std::count(levelString.begin(), levelString.end(), ';');
    }

    std::string getNextObject() {
        std::string object;
        if(getline(m_stream, object, ';')) {
            return object;
        }
        return "";
    }

    size_t getObjectCount() const {
        return m_objectCount;
    }
};