#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED,
}MetaCommandResult;

typedef enum{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATMENT,
} PrepareResult;

typedef enum{
    STATMENT_INSERT,
    STATMENT_SELECT,
}StatementType;

typedef struct{
    StatementType type ;
}Statment;

InputBuffer *new_input_buffer(){
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt(){ 
    printf("Your Personal DB >"); 
}



void close_input_buffer(InputBuffer *input_buffer){
    free(input_buffer->buffer);
    free(input_buffer);
}
void read_input(InputBuffer *input_buffer)
{
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read <= 0)
    {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}
MetaCommandResult do_meta_command(InputBuffer *input_buffer){
    if (strcmp(input_buffer->buffer,".exit") == 0){
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    }else {
        return META_COMMAND_UNRECOGNIZED;
    }
}

PrepareResult prepare_statement(InputBuffer *input_buffer, Statment *statement){
    if (strcmp(input_buffer->buffer,"insert") == 0){
        statement->type = STATMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    else if (strcmp(input_buffer->buffer, "select") == 0){
        statement->type = STATMENT_SELECT;
        return PREPARE_SUCCESS;
    }else {
        return PREPARE_UNRECOGNIZED_STATMENT;
    }
}

void execute_statement(Statment *statement){
    switch (statement->type){
    case STATMENT_INSERT:
        printf("it is a insert statement yaahhhh!!\n");
        break;

    case STATMENT_SELECT:
        printf("it is a select statement yaahhhh!!\n");
        break;
    }
}

int main(int argc, char *argv[]){
    InputBuffer *input_buffer = new_input_buffer();
    while (true){
        print_prompt();
        read_input(input_buffer);
        if (input_buffer->buffer[0] =='.'){
            switch (do_meta_command(input_buffer)){
                case (META_COMMAND_SUCCESS) :
                    continue;
                case (META_COMMAND_UNRECOGNIZED):
                    printf("this command is not recognize!!!!! check what you typed???\n");
                    continue;
            }
        }
        Statment statement ;
        switch (prepare_statement(input_buffer,&statement)){
            case (PREPARE_SUCCESS):
                break;

            case(PREPARE_UNRECOGNIZED_STATMENT) : 
                printf("unrecognize the keyword that you typed '%s'.\n",input_buffer->buffer);
                continue;
        }
        execute_statement(&statement);
        printf("done.. executed!!\n");

    }
}