#pragma once

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <chrono>
#include <map>

namespace mgcp
{
    static int64_t GetMicroTime()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    static bool IsStringInside(std::string &src, std::string &insider, int32_t srcIndex)
    {
        if (insider.size() > src.size() - srcIndex)
        {
            return false;
        }
        for (int32_t i = 0; i < insider.size(); ++i)
        {
            if (insider[i] != src[srcIndex + i])
            {
                return false;
            }
        }
        return true;
    }

    static std::vector<std::string> *SplitString(std::string &target, std::string &delimiter)
    {
        std::vector<std::string> *answer = new std::vector<std::string>();
        int32_t currentIndex = 0, i = 0, currentLength = 0;
        for (; i < target.size(); ++i)
        {
            if (IsStringInside(target, delimiter, i))
            {
                answer->push_back(target.substr(currentIndex, currentLength));
                currentIndex = i + (int32_t)delimiter.size();
                currentLength = 0;
            }
            else
            {
                ++currentLength;
            }
        }
        answer->push_back(target.substr(currentIndex, i));
        return answer;
    }

    static std::vector<std::string> *SplitString(std::string &target, char delimiter)
    {
        std::vector<std::string> *answer = new std::vector<std::string>();
        int32_t currentIndex = 0, i = 0, currentLength = 0;
        for (; i < target.size(); ++i)
        {
            if (delimiter == target[i])
            {
                answer->push_back(target.substr(currentIndex, currentLength));
                currentIndex = i + 1;
                currentLength = 0;
            }
            else
            {
                ++currentLength;
            }
        }
        answer->push_back(target.substr(currentIndex, i));
        return answer;
    }

    static std::vector<std::string> *SplitString(std::string &target, bool (*predicate)(char a, int32_t index, std::string &source) = nullptr)
    {
        std::vector<std::string> *answer = new std::vector<std::string>();
        if (predicate == nullptr)
            return answer;
        int32_t currentIndex = 0, i = 0, currentLength = 0;
        for (; i < target.size(); ++i)
        {
            if (predicate(target[i], i, target))
            {
                answer->push_back(target.substr(currentIndex, currentLength));
                currentIndex = i + 1;
                currentLength = 0;
            }
            else
            {
                ++currentLength;
            }
        }
        answer->push_back(target.substr(currentIndex, i));
        return answer;
    }

    static std::string StringifyVectorContents(std::vector<std::string> &vec, std::string optionalPrepend = "")
    {
        std::string s = optionalPrepend + " { ";
        for (int32_t i = 0; i < vec.size(); ++i)
        {
            s.append(vec[i]);
            if (i < vec.size() - 1)
            {
                s.append(", ");
            }
        }
        s.append(" }");
        return s;
    }

    template <typename T>
    static void PrintMapKeys(std::map<std::string, T> &map, std::string name = "")
    {
        std::cout << "PrintMap::" << name << '\n';
        for (auto itr = map.begin(); itr != map.end(); itr++)
        {
            auto key = itr->first;
            auto value = itr->second;
            std::cout << "\tkey: " << key << '\n';
        }
        std::cout << "PrintMap:: finished" << name << '\n';
    }

    static void PrintVector(std::vector<std::vector<int32_t>> &vec, std::string optionalPrepend = "")
    {
        std::string s = optionalPrepend + "{\n";
        for (int32_t i = 0; i < vec.size(); ++i)
        {
            s.append("\t{ ");
            for (int32_t j = 0; j < vec[i].size(); ++j)
            {
                s.append(std::to_string(vec[i][j]));
                if (j < vec[i].size() - 1)
                {
                    s.append(", ");
                }
            }
            s.append("}\n");
        }
        std::cout << s << "}"
                  << '\n';
    }

    static void PrintVector(std::vector<int32_t> &vec, std::string optionalPrepend = "")
    {
        std::string s = optionalPrepend + "{ ";
        for (int32_t i = 0; i < vec.size(); ++i)
        {
            s.append(std::to_string(vec[i]));
            if (i < vec.size() - 1)
            {
                s.append(", ");
            }
        }
        std::cout << s << " }"
                  << '\n';
    }

    static void PrintVector(std::vector<std::string> &vec, std::string optionalPrepend = "")
    {
        std::string s = optionalPrepend + "{ ";
        for (int32_t i = 0; i < vec.size(); ++i)
        {
            s.append(vec[i]);
            if (i < vec.size() - 1)
            {
                s.append(", ");
            }
        }
        std::cout << s << " }"
                  << '\n';
    }

    static void PrintVector(std::vector<char> &vec, std::string optionalPrepend = "")
    {
        std::string s = optionalPrepend + "{ ";
        for (int32_t i = 0; i < vec.size(); ++i)
        {
            s.push_back(vec[i]);
            if (i < vec.size() - 1)
            {
                s.append(", ");
            }
        }
        std::cout << s << " }"
                  << '\n';
    }

} // namespace mgcp