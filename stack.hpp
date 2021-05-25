//
// Created by NebelFox on 08.05.2021.
//

#ifndef INC_4_FUNCTIONS_STACK_HPP
#define INC_4_FUNCTIONS_STACK_HPP

#include <list>
#include <iostream>

template <typename T>
class Stack
{
private:
    std::list<T> data;
public:
    Stack () = default;
//    Stack (const Stack<T> & instance)
//    {
//        for (auto i=instance.data.cbegin(); i!=instance.data.cend(); ++i)
//            this->data.push_back (*i);
//    }
    T& top () {return this->data.back ();}
    void pop () {this->data.pop_back();};
    void push (const T & item) {this->data.emplace_back (item);};
    void push (T && item) {this->data.emplace_back (item);};
    bool empty () {return this->data.empty();};
    size_t count () {return this->data.size ();};
    void clear () {this->data.clear ();};
    void reverse () {this->data.reverse ();}
    friend std::ostream& operator << (std::ostream & stream, const Stack<T> & instance)
    {
        for (auto i=instance.data.cbegin (); i != instance.data.cend (); ++i)
            stream << (*i) << ", ";
        return stream;
    }
};

#endif //INC_4_FUNCTIONS_STACK_HPP
