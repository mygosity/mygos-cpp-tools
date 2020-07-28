#include "dynamicobject.h"

DynamicObject::DynamicObject(const std::string &name) : m_sName(name)
{
    std::cout << "Dynamic Object created with name: " << this->m_sName << '\n';
    m_mMethodMap = {};
};

std::string DynamicObject::GetType()
{
    return boost::typeindex::type_id_runtime(this).pretty_name();
};
