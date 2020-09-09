#ifndef MGCP_LIB_TYPES_DYNAMICOBJECT_H
#define MGCP_LIB_TYPES_DYNAMICOBJECT_H

#include <boost/type_index.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <string>

class DynamicObject {
   public:
    std::map<std::string, std::function<void()>> m_mMethodMap;
    std::string m_sName;

    DynamicObject(const std::string& name);
    virtual ~DynamicObject() = default;

    virtual void InvokeMethod(std::string& methodKey) = 0;

    std::string GetType();

    inline const std::string& GetName() const { return m_sName; }
};

#endif