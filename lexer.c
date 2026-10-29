/*
# Project Description: Lexical Analyzer in C

Project Title: Lexical Analyzer
Author: Sanjay Hibare
Date: 20 October 2026

## Description
The Lexical Analyzer is a C-based project that reads a source code file and identifies different types of tokens. It is the first phase of a compiler, where the input program is divided into meaningful units called tokens.
The project identifies keywords, identifiers, constants, operators, delimiters, and comments from the given C source code. It uses basic C programming concepts such as functions, arrays, loops, conditional statements, string handling, and file handling.
The program reads the input file character by character, groups characters into lexemes, and classifies them according to their token types. Finally, it displays the identified tokens in a readable format.

## Objectives
- To understand the working of a lexical analyzer.
- To learn how source code is divided into tokens.
- To identify keywords, identifiers, constants, operators, and delimiters.
- To practice file handling and string manipulation in C.
- To understand the basic working of the first phase of a compiler.

## Conclusion
This project provides a basic understanding of lexical analysis and compiler fundamentals. 
It improves practical knowledge of C programming, file handling, and token identification.
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define SIZE 100

/* C keywords used by the lexer */
const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "int", "long", "register", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while"
};

#define KEYWORD_COUNT (sizeof(keywords) / sizeof(keywords[0]))

/* Check whether a word is a C keyword. */
int isKeyword(const char word[])
{
    size_t i;
    for (i = 0; i < KEYWORD_COUNT; i++) {
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

/* Print one token in a simple table. */
void printToken(const char lexeme[], const char type[])
{
    printf("%-20s %s\n", lexeme, type);
}

/* Read and print a word: keyword or identifier. */
void readWord(FILE *fp, int firstChar)
{
    char word[SIZE];
    int i = 0, ch;

    word[i++] = (char)firstChar;
    while ((ch = fgetc(fp)) != EOF && (isalnum((unsigned char)ch) || ch == '_')) {
        if (i < SIZE - 1)
            word[i++] = (char)ch;
    }
    word[i] = '\0';

    if (ch != EOF)
        ungetc(ch, fp);  /* Put back the character that is not part of the word. */

    if (isKeyword(word))
        printToken(word, "KEYWORD");
    else
        printToken(word, "IDENTIFIER");
}

/* Read and print an integer or a simple decimal number. */
void readNumber(FILE *fp, int firstChar)
{
    char number[SIZE];
    int i = 0, ch, isFloat = 0;

    number[i++] = (char)firstChar;
    while ((ch = fgetc(fp)) != EOF) {
        if (isdigit((unsigned char)ch)) {
            if (i < SIZE - 1) number[i++] = (char)ch;
        } else if (ch == '.' && !isFloat) {
            isFloat = 1;
            if (i < SIZE - 1) number[i++] = (char)ch;
        } else {
            ungetc(ch, fp);
            break;
        }
    }
    number[i] = '\0';
    printToken(number, isFloat ? "FLOAT CONSTANT" : "INTEGER CONSTANT");
}

/* Read a string or character constant, including its quotes. */
void readQuoted(FILE *fp, int quote)
{
    char value[SIZE];
    int i = 0, ch;
    value[i++] = (char)quote;

    while ((ch = fgetc(fp)) != EOF && ch != '\n') {
        if (i < SIZE - 1) value[i++] = (char)ch;
        if (ch == '\\') { /* An escaped quote does not end the string. */
            ch = fgetc(fp);
            if (ch == EOF) break;
            if (i < SIZE - 1) value[i++] = (char)ch;
        } else if (ch == quote) {
            break;
        }
    }
    value[i] = '\0';

    if (quote == '"')
        printToken(value, "STRING LITERAL");
    else
        printToken(value, "CHARACTER CONSTANT");
}

/* Handle comments and the division operator. */
void readSlash(FILE *fp)
{
    int ch = fgetc(fp);
    char comment[SIZE];
    int i = 0;

    if (ch == '/') {
        comment[i++] = '/'; comment[i++] = '/';
        while ((ch = fgetc(fp)) != EOF && ch != '\n') {
            if (i < SIZE - 1) comment[i++] = (char)ch;
        }
        comment[i] = '\0';
        printToken(comment, "COMMENT");
    } else if (ch == '*') {
        comment[i++] = '/'; comment[i++] = '*';
        while ((ch = fgetc(fp)) != EOF) {
            if (i < SIZE - 1) comment[i++] = (char)ch;
            if (ch == '/') {
                /* The previous character is checked below for the closing */
                if (i >= 2 && comment[i - 2] == '*') break;
            }
        }
        comment[i] = '\0';
        printToken(comment, "COMMENT");
    } else {
        if (ch != EOF) ungetc(ch, fp);
        printToken("/", "OPERATOR");
    }
}

/* Read the input file and classify each character/lexeme. */
void analyzeFile(FILE *fp)
{
    int ch, next;

    printf("\n%-20s %s\n", "LEXEME", "TOKEN TYPE");
    printf("----------------------------------------\n");

    while ((ch = fgetc(fp)) != EOF) {
        if (isspace((unsigned char)ch))
            continue;

        if (isalpha((unsigned char)ch) || ch == '_') {
            readWord(fp, ch);
        } else if (isdigit((unsigned char)ch)) {
            readNumber(fp, ch);
        } else if (ch == '"' || ch == '\'') {
            readQuoted(fp, ch);
        } else if (ch == '/') {
            readSlash(fp);
        } else if (strchr("+-*%=<>!&|^", ch) != NULL) {
            char op[3] = {(char)ch, '\0', '\0'};
            next = fgetc(fp);
            if (next == '=' ||
                ((ch == '+' || ch == '-') && next == ch) ||
                ((ch == '&' || ch == '|') && next == ch)) {
                op[1] = (char)next;
            } else if (next != EOF) {
                ungetc(next, fp);
            }
            printToken(op, "OPERATOR");
        } else if (strchr("(){}[];,.", ch) != NULL) {
            char symbol[2] = {(char)ch, '\0'};
            printToken(symbol, "SPECIAL CHARACTER");
        } else if (ch == '#') {
            char directive[SIZE];
            int i = 0;
            directive[i++] = '#';
            while ((next = fgetc(fp)) != EOF && next != '\n') {
                if (i < SIZE - 1) directive[i++] = (char)next;
            }
            directive[i] = '\0';
            printToken(directive, "PREPROCESSOR DIRECTIVE");
        } else {
            char unknown[2] = {(char)ch, '\0'};
            printToken(unknown, "UNKNOWN CHARACTER");
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *fp;

    if (argc != 2) {
        printf("Usage: %s source_file.c\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Could not open input file");
        return 1;
    }

    printf("C LEXICAL ANALYZER\n");
    printf("Input file: %s\n", argv[1]);
    analyzeFile(fp);

    fclose(fp);
    return 0;
}
