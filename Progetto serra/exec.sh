#!/bin/bash

bison -d  parser.y
flex lexer.l
gcc -o serra *.c -ll