#ifndef CODE_H
#define CODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>   //для широких символов (юникоды на 4 байта)
#include "structs.h" // Включаем заголовочный файл со структурами

// Функция для преобразования широкой строки в строку ASCII
unsigned char *ToString(wchar_t *str)
{
    int flag = 0;
    int len = wcslen(str);     // Получаем длину строки
    if (str[len - 1] != L'\n') // Проверяем, если последний символ не новая строка
    {
        flag = 1; // Устанавливаем флаг наличия новой строки
    }

    // Выделяем память для результирующей строки с учетом возможной новой строки
    unsigned char *res = (unsigned char *)malloc((len + 1 + flag) * sizeof(unsigned char));
    if (res == NULL)
    {
        return NULL; // Возвращаем NULL в случае ошибки выделения памяти
    }

    // Копируем символы из широкой строки в результирующую строку
    for (int i = 0; i < len; i++)
    {
        *(res + i) = (unsigned char)str[i];
    }
    // Если была новая строка в исходной строке, добавляем её в результирующую строку
    if (flag)
    {
        *(res + len) = '\n';
    }
    *(res + len + flag) = '\0'; // Завершаем строку нулевым символом
    return res;                 // Возвращаем указатель на результирующую строку
}

// Функция для подсчета частоты встречаемости символов в файле
int *GetFrequency(FILE *in)
{
    int *res = (int *)malloc(256 * sizeof(int)); // Выделяем память для массива частот
    if (res == NULL)
    {
        return NULL; // Возвращаем NULL в случае ошибки выделения памяти
    }

    wchar_t buffer[MAX_STR_LENGTH]; // Создаем буфер для чтения строк из файла

    // Инициализируем массив частот нулями
    for (int i = 0; i < 256; i++)
    {
        *(res + i) = 0;
    }

    // Считываем строки из файла и обновляем частоты символов
    while (fgetws(buffer, MAX_STR_LENGTH, in) != NULL)
    {
        unsigned char *str = ToString(buffer); // Преобразуем широкую строку в строку ASCII
        if (str == NULL)
        {
            free(res); // Освобождаем память массива частот в случае ошибки
            return NULL;
        }

        int len = strlen((char *)str); // Получаем длину строки
        // Увеличиваем частоты каждого символа в строке
        for (int i = 0; i < len; i++)
        {
            *(res + str[i]) += 1;
        }
        free(str); // Освобождаем память строки
    }

    return res; // Возвращаем указатель на массив частот
}

// Функция для создания стека узлов дерева Хаффмана на основе частот
STACK *CreateStack(FILE *in, int *codeSize)
{
    STACK *res = CreateS(NULL); // Создаем пустой стек узлов

    int *freq = GetFrequency(in);           // Получаем частоты символов из файла
    for (unsigned char i = 0; i < 255; i++) // Перебираем все возможные символы
    {
        if (*(freq + i) != 0) // Если символ встречается, то
        {
            Insert(res, CreateS(CreateN(i, *(freq + i)))); // Создаем узел и добавляем его в стек
            *codeSize += 1;                                // Увеличиваем размер алфавита
        }
    }

    free(freq); // Освобождаем память массива частот
    return res; // Возвращаем указатель на стек узлов
}

// Функция для создания дерева Хаффмана из стека узлов
NODE *CreateTree(STACK *first)
{
    // Пока в стеке остаются хотя бы два узла, комбинируем их в новый узел
    while (first->next != NULL && first->next->next != NULL)
    {
        Insert(first, CreateS(CombineFromStack(first)));
    }

    NODE *res = first->next->node; // Получаем корень дерева
    free(first->next);             // Освобождаем память вершины стека
    free(first->node);             // Освобождаем память узла
    free(first);                   // Освобождаем память стека
    return res;                    // Возвращаем указатель на корень дерева
}

// Функция для добавления бита 0 к строке, чтобы спускаться по дереву
char *AddBit(char *str, char bit)
{
    int len = strlen(str);                                // Получаем длину строки
    char *res = (char *)malloc((len + 2) * sizeof(char)); // Выделяем память для новой строки

    // Копируем символы из исходной строки
    for (int i = 0; i < len; i++)
    {
        *(res + i) = *(str + i);
    }
    *(res + len) = bit;      // Добавляем новый бит
    *(res + len + 1) = '\0'; // Завершаем строку нулевым символом

    return res; // Возвращаем указатель на новую строку
}

// Функция для установки кодов символов в дереве Хаффмана
void SetCode(NODE *cur, char *code, char **codes)
{
    if (cur->byte != 0) // Если узел является листом дерева
    {
        *(codes + cur->byte) = code; // Устанавливаем код символа
    }
    else
    {
        // Рекурсивно обходим левое и правое поддеревья
        SetCode(cur->left, AddBit(code, '0'), codes);
        SetCode(cur->right, AddBit(code, '1'), codes);
    }
}

