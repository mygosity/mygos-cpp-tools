#pragma once

#include <string>
#include <map>
#include <functional>
#include <iostream>

#include <boost/type_index.hpp>

class DynamicObject
{
public:
    std::map<std::string, std::function<void()>> m_mMethodMap;
    std::string m_sName;

    DynamicObject(const std::string &name);
    virtual ~DynamicObject() = default;

    virtual void InvokeMethod(std::string &methodKey) = 0;

    std::string GetType();

    inline const std::string &GetName() const
    {
        return m_sName;
    }
};
