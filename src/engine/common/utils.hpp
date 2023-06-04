#pragma once
#include "SFML/System/Vector2.hpp"
#include "index_vector.hpp"
#include <sstream>

//类型转换
template<typename U, typename T>
U to(const T& v)
{
    return static_cast<U>(v);
}


template<typename T>
using CIVector = civ::Vector<T>;

//判断正负
template<typename T>
T sign(T v)
{
    return v < 0.0f ? -1.0f : 1.0f;
}

//
template<typename T>
static std::string toString(T value)
{
    std::stringstream sx;
    sx << value;
    return sx.str();
}

//将类型T的vector转换成float类型的vector
template<typename T>
sf::Vector2f toVector2f(sf::Vector2<T> v)
{
    return {to<float>(v.x), to<float>(v.y)};
}
