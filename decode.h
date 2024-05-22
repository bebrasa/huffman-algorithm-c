#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "structs.h"

    // Функция для преобразования символа в его двоичное представление
    // Параметры:
    //   sym: Входной символ для преобразования
    // Возвращает:
    //   Указатель на строку, представляющую двоичный код входного символа
    char *
    SymToCode(char sym)
{
    char syms[64] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ&#";
    int dec = 0;
    // Находим индекс символа в массиве символов
    while (syms[dec] != sym)
    {
        dec++;
    }

    // Выделяем память под двоичный код (6 бит) и инициализируем его '0'
    char *res = (char *)malloc(7 * sizeof(char));
    for (int i = 0; i < 6; i++)
    {
        *(res + i) = '0';
    }
    *(res + 6) = '\0';

    // Преобразуем десятичный индекс в двоичный
    int i = 5;
    while (dec > 0 && i >= 0)
    {
        *(res + i) = (dec % 2) + '0';
        i--;
        dec /= 2;
    }

    return res;
}

// Функция для получения кодов для каждого байта из входного файла
// Параметры:
//   input: Указатель на файл с кодами
// Возвращает:
//   Указатель на массив строк, представляющих коды для каждого байта
char **GetCodes(FILE *input)
{
    char **res = (char **)malloc(256 * sizeof(char *));
    for (int i = 0; i < 256; i++)
    {
        *(res + i) = NULL;
    }
    char buffer[MAX_STR_LENGTH];

    int codeSize;
    // Считываем количество кодов во входном файле
    fscanf(input, "%d\n", &codeSize);

    for (int i = 0; i < codeSize; i++)
    {
        int curByte;
        fscanf(input, "%d ", &curByte);
        fgets(buffer, MAX_STR_LENGTH, input);

        int len = strlen(buffer);
        char *code = (char *)malloc((len + 1) * sizeof(char));
        strcpy(code, buffer);
        *(code + len - 1) = '\0';
        *(res + curByte) = code;
    }

    return res;
}

// Функция для присоединения узла к дереву
// Параметры:
//   cur: Текущий узел в дереве
//   byte: Байт, который будет присоединен к узлу
//   code: Код для присоединения
//   i: Индекс в коде
void AttachNode(NODE *cur, unsigned char byte, char *code, int i)
{
    // Если код закончился, присоединяем байт к узлу
    if (*(code + i) == '\0')
    {
        cur->byte = byte;
    }
    else
    {
        // В зависимости от значения в коде, спускаемся влево или вправо
        if (*(code + i) == '0')
        {
            if (cur->left == NULL)
            {
                cur->left = CreateN(0, 0);
            }
            AttachNode(cur->left, byte, code, i + 1);
        }
        else
        {
            if (cur->right == NULL)
            {
                cur->right = CreateN(0, 0);
            }
            AttachNode(cur->right, byte, code, i + 1);
        }
    }
}

// Функция для распаковки строки символов в строку битов (0 и 1)
// Параметры:
//   str: Строка символов для распаковки
// Возвращает:
//   Указатель на распакованную строку битов
char *DecompactStr(char *str)
{
    int lenS = strlen(str);
    int lenR = lenS * 6;

    char *res = (char *)malloc((lenR + 1) * sizeof(char));
    *res = '\0';

    for (int i = 0; i < lenS; i++)
    {
        char *code = SymToCode(*(str + i));
        strcat(res, code);
        free(code);
    }

    *(res + lenR) = '\0';
    return res;
}

// Функция для декодирования строки из 0 и 1
// Параметры:
//   str: Строка из 0 и 1 для декодирования
//   first: Указатель на корень дерева кодов
//   out: Указатель на файл для записи декодированной строки
void DecodeStr(char *str, NODE *first, FILE *out)
{
    wchar_t decoded[MAX_STR_LENGTH];
    int indx = 0;
    NODE *cur = first;

    int len = strlen(str);
    int i;
    for (i = 0; i < len; i++)
    {
        if (*(str + i) == '0')
        {
            cur = cur->left;
        }
        else
        {
            cur = cur->right;
        }

        if (cur->byte != 0)
        {
            decoded[indx] = cur->byte;
            indx++;
            if (cur->byte == 10)
            {
                break;
            }
            cur = first;
        }
    }

    decoded[indx] = L'\0';
    fputws(decoded, out);
}

// Функция для декодирования данных из файла
// Параметры:
//   inputName: Имя входного файла с кодами и закодированными данными
//   output: Указатель на файл для записи декодированных данных
void Decode(char *inputName, FILE *output)
{
    FILE *input = fopen(inputName, "r");
    char buffer[MAX_STR_LENGTH];

    NODE *codeTree = CreateN(0, 0);
    char **codes = GetCodes(input);

    for (unsigned char i = 0; i < 255; i++)
    {
        if (*(codes + i) != NULL)
        {
            AttachNode(codeTree, i, *(codes + i), 0);
        }
    }

    while (fgets(buffer, MAX_STR_LENGTH, input) != NULL)
    {
        char *decomp = DecompactStr(buffer);
        DecodeStr(decomp, codeTree, output);
        free(decomp);
    }

    fclose(input);
}

#endif // DECODE_H