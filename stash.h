//
// Created by Mashtaliar on 18.05.2021.
//

#ifndef INC_4_FUNCTIONS_STASH_H
#define INC_4_FUNCTIONS_STASH_H

#include <initializer_list>

template <typename value_type>
class Stash
{
private:
    struct Node
    {
        Node* prev = nullptr;
        value_type value;
    };

    Node* temp;
    Node* tail;

public:
    Stash ();
    Stash (std::initializer_list<value_type> l);
    ~Stash ();
    void push (const value_type & value);
    value_type& top ();
    void pop ();
    bool empty ();
    void clear ();
};

#include "stash.cpp"

#endif //INC_4_FUNCTIONS_STASH_H
