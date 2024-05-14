#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>



#define CULOUMN_USERNAME_SIZE 32
#define CULOUMN_EMAIL_SIZE 255
#define clac_username_offset(ID_OFFSET, ID_SIZE) (ID_OFFSET + ID_SIZE)
#define clac_email_offset(USERNAME_OFFSET, USERNAME_SIZE) (USERNAME_OFFSET + USERNAME_SIZE)
#define clac_total_row_size(ID_SIZE, USERNAME_SIZE, EMAIL_SIZE) (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)
#define clac_rows_per_page(MAX_PAGE, row_size) (MAX_PAGE / row_size)
#define clac_table_max_rows(MAX_TABLE_SIZE, rows_per_page) (MAX_TABLE_SIZE * rows_per_page)
#define MAX_TABLE_PAGES 100
typedef struct{
    uint32_t id;
    char username[CULOUMN_USERNAME_SIZE];
    char email[CULOUMN_EMAIL_SIZE];
} Row;


const uint32_t ID_SIZE = sizeof(((Row *)0)->id);
const uint32_t USERNAME_SIZE = sizeof(((Row *)0)->username);
const uint32_t EMAIL_SIZE = sizeof(((Row *)0)->email);

const uint32_t ID_OFFSET = 0;

const uint32_t MAX_PAGE = 4096;



// size_of_attribute it's a macro that takes a struct type and an atteribute inside that struct 
// and calculate the size of that attribute without allocationg a memory using null pointer


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
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR,
}PrepareResult;


typedef enum {
    EXECUTE_SECCUESS,
    EXECUTE_TABLE_FULL,
}ExecuteResults;

typedef enum{
    STATEMENT_INSERT,
    STATEMENT_SELECT,
}StatementType;

typedef struct{
    StatementType type ;
    Row row_to_insert;
}Statement;

typedef struct{
    void *pages[MAX_TABLE_PAGES];
    uint32_t num_rows;
}Table;


InputBuffer *new_input_buffer(){
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void* row_slot(Table *table , uint32_t row_num){
    uint32_t page_num = row_num / clac_rows_per_page(MAX_PAGE,clac_total_row_size(ID_SIZE,USERNAME_SIZE,EMAIL_SIZE));
    void *page = table->pages[page_num];
    if (!page){
        page = table->pages[page_num] = malloc(MAX_PAGE);
    }
    uint32_t row_offset = row_num % clac_rows_per_page(MAX_PAGE,clac_total_row_size(ID_SIZE,USERNAME_SIZE,EMAIL_SIZE));
    uint32_t byte_offset = row_offset * clac_total_row_size(ID_SIZE, USERNAME_SIZE, EMAIL_SIZE);
    return page + byte_offset;
}

Table* new_table(){
    Table* table = (Table*)malloc(sizeof(Table));
    table->num_rows = 0;
    for (uint32_t i =0; i< MAX_TABLE_PAGES ; i++){
        table->pages[i] = NULL;
    }
    return table;
}





void free_table(Table* table){
    for (int i =0;i<=MAX_TABLE_PAGES;i++){
        free(table->pages[i]);
    }
    free(table);
}



void print_prompt(){ 
    printf("Your Personal DB >"); 
}

void serialize_row(Row *source, void *destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + clac_username_offset(ID_OFFSET, ID_SIZE), &(source->username), USERNAME_SIZE);
    memcpy(destination + clac_email_offset(clac_username_offset(ID_OFFSET, ID_SIZE), USERNAME_SIZE), &(source->email), EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destiation)
{
    memcpy(&(destiation->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destiation->username), source + clac_username_offset(ID_OFFSET, ID_SIZE), USERNAME_SIZE);
    memcpy(&(destiation->email), source + clac_email_offset(clac_username_offset(ID_OFFSET, ID_SIZE), USERNAME_SIZE), EMAIL_SIZE);
}

void close_input_buffer(InputBuffer *input_buffer){
    free(input_buffer->buffer);
    free(input_buffer);
}

void read_input(InputBuffer *input_buffer){
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read <= 0){
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
void print_row(Row *row){
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

PrepareResult prepare_statement(InputBuffer *input_buffer,Statement *statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(
            input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id),
            statement->row_to_insert.username, statement->row_to_insert.email);
        if (args_assigned < 3)
        {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResults execute_insert(Statement *statement, Table *table){
    if (table->num_rows >= MAX_TABLE_PAGES){
        return EXECUTE_TABLE_FULL;
    }
    Row* row_to_insert = &(statement->row_to_insert);
    table->num_rows += 1 ;
    serialize_row(row_to_insert , row_slot(table,table->num_rows));
    return EXECUTE_SECCUESS;

}
ExecuteResults execute_select(Table *table){
    Row row;
    for (uint32_t i = 0;i<=table->num_rows;i++){
        deserialize_row(row_slot(table,i),&row);
        print_row(&row);
    }
}

ExecuteResults execute_statement(Statement *statement , Table* table){
    switch (statement->type){
    case STATEMENT_INSERT:
        execute_insert(statement, table);
        return EXECUTE_SECCUESS;

    case STATEMENT_SELECT:
        execute_select(table);
        return EXECUTE_SECCUESS;
    }
}

int main(int argc, char *argv[]){
    Table* tabel = new_table();
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
        Statement statement ;
        switch (prepare_statement(input_buffer,&statement)){
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_SYNTAX_ERROR):
                printf("syntax error, foucs on your typos or go to sleep" );
            case(PREPARE_UNRECOGNIZED_STATEMENT) : 
                printf("unrecognize the keyword that you typed '%s'.\n",input_buffer->buffer);
                continue;
        }
        switch (execute_statement(&statement, tabel)){
        case EXECUTE_SECCUESS:
            printf("shatora :} \n"); 
            break;

        case EXECUTE_TABLE_FULL:
            printf("etmalit 5las :/ \n");
            break;
        }
    }
}