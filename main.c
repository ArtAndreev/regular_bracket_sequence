#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITIAL_BUFFER_SIZE 8

// Errors
#define SUCCESS 0
#define MEMORY_ERROR 1
#define WRONG_ARGUMENT 2

#define CHECK_INTERRUPTED 3

// Stack constants
#define STACK_SUCCESS 0
#define STACK_WRONG_ARGUMENT 4
#define STACK_MEMORY_ERROR 5

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
int Stack_top(const Stack* stack, char* symb);
int Stack_pop(Stack* stack, char* symb);
int Stack_get_size(const Stack* stack, size_t* size);
int Stack_is_empty(const Stack* stack, bool* res);
void Stack_free(Stack* stack);
int Stack_realloc_buffer(Stack* stack);

//! Reads strings from input into array. If string is empty, input ends.
int read_lines(char*** array, size_t* len, size_t* buffer_size);
//! Reads one string into variable, the string ends with a new line.
int read_string(char** string, bool* end_of_input);
int realloc_string(char** string, size_t* buffer_size);
//! Fill result array with strings that satisfy the condition of a regular
//! bracket sequence. The strings are from string array of length len.
//! Returns the number of strings or -1 if error occurs.
int get_correct_strings(const char** array, size_t len, char*** result,
                         size_t* correct_count);
//! Check one string. Returns true if it satisfies regular bracket sequence,
//! otherwise false or -1 if error occurs.
int check_string(const char* string, bool* is_correct_string);
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
        return 0; // MEMORY_ERROR
    }
    size_t len = 0;
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    int result_code = read_lines(&strings, &len, &buffer_size);
    if (result_code != SUCCESS) {
        free_string_array(strings, len);
        return 0; // result_code
    }

    char** result = malloc(sizeof(char*) * INITIAL_BUFFER_SIZE);
    if (!result) {
        printf("[error]");
        free_string_array(strings, len);
        return 0; // MEMORY_ERROR
    }

    size_t correct_count;
    result_code = get_correct_strings((const char**)strings, len, &result,
            &correct_count);
    if (result_code != SUCCESS) {
        free_string_array(strings, len);
        free(result);
        return 0; // result_code
    }

    for (size_t i = 0; i < correct_count; i++) {
        printf("%s\n", result[i]);
    }

    free_string_array(strings, len);
    free_string_array(result, correct_count);

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
    if (!stack) {
        return STACK_WRONG_ARGUMENT;
    }
    if (stack->_real_size == stack->_buffer_size) {
        int result_code = Stack_realloc_buffer(stack);
        if (result_code != STACK_SUCCESS) {
            return result_code;
        }
    }
    stack->_buffer[stack->_real_size] = symbol;
    stack->_real_size++;

    return STACK_SUCCESS;
}

int Stack_top(const Stack* stack, char* symbol) {
    if (!stack || !symbol) {
        return STACK_WRONG_ARGUMENT;
    }
    *symbol = stack->_buffer[stack->_real_size - 1];

    return STACK_SUCCESS;
}

int Stack_pop(Stack* stack, char* symbol) {
    if (!stack || !symbol) {
        return STACK_WRONG_ARGUMENT;
    }
    *symbol = stack->_buffer[stack->_real_size - 1];
    stack->_real_size--;

    return STACK_SUCCESS;
}

int Stack_get_size(const Stack* stack, size_t* size) {
    if (!stack || !size)
        return STACK_WRONG_ARGUMENT;
    *size = stack->_real_size;

    return STACK_SUCCESS;
}

int Stack_is_empty(const Stack* stack, bool* res) {
    if (!stack || !res)
        return STACK_WRONG_ARGUMENT;
    *res = stack->_real_size == 0;

    return STACK_SUCCESS;
}

void Stack_free(Stack* stack) {
    if (!stack)
        return;
    free(stack->_buffer);
    free(stack);
    stack = NULL;
}

int Stack_realloc_buffer(Stack* stack) {
    if (!stack) {
        return STACK_WRONG_ARGUMENT;
    }
    size_t new_size = stack->_buffer_size * 2;
    char* tmp = realloc(stack->_buffer, sizeof(stack->_buffer) * new_size);
    if (!tmp) {
        printf("[error]");
        return STACK_MEMORY_ERROR;
    }
    stack->_buffer = tmp;
    stack->_buffer_size = new_size;

    return STACK_SUCCESS;
}

int read_lines(char*** array, size_t* len, size_t* buffer_size) {
    if (!array || !len || !buffer_size)
        return WRONG_ARGUMENT;
    bool end_of_input = false;
    *len = 0;
    int result_code = 0;
    while (true) {
        if (*len == *buffer_size) {
            result_code = realloc_string_array(array, buffer_size);
            if (result_code != SUCCESS) {
                return result_code;
            }
        }
        result_code = read_string(&((*array)[*len]), &end_of_input);
        if (result_code != SUCCESS) {
            return result_code;
        }
        if (end_of_input) {
            (*len)++;
            break;
        }
        (*len)++;
    }

    return SUCCESS;
}

