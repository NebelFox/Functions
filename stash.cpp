//
// Created by Mashtaliar on 18.05.2021.
//

#ifndef INC_4_FUNCTIONS_STASH_CPP
#define INC_4_FUNCTIONS_STASH_CPP

#include "stash.h"

template <typename value_type>
Stash<value_type>::Stash () : tail (new Node), temp(nullptr) {}

template <typename value_type>
Stash<value_type>::Stash (std::initializer_list<value_type> l) : tail (new Node), temp(nullptr)
{
    for (auto i = l.begin(); i != l.end (); ++i)
        this->push (*i);
}

template <typename value_type>
Stash<value_type>::~Stash ()
{
    this->clear ();
}

template <typename value_type>
void Stash<value_type>::push (const value_type &value)
{
    this->tail = new Node {.prev = this->tail, .value = value};
}

template <typename value_type>
value_type& Stash<value_type>::top ()
{
    return this->tail->value;
}

template <typename value_type>
void Stash<value_type>::pop ()
{
    this->temp = this->tail->prev;
    delete this->tail;
    this->tail = temp;
}

template <typename value_type>
bool Stash<value_type>::empty ()
{
    return this->tail->prev == nullptr;
}

template <typename value_type>
void Stash<value_type>::clear ()
{
    while (!this->empty ())
    {
        this->pop ();
    }
}

#endif