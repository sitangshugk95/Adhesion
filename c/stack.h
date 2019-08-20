/*
@file   stack.h

@author Lars Pastewka <lars.pastewka@imtek.uni-freiburg.de>

@date   10 Apr 2017

@brief  Simple stack

@section LICENCE

Copyright 2015-2018 Till Junge, Lars Pastewka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __STACK_H
#define __STACK_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cassert>

class Stack {
 public:
  Stack(size_t buffer_size) {
    buffer_size_ = buffer_size;
    top_ = 0;
    tp_ = 0;
    bp_ = 0;
    is_empty_ = true;
    data_ = malloc(buffer_size_);
  }
  ~Stack() {
    free(data_);
  }

  bool is_empty() {
    return is_empty_;
  }

  size_t get_size() {
    if (is_empty_)
      return 0;
    if (tp_ >= bp_) {
      assert(top_ == 0);
      return tp_-bp_;
    }
    return top_-bp_+tp_;
  }

  size_t get_buffer_size() {
    return buffer_size_;
  }

  template<typename T> void push(T value) {
    if (tp_ >= bp_) {
      /* Check if there is enough space beyond tp_ or before bp_ */
      if (buffer_size_-tp_ < sizeof(T) && bp_ < sizeof(T)) {
        expand(2*buffer_size_);
      }
    }
    else {
      /* Check if there is enough space between tp_ and bp_ */
      if (bp_ - tp_ < sizeof(T)) {
        expand(2*buffer_size_);
      }
    }
    /* Check if we would exceed size of buffer, then wrap around */
    if (tp_+sizeof(T) > buffer_size_) {
      top_ = tp_;
      tp_ = 0;
    }

    *((T*) ((uint8_t*) data_+tp_)) = value;
    tp_ += sizeof(T);

    is_empty_ = false;
  }

  template<typename T> void pop(T &value) {
    if (tp_ == 0) {
      assert(top_ > 0);
      tp_ = top_-sizeof(T);
    }
    else {
      assert(tp_ >= sizeof(T));
      tp_ -= sizeof(T);
    }
    value = *((T*) ((uint8_t*) data_+tp_));

    is_empty_ = bp_ == tp_;
  }

  template<typename T> void pop_bottom(T &value) {
    if (bp_+sizeof(T) > buffer_size_) {
      assert(bp_ == top_);
      bp_ = 0;
    }
    value = *((T*) ((uint8_t*) data_+bp_));
    bp_ += sizeof(T);

    is_empty_ = bp_ == tp_;
  }

  template<typename T1, typename T2> void push(T1 value1, T2 value2) {
    push(value1);
    push(value2);
  }
  template<typename T1, typename T2> void pop(T1 &value1, T2 &value2) {
    pop(value2);
    pop(value1);
  }
  template<typename T1, typename T2> void pop_bottom(T1 &value1, T2 &value2) {
    pop_bottom(value1);
    pop_bottom(value2);
  }

  template<typename T1, typename T2, typename T3>
    void push(T1 value1, T2 value2, T3 value3) {
    push(value1);
    push(value2);
    push(value3);
  }
  template<typename T1, typename T2, typename T3>
    void pop(T1 &value1, T2 &value2, T3 &value3) {
    pop(value3);
    pop(value2);
    pop(value1);
  }
  template<typename T1, typename T2, typename T3>
    void pop_bottom(T1 &value1, T2 &value2, T3 &value3) {
    pop_bottom(value1);
    pop_bottom(value2);
    pop_bottom(value3);
  }

  template<typename T1, typename T2, typename T3, typename T4>
    void push(T1 value1, T2 value2, T3 value3, T4 value4) {
    push(value1);
    push(value2);
    push(value3);
    push(value4);
  }
  template<typename T1, typename T2, typename T3, typename T4>
    void pop(T1 &value1, T2 &value2, T3 &value3, T4 &value4) {
    pop(value4);
    pop(value3);
    pop(value2);
    pop(value1);
  }
  template<typename T1, typename T2, typename T3, typename T4>
    void pop_bottom(T1 &value1, T2 &value2, T3 &value3, T4 &value4) {
    pop_bottom(value1);
    pop_bottom(value2);
    pop_bottom(value3);
    pop_bottom(value4);
  }

 private:
  size_t buffer_size_;  /* Total size of the buffer holding the stack */
  size_t top_;          /* Where does the data end after wrapping tp_? */
  size_t tp_, bp_;      /* Top pointer, bottom pointer, end markers of stack */

  bool is_empty_;

  void *data_;

  void expand(size_t new_size) {
    printf("Expanding stack size to %3.2f MB.\n",
	   ((double) new_size)/(1024*1024));
    void *new_data = malloc(new_size);
    if (!new_data) {
      printf("Failed to allocate new stack!\n");
    }
    if (tp_ >= bp_) {
      assert(top_ == 0);
      memcpy(new_data, ((uint8_t *) data_+bp_), tp_-bp_);
      tp_ -= bp_;
    }
    else {
      assert(top_ > 0);
      memcpy(new_data, ((uint8_t *) data_+bp_), top_-bp_);
      memcpy(((uint8_t *) new_data+top_-bp_), data_, tp_);
      tp_ = top_-bp_+tp_;
    }
    free(data_);
    data_ = new_data;
    buffer_size_ = new_size;
    top_ = 0;
    bp_ = 0;
  }
};

#endif
