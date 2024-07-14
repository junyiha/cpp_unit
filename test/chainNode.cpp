/**
 * @file chainNode.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <iostream>

template <class T>
struct chainNode
{
    // 数据成员
    T element;
    chainNode<T> *next;

    // 方法
    chainNode() {}
    chainNode(const T& element)
    {
        this->element = element;
    }
    chainNode(const T& element, chainNode<T>* next)
    {
        this->element = element;
        this->next = next;
    }
};

