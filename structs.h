#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>

#define STACK struct stack
#define NODE struct node

#define MAX_STR_LENGTH 50000
#define BYTE_SIZE 8

// Структура для узла в дереве
NODE
{
    unsigned char byte; // Байт данных
    int cnt;            // Счетчик
    NODE *left;         // Левый потомок
    NODE *right;        // Правый потомок
};

// Структура для стека
STACK
{
    NODE *node;  // Указатель на узел
    STACK *next; // Указатель на следующий элемент стека
};

// Функция для создания узла
NODE *CreateN(unsigned char byte, int cnt)
{
    NODE *res = (NODE *)malloc(sizeof(NODE));

    res->byte = byte;
    res->cnt = cnt;
    res->left = NULL;
    res->right = NULL;

    return res;
}

// Функция для создания элемента стека
STACK *CreateS(NODE *node)
{
    STACK *res = (STACK *)malloc(sizeof(STACK));

    res->node = node;
    res->next = NULL;

    return res;
}

// Функция для вставки элемента в стек
void Insert(STACK *first, STACK *elem)
{
    if (first->next == NULL || first->next->node->cnt > elem->node->cnt)
    {
        elem->next = first->next;
        first->next = elem;
    }
    else
    {
        Insert(first->next, elem);
    }
}

// Функция для извлечения узла из стека
NODE *Take(STACK *first)
{
    if (first->next == NULL)
    {
        return CreateN(0, 0);
    }

    STACK *cur = first->next;
    NODE *res = cur->node;
    first->next = cur->next;
    free(cur);
    return res;
}

// Функция для объединения двух узлов в один и добавления в стек
NODE *CombineFromStack(STACK *first)
{
    NODE *n1 = Take(first);
    NODE *n2 = Take(first);

    NODE *res = CreateN(0, n1->cnt + n2->cnt);

    res->left = n1;
    res->right = n2;

    return res;
}

// Функция для безопасного добавления узла к текущему узлу
NODE *SafeAdd(NODE *cur, char bit)
{
    if (bit == '0')
    {
        if (cur->left == NULL)
        {
            NODE *res = CreateN(0, 0);
            cur->left = res;
            return res;
        }
        else
        {
            return cur->left;
        }
    }
    else
    {
        if (cur->right == NULL)
        {
            NODE *res = CreateN(0, 0);
            cur->right = res;
            return res;
        }
        else
        {
            return cur->right;
        }
    }
}

#endif