/*
C++ vector implemented with C
*/
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct vector {
    /* The function callback for the user to define the way they want to copy
     * elements */
    copy_constructor_type copy_constructor;

    /* The function callback for the user to define the way they want to destroy
     * elements */
    destructor_type destructor;

    /* The function callback for the user to define the way they a default
     * element to be constructed */
    default_constructor_type default_constructor;

    /* Void pointer to the beginning of an array of void pointers to arbitrary
     * data. */
    void **array;

    /**
     * The number of elements in the vector.
     * This is the number of actual objects held in the vector,
     * which is not necessarily equal to its capacity.
     */
    size_t size;

    /**
     * The size of the storage space currently allocated for the vector,
     * expressed in terms of elements.
     */
    size_t capacity;
};

/**
 * IMPLEMENTATION DETAILS
 *
 * The following is documented only in the .c file of vector,
 * since it is implementation specfic and does not concern the user:
 *
 * This vector is defined by the struct above.
 * The struct is complete as is and does not need any modifications.
 *
 * The only conditions of automatic reallocation is that
 * they should happen logarithmically compared to the growth of the size of the
 * vector inorder to achieve amortized constant time complexity for appending to
 * the vector.
 *
 * For our implementation automatic reallocation happens when -and only when-
 * adding to the vector makes its new  size surpass its current vector capacity
 * OR when the user calls on vector_reserve().
 * When this happens the new capacity will be whatever power of the
 * 'GROWTH_FACTOR' greater than or equal to the target capacity.
 * In the case when the new size exceeds the current capacity the target
 * capacity is the new size.
 * In the case when the user calls vector_reserve(n) the target capacity is 'n'
 * itself.
 * We have provided get_new_capacity() to help make this less ambigious.
 */

static size_t get_new_capacity(size_t target) {
    /**
     * This function works according to 'automatic reallocation'.
     * Start at 1 and keep multiplying by the GROWTH_FACTOR untl
     * you have exceeded or met your target capacity.
     */
    size_t new_capacity = 1;
    while (new_capacity < target) {
        new_capacity *= GROWTH_FACTOR;
    }
    return new_capacity;
}

/**
 * Allocate and return a pointer to a new vector (on the heap).
 *
 * If you would like to make 'shallow' copies of the elements of the 'vector',
 * then you may pass in NULL for the parameters
 * (ex. vector_create(NULL, NULL, NULL)).
 * If you would like to make 'deep' copies of the elements of the 'vector',
 * then you may pass the addresses of a particular set of
 * callback functions in for the parameters
 * (ex. vector_create(&string_copy_constructor, &string_destructor,
 * &string_default_constructor)).
 * Hint: Look at callbacks.h.
 * This means that everytime an element is to be copied or removed from the
 * 'vector' the pointer to that element is copied or removed
 * instead of using the user supplied copy constructor and destructor.
 * This also means means the default constructor will return a NULL pointer.
 */
vector *vector_create(copy_constructor_type copy_constructor,
                      destructor_type destructor,
                      default_constructor_type default_constructor) {
    size_t capacity = get_new_capacity(INITIAL_CAPACITY);

    vector * vec = (vector * )malloc(sizeof(vector));
    if (!vec) {
        perror("malloc for vector failed");
        free(vec);
        exit(1);
    }

    vec -> size = 0;
    vec -> capacity = capacity;
    vec -> array = (void **)malloc(capacity * sizeof(void *));
    if (vec -> array == NULL) {
        perror("malloc for vec -> array failed");
        free(vec -> array);
        exit(1);
    }

    if (copy_constructor == NULL) {
        vec -> copy_constructor = shallow_copy_constructor;
    } else {
        vec -> copy_constructor = copy_constructor;
    }

    if (destructor == NULL) {
        vec -> destructor = shallow_destructor;
    } else {
        vec -> destructor = destructor;
    }

    if (default_constructor == NULL) {
        vec -> default_constructor = shallow_default_constructor;
    } else {
        vec -> default_constructor = default_constructor;
    }
    return vec;
}

/**
 * Destroys all container elements by
 * calling on the user provided destructor for every element,
 * and deallocates all the storage capacity allocated by the 'vector'.
 */
