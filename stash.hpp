//
// Created by Mashtaliar on 12.05.2021.
//

#ifndef INC_4_FUNCTIONS_STASH_HPP
#define INC_4_FUNCTIONS_STASH_HPP

template <typename value_type>
class Stack
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
    Stack () : tail (new Node), temp(nullptr) {}
    /*Stack (const Stack & instance)
    {
        this->tail = new Node;
        Node* node = instance.tail;
        while (node.prev != nullptr)
        {
            this->tail = new Node {.prev = this->tail, .value = node.value}
        }
    }*/
    void push (const value_type & value)
    {
        this->tail = new Node {.prev = this->tail, .value = value};
    }
    value_type& top ()
    {
        return this->tail->value;
    }
    void pop ()
    {
        this->temp = this->tail->prev;
        delete this->tail;
        this->tail = temp;
    }
    bool empty ()
    {
        return this->tail->prev == nullptr;
    }
    void clear ()
    {
        while (!this->empty ())
        {
            this->pop ();
        }
    }
    ~Stack ()
    {
        this->clear ();
    }
};

#endif //INC_4_FUNCTIONS_STASH_HPP
