#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITIAL_BUFFER_SIZE 8
#define CHECK_INTERRUPTED 2

typedef struct Stack {
//    private part
    char* _buffer;
    size_t _buffer_size;
    size_t _real_size;
} Stack;

//! Stack methods
//! Stack_init returns NULL if memory error occurs.
//! Stack_realloc_buffer returns EXIT_FAILURE if memory error occurs.
Stack* Stack_init();
int Stack_push(Stack* stack, char symbol);
char Stack_top(const Stack* stack);
void Stack_pop(Stack* stack);
size_t Stack_get_size(const Stack* stack);
bool Stack_is_empty(const Stack* stack);
void Stack_free(Stack* stack);
int Stack_realloc_buffer(Stack* stack);

//! Reads strings from input into array. If string is empty, input ends.
int read_lines(char*** array, size_t* len, size_t* buffer_size);
//! Reads one string into variable, the string ends with a new line.
int read_string(char** string);
int realloc_string(char** string, size_t* buffer_size);
//! Fill result array with strings that satisfy the condition of a regular
//! bracket sequence. The strings are from string array of length len.
//! Returns the number of strings or -1 if error occurs.
long get_correct_strings(const char** array, size_t len, char*** result);
//! Check one string. Returns true if it satisfies regular bracket sequence,
//! otherwise false or -1 if error occurs.
int check_string(const char* string);
//! The symbol is bracket '(' or ').
bool is_bracket(char symbol);
//! Check brackets from the stack.
int bracket_step(Stack* brackets_stack, char symbol);
//! The symbol is opening bracket '('.
bool is_opening_bracket(char symbol);
//! The symbols are pair ( '(' and ')' ).
bool is_one_brackets_type(char first, char second);
//! Adds string (copies it) to array.
int add_string(const char* string, char*** array, size_t* buffer_size,
               size_t* count);
int realloc_string_array(char*** array, size_t* buffer_size);
void free_string_array(char** array, size_t len);

int main() {
    char** strings = malloc(sizeof(char*) * INITIAL_BUFFER_SIZE);
    if (!strings) {
        printf("[error]");
        return 0; // EXIT_FAILURE
    }
    size_t len = 0;
    size_t buffer_size = INITIAL_BUFFER_SIZE;

    if (read_lines(&strings, &len, &buffer_size) == EXIT_FAILURE) {
        free_string_array(strings, len);
        return 0; // EXIT_FAILURE
    }

    char** result = malloc(sizeof(char*) * INITIAL_BUFFER_SIZE);
    if (!result) {
        printf("[error]");
        free_string_array(strings, len);
        return 0; // EXIT_FAILURE
    }
    long res_count = get_correct_strings((const char**)strings, len, &result);
    if (res_count == -1) {
        free_string_array(strings, len);
        free(result);
        return 0; // EXIT_FAILURE
    }

    for (size_t i = 0; i < res_count; i++) {
        printf("%s\n", result[i]);
    }

    free_string_array(strings, len);
    free_string_array(result, res_count);

    return 0;
}

Stack* Stack_init() {
    Stack* stack = malloc(sizeof(Stack));
    if (!stack) {
        return NULL;
    }

    stack->_buffer = malloc(INITIAL_BUFFER_SIZE);
    if (!stack->_buffer) {
        free(stack);
        return NULL;
    }

    stack->_buffer_size = INITIAL_BUFFER_SIZE;
    stack->_real_size = 0;

    return stack;
}

