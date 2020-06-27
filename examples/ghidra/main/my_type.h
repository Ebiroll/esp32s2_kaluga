#pragma once


typedef struct my_type
{
    char         my_char;
    int          my_int; 
    char         my_string[8];
} my_type;


int one_arg(int my_arg);

void two_args(int my_arg,my_type *t);

int three_args(int arg1,int arg2,int arg3);