void vector_destroy(vector *this) {
    assert(this);    
    for (size_t i = 0; i < this->size; i ++) {
        if (this -> array[i] != NULL) 
            this -> destructor(this -> array[i]);
    }
    free(this->array);
    free(this);
    this = NULL;
}

void **vector_begin(vector *this) {
    return this->array;
}

void **vector_end(vector *this) {
    return this->array + this->size;
}

/* number of actual objects in vector, not storage capacity */
size_t vector_size(vector *this) {
    assert(this);
    return this->size;
}

void vector_resize(vector *this, size_t n) {
    assert(this);
    // your code here
    if (n < (this -> size)) {
        for(size_t i = n; i < (this -> size); i++) {
            if (this -> array[i] != NULL)
                this -> destructor(this -> array[i]);
        }
    } else if (n > (this -> size)) {
        if (n > (this -> capacity)) {
            vector_reserve(this, n);
        }

        for (int i = this -> size; i < (int)n; i++) {
            this -> array[i] = this -> default_constructor();
        }
    }
    this -> size = n;
}

/* storage capacity allocated for vector, not vector size */
size_t vector_capacity(vector *this) {
    assert(this);
    // your code here
    return this->capacity;
}

bool vector_empty(vector *this) {
    assert(this);
    // your code here
    if (this->size != 0) return false;
    return true;
}

//  Requests that the vector capacity be at least enough to contain 'n' elements.
//  In the case when the user calls vector_reserve(n) the target capacity is 'n'
//  itself.
void vector_reserve(vector *this, size_t n) {
    assert(this);
    // your code here
    if (n > (this -> capacity)) {
        this -> capacity = get_new_capacity(n);
        this -> array = (void **)realloc(this -> array, get_new_capacity(n) * sizeof(void *));
    }
}

void **vector_at(vector *this, size_t position) {
    assert(this);
    assert(position < (this -> size) && position >= 0);

    void ** output = this->array;
    size_t pos = position;
    while (pos > 0) {
        output ++;
        pos --;
    }
    return output;
}

void vector_set(vector *this, size_t position, void *element) {
    assert(this);
    assert(position < (this -> size));
    this -> array[position] = this -> copy_constructor(element);;
}

void *vector_get(vector *this, size_t position) {
    assert(this);
    assert(position < this->size);
    return this -> array[position];
}

void **vector_front(vector *this) {
    assert(this);
    // your code here
    assert((this -> size) > 0 && (this -> capacity) > 0);
    return this -> array;
}

/*
 * Returns a pointer to the last element in the vector.
 *
 * Calling this function on an empty container causes undefined behavior.
 */
void **vector_back(vector *this) {
    // your code here
    assert(this);
    assert((this -> size) > 0 && (this -> capacity) > 0);
    void ** ptr = (void **)this -> array;
    int cnt = (int)this -> size;
    while (cnt > 1) {
        ptr ++;
        cnt --;
    }
    return ptr;
}

void vector_push_back(vector *this, void *element) {
    assert(this);
    // your code here
    if (((this -> size) + 1) > (this -> capacity)) {
        vector_reserve(this, (this -> size) + 1);
    }
    this -> array[this -> size] = this -> copy_constructor(element);
    this -> size ++;
}

/*
 * Removes the last element in the vector, effectively reducing the container
 * size by one.
 *
 * This destroys the removed element.
 */
void vector_pop_back(vector *this) {
    assert(this);
    assert((this -> size) > 0);
    this -> destructor(*vector_back(this));
    *vector_back(this) = NULL;
    this -> size --;
}

void vector_insert(vector *this, size_t position, void *element) {
    assert(this);
    if (((this -> size) + 1) > (this -> capacity)) {
        vector_reserve(this, (this -> size) + 1);
    }

    for (size_t i = (this -> size); i > position; i --) {
        this->array[i] = this->array[i - 1];
    }

    this -> array[position] = this -> copy_constructor(element);
    this -> size ++;
}

void vector_erase(vector *this, size_t position) {
    assert(this);
    assert(position < vector_size(this));

    this -> destructor(this -> array[position]);

    for (size_t i = position; i < (this -> size) - 1; i ++) {
        this -> array[i] = this -> array[i + 1];
    }
    this -> size --;
}

void vector_clear(vector *this) {
    for (int i = 0; i < (int)this->size; i++) {
        this -> destructor(this -> array[i]);
    }
    this -> size = 0;
}