int Stack_push(Stack* stack, char symbol) {
    if (stack->_real_size == stack->_buffer_size) {
        if (Stack_realloc_buffer(stack) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }
    stack->_buffer[stack->_real_size] = symbol;
    stack->_real_size++;

    return EXIT_SUCCESS;
}

char Stack_top(const Stack* stack) {
    return stack->_buffer[stack->_real_size - 1];
}

void Stack_pop(Stack* stack) {
    stack->_real_size--;
}

size_t Stack_get_size(const Stack* stack) {
    return stack->_real_size;
}

bool Stack_is_empty(const Stack* stack) {
    return stack->_real_size == 0;
}

void Stack_free(Stack* stack) {
    free(stack->_buffer);
    free(stack);
}

int Stack_realloc_buffer(Stack* stack) {
    size_t new_size = stack->_buffer_size * 2;
    char* tmp = realloc(stack->_buffer, sizeof(stack->_buffer) * new_size);
    if (!tmp) {
        printf("[error]");
        return EXIT_FAILURE;
    }
    stack->_buffer = tmp;
    stack->_buffer_size = new_size;

    return EXIT_SUCCESS;
}

int read_lines(char*** array, size_t* len, size_t* buffer_size) {
    for (*len = 0; ; (*len)++) {
        if (*len == *buffer_size) {
            if (realloc_string_array(array, buffer_size) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }
        }
        if (read_string(&((*array)[*len])) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }

        if (!strcmp((*array)[*len], "")) {
            free((*array)[*len]);
            break;
        }
    }

    return EXIT_SUCCESS;
}

int read_string(char** string) {
    *string = malloc(INITIAL_BUFFER_SIZE);
    if (!(*string)) {
        printf("[error]");
        return EXIT_FAILURE;
    }
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    for (size_t index = 0; ; index++) {
        if (index == buffer_size) {
            if (realloc_string(string, &buffer_size) == EXIT_FAILURE) {
                free(*string);
                *string = NULL;
                return EXIT_FAILURE;
            }
        }
        (*string)[index] = (char)getchar();
        if ((*string)[index] == '\n') {
            (*string)[index] = '\0';
            break;
        }
    }

    return EXIT_SUCCESS;
}

int realloc_string(char** string, size_t* buffer_size) {
    size_t new_size = *buffer_size * 2;
    char* tmp = realloc(*string, new_size);
    if (!tmp) {
        printf("[error]");
        return EXIT_FAILURE;
    }
    *string = tmp;
    *buffer_size = new_size;

    return EXIT_SUCCESS;
}

long get_correct_strings(const char** array, size_t len, char*** result) {
    size_t count = 0;
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    for (size_t i = 0; i < len; i++) {
        int is_correct = check_string(array[i]);
        if (is_correct == -1) {
            return -1;
        }
        if (is_correct == true) {
            if (add_string(array[i], result, &buffer_size, &count)
                == EXIT_FAILURE) {
                return -1;
            }
        }
    }

    return count;
}

int check_string(const char* string) {
    Stack* brackets_stack = Stack_init();
    if (!brackets_stack) {
        printf("[error]");
        return -1;
    }

    int result_code = 0;
    size_t str_len = strlen(string);
    for (size_t i = 0; i < str_len; i++) {
        char symbol = string[i];
        if (is_bracket(symbol)) {
            result_code = bracket_step(brackets_stack, symbol);
            if (result_code == CHECK_INTERRUPTED)
                break;
            if (result_code == EXIT_FAILURE) {
                Stack_free(brackets_stack);
                return -1;
            }
        }
    }

    if (Stack_is_empty(brackets_stack) && result_code == EXIT_SUCCESS) {
        Stack_free(brackets_stack);
        return true;
    }

    Stack_free(brackets_stack);

    return false;
}

bool is_bracket(char symbol) {
    return symbol == '(' || symbol == ')' /*|| symbol == '{' ||
           symbol == '}' || symbol == '[' || symbol == ']'*/;
}

int bracket_step(Stack* brackets_stack, char symbol) {
    char last_bracket = 0;
    if (is_opening_bracket(symbol)) {
        if (Stack_push(brackets_stack, symbol) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }
    else { // is closing bracket
        if (Stack_is_empty(brackets_stack))
            return CHECK_INTERRUPTED;
        last_bracket = Stack_top(brackets_stack);
        if (is_one_brackets_type(last_bracket, symbol)) {
            Stack_pop(brackets_stack);
        }
        else
            return CHECK_INTERRUPTED;
    }

    return EXIT_SUCCESS;
}

bool is_opening_bracket(char symbol) {
    return symbol == '(' /*|| symbol == '{' || symbol == '['*/;
}

bool is_one_brackets_type(char first, char second) {
//    switch (first) {
//        case '(': return second == ')';
//        case '{': return second == '}';
//        case '[': return second == ']';
//    }
    return first == '(' && second == ')';
}

int add_string(const char* string, char*** array, size_t* buffer_size,
               size_t* count) {
    if (*count == *buffer_size) {
        if (realloc_string_array(array, buffer_size) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }

    size_t full_str_len = strlen(string) + 1;
    (*array)[*count] = malloc(full_str_len);
    if (!(*array)[*count]) {
        printf("[error]");
        return EXIT_FAILURE;
    }
    memcpy((*array)[*count], string, full_str_len);
    (*count)++;

    return EXIT_SUCCESS;
}


int realloc_string_array(char*** array, size_t* buffer_size) {
    size_t new_size = *buffer_size * 2;
    char** tmp = realloc(*array, sizeof(char*) * new_size);
    if (!tmp) {
        printf("[error]");
        return EXIT_FAILURE;
    }
    *array = tmp;
    *buffer_size = new_size;

    return EXIT_SUCCESS;
}

void free_string_array(char** array, size_t len) {
    if (!array) {
        return;
    }
    for (size_t i = 0; i < len; i++) {
        free(array[i]);
    }
    free(array);
}