int read_string(char** string, bool* end_of_input) {
    if (!string || !end_of_input)
        return WRONG_ARGUMENT;
    *string = malloc(INITIAL_BUFFER_SIZE);
    if (!(*string)) {
        printf("[error]");
        return MEMORY_ERROR;
    }
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    for (size_t index = 0; ;) {
        if (index == buffer_size) {
            int result_code = realloc_string(string, &buffer_size);
            if (result_code != SUCCESS) {
                free(*string);
                *string = NULL;
                return result_code;
            }
        }
        int symbol = getchar();

        if (symbol == '\0') { // symbols can be after '\0', we ignore it
            continue;
        }
        if (symbol == EOF) {
            *end_of_input = true;
            (*string)[index] = '\0';
            break;
        }
        if (symbol == '\n') {
            (*string)[index] = '\0';
            break;
        }

        (*string)[index] = (char)symbol;
        index++;
    }

    return SUCCESS;
}

int realloc_string(char** string, size_t* buffer_size) {
    if (!string || !buffer_size)
        return WRONG_ARGUMENT;
    size_t new_size = *buffer_size * 2;
    char* tmp = realloc(*string, new_size);
    if (!tmp) {
        printf("[error]");
        return MEMORY_ERROR;
    }
    *string = tmp;
    *buffer_size = new_size;

    return SUCCESS;
}

int get_correct_strings(const char** array, size_t len, char*** result, size_t* count) {
    if (!array || !result || !count)
        return WRONG_ARGUMENT;
    *count = 0;
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    for (size_t i = 0; i < len; i++) {
        bool is_correct_string = false;
        int result_code = check_string(array[i], &is_correct_string);
        if (result_code != SUCCESS) {
            return result_code;
        }
        if (is_correct_string) {
            result_code = add_string(array[i], result, &buffer_size, count);
            if (result_code != SUCCESS) {
                return result_code;
            }
        }
    }

    return SUCCESS;
}

int check_string(const char* string, bool* is_correct_string) {
    if (!string || !is_correct_string)
        return WRONG_ARGUMENT;
    Stack* brackets_stack = Stack_init();
    if (!brackets_stack) {
        printf("[error]");
        return STACK_MEMORY_ERROR;
    }

    int result_code = 0;
    size_t str_len = strlen(string);
    for (size_t i = 0; i < str_len; i++) {
        char symbol = string[i];
        if (is_bracket(symbol)) {
            result_code = bracket_step(brackets_stack, symbol);
            if (result_code == CHECK_INTERRUPTED)
                break;
            if (result_code != STACK_SUCCESS) {
                Stack_free(brackets_stack);
                return result_code;
            }
        }
    }
    bool is_empty = false;
    int stack_result_code = Stack_is_empty(brackets_stack, &is_empty);
    if (stack_result_code) {
        Stack_free(brackets_stack);
        return stack_result_code;
    }

    if (is_empty && result_code == SUCCESS) {
        Stack_free(brackets_stack);
        *is_correct_string = true;
        return SUCCESS;
    }

    Stack_free(brackets_stack);

    *is_correct_string = false;

    return SUCCESS;
}

bool is_bracket(char symbol) {
    return symbol == '(' || symbol == ')' /*|| symbol == '{' ||
           symbol == '}' || symbol == '[' || symbol == ']'*/;
}

int bracket_step(Stack* brackets_stack, char symbol) {
    if (!brackets_stack)
        return WRONG_ARGUMENT;
    char last_bracket = 0;
    int result_code = 0;
    if (is_opening_bracket(symbol)) {
        result_code = Stack_push(brackets_stack, symbol);
        if (result_code != STACK_SUCCESS) {
            return result_code;
        }
    }
    else { // is closing bracket
        bool is_empty = false;
        result_code = Stack_is_empty(brackets_stack, &is_empty);
        if (result_code != STACK_SUCCESS)
            return result_code;
        if (is_empty)
            return CHECK_INTERRUPTED;
        Stack_top(brackets_stack, &last_bracket);
        if (is_one_brackets_type(last_bracket, symbol)) {
            char popped = '\0';
            Stack_pop(brackets_stack, &popped);
        }
        else
            return CHECK_INTERRUPTED;
    }

    return SUCCESS;
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
    if (!string || !array || !buffer_size || !count)
        return WRONG_ARGUMENT;
    if (*count == *buffer_size) {
        int result_code = realloc_string_array(array, buffer_size);
        if (result_code != SUCCESS) {
            return result_code;
        }
    }

    size_t full_str_len = strlen(string) + 1;
    (*array)[*count] = malloc(full_str_len);
    if (!(*array)[*count]) {
        printf("[error]");
        return MEMORY_ERROR;
    }
    memcpy((*array)[*count], string, full_str_len);
    (*count)++;

    return SUCCESS;
}


int realloc_string_array(char*** array, size_t* buffer_size) {
    if (!array || !buffer_size)
        return WRONG_ARGUMENT;
    size_t new_size = *buffer_size * 2;
    char** tmp = realloc(*array, sizeof(char*) * new_size);
    if (!tmp) {
        printf("[error]");
        return MEMORY_ERROR;
    }
    *array = tmp;
    *buffer_size = new_size;

    return SUCCESS;
}

void free_string_array(char** array, size_t len) {
    if (!array) {
        return;
    }
    for (size_t i = 0; i < len; i++) {
        if (array[i])
            free(array[i]);
    }
    free(array);
    array = NULL;
}