// Функция для кодирования строки с использованием кодов символов
char *CodeStr(unsigned char *str, char **codes)
{
    char *res = (char *)malloc(MAX_STR_LENGTH * sizeof(char)); // Выделяем память для результирующей строки
    *res = '\0';                                               // Обнуляем результирующую строку

    int len = strlen((char *)str); // Получаем длину строки
    // Заменяем каждый символ входной строки на его код
    for (int i = 0; i < len; i++)
    {
        strcat(res, *(codes + (*(str + i)))); // Конкатенируем код символа к результирующей строке
    }

    return res; // Возвращаем указатель на результирующую строку
}

// Функция для сжатя строки бит
char *CompactStr(char *str)
{
    char syms[64] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ&#"; // Массив символов для сжатия
    int lenS = strlen(str);                                                             // Получаем длину исходной строки

    int lenR = lenS + 6 - (lenS % 6);                       // Вычисляем длину результирующей строки
    char *nStr = (char *)malloc((lenR + 1) * sizeof(char)); // Выделяем память для новой строки
    nStr[0] = '\0';                                         // Обнуляем новую строку

    strcat(nStr, str);                // Копируем исходную строку в новую строку
    for (int i = lenS; i < lenR; i++) // Заполняем оставшиеся позиции нулями
    {
        *(nStr + i) = '0';
    }
    *(nStr + lenR) = '\0'; // Завершаем строку нулевым символом

    int numGroups = lenR / 6;                                   // Вычисляем количество групп
    char *res = (char *)malloc((numGroups + 1) * sizeof(char)); // Выделяем память для результирующей строки

    // Каждые 6 битов кодируем в один символ из массива syms
    for (int i = 0; i < numGroups; i++)
    {
        char group[7];
        strncpy(group, nStr + i * 6, 6);  // Копируем 6 бит из строки
        group[6] = '\0';                  // Завершаем строку нулевым символом
        int dec = strtol(group, NULL, 2); // Преобразуем строку в число
        res[i] = syms[dec];               // Записываем символ в результирующую строку
    }
    *(res + numGroups) = '\0'; // Завершаем строку нулевым символом

    free(nStr); // Освобождаем память новой строки
    return res; // Возвращаем указатель на результирующую строку
}

// Основная функция для кодирования файла
void Code(char *inputName, FILE *output)
{
    int codeSize = 0; // Переменная для хранения размера алфавита

    FILE *input = fopen(inputName, "r"); // Открываем входной файл для чтения
    if (input == NULL)                   // Проверяем, успешно ли открыт файл
    {
        printf("Error opening input file.\n"); // Выводим сообщение об ошибке
        return;
    }

    STACK *codeStack = CreateStack(input, &codeSize); // Создаем стек узлов дерева Хаффмана
    if (codeStack == NULL || codeStack->next == NULL) // Проверяем, успешно ли создан стек
    {
        if (codeStack != NULL)
            free(codeStack); // Освобождаем память стека в случае ошибки
        fclose(input);       // Закрываем файл
        return;
    }
    NODE *codeTree = CreateTree(codeStack); // Создаем дерево Хаффмана из стека узлов

    char **codes = (char **)malloc(256 * sizeof(char *)); // Выделяем память для массива кодов символов
    for (int i = 0; i < 256; i++)
    {
        *(codes + i) = NULL; // Инициализируем массив NULL-ами
    }

    char *code = (char *)malloc(1 * sizeof(char)); // Выделяем память для временной строки
    *code = '\0';                                  // Обнуляем временную строку
    SetCode(codeTree, code, codes);                // Устанавливаем коды символов

    fprintf(output, "%d\n", codeSize); // Выводим размер алфавита в выходной файл
    for (int i = 0; i < 256; i++)      // Перебираем все возможные символы
    {
        // Если символ имеет код, выводим его в файл
        if (*(codes + i) != NULL)
        {
            fprintf(output, "%d ", i);   // Выводим код символа
            fputs(*(codes + i), output); // Выводим код в файл
            fprintf(output, "\n");       // Переходим на новую строку
        }
    }

    fseek(input, 0, SEEK_SET); // Возвращаем указатель на начало файла

    wchar_t buffer[MAX_STR_LENGTH];                       // Буфер для чтения строк из файла
    while (fgetws(buffer, MAX_STR_LENGTH, input) != NULL) // Читаем строки из файла
    {
        unsigned char *str = ToString(buffer); // Преобразуем широкую строку в строку ASCII
        char *coded = CodeStr(str, codes);     // Кодируем строку
        char *compacted = CompactStr(coded);   // Компактируем строку
        fputs(compacted, output);              // Выводим компактированную строку в файл
        fprintf(output, "\n");                 // Переходим на новую строку
        free(str);                             // Освобождаем память строки
        free(coded);                           // Освобождаем память закодированной строки
        free(compacted);                       // Освобождаем память компактированной строки
    }

    fclose(input); // Закрываем входной файл
}

#endif // CODE_H
