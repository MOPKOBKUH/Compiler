#ifndef COMPILER_TABLES_H
#define COMPILER_TABLES_H

/* В данной программе хранится информация об основных классах, с которыми мы работаем и описания их методов
* здесь создается таблица Wordlist - список лексем и их значений (зарезервированные слова системы)
*  в функции forming входной поток данных в виде файла in.txt превращается в поток лексем
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#define max_file 100
#define max_size 50 //значения для выделения памяти для временныхх массивов (в случае неизвестного настоящего размера)

using std::string;
using std::cout;


class Cell // для записи слова и его значения
{
    string m_simb; //слово
    int m_ind; //соотвествующая лексема
public:
    Cell(string simb, int ind)
    {
        m_simb = simb;
        m_ind = ind;
    }
    Cell()
    {
        m_simb = "";
        m_ind = 0;
    }
    int index() //возвращает лексему заданной "клетки"
    {
        return m_ind;
    }
    string sym() //возвращает слово заданной "клетки"
    {
        return m_simb;
    }
    void c_sym(string sym) //меняет значение слова на sym
    {
        m_simb = sym;
    }
    void c_ind(int ind) //меняет значение лексемы на ind
    {
        m_ind = ind;
    }
    void Print() // печатает 1 клетку
    {
        cout << m_simb << "  ---   " << m_ind << "\n";
        return;
    }
};

class Table // для словарей
{
    int m_real_size; // настоящий размер массива
    Cell* m_s; // массив клеток (лексем и их значений)
    // m_ -- member

public:
    Table(int real, Cell* s1)
    {
        this->m_real_size = real;
        m_s = (Cell*)malloc(sizeof(Cell) * (m_real_size+1));
        Cell *table = m_s, *tmp = s1;
        for (int i = 0; i < m_real_size; table++, tmp++)
        {
            if (m_s!=NULL ) {
                *m_s = *s1;
            }
        }
    }
    Cell take(int i) // получаем клетку по индексу
    {
        return m_s[i];
    }
    int search(string str) // ищем есть ли в словаре такое слово str, -1 - если нет, если есть, то индекс в массиве
    {
        string tms, str2;
        for (int i = 0; i < m_real_size; i++)
        {
            tms = m_s[i].sym();

            if (tms == str)
            {
                return i;
            }
            tms = "";

        }
        return -1;
    }
    int lexemvalue(int i) // для удобства - возвращает лексему iой клетки
    {
        return m_s[i].index();
    }

    void Add(string str, int val ) // добавляет 1 новое слово с лексемой к прежним
    {
        m_real_size++;

        Cell* tmp_m_s= (Cell*)realloc(m_s, sizeof(Cell) * m_real_size);
        if (tmp_m_s != NULL)         m_s = tmp_m_s;
        else free(tmp_m_s);
        Cell s1(str, val);
        m_s[m_real_size-1] = s1;
    }
    void BigAdd(int n, string *str, int* arr) // добавляет n новых слов по массивам значений
    {
        m_real_size = m_real_size + n;;
        Cell* tmp_m_s = (Cell*)realloc(m_s, sizeof(Cell) * m_real_size);
        if (tmp_m_s != NULL)  m_s = tmp_m_s;
        else free(tmp_m_s);
        int j = 0;
        for (int i = m_real_size - n; i < m_real_size; i++)
        {
            m_s[i].c_sym(str[j]);
            m_s[i].c_ind(arr[j]);
            j++;
        }

    }
    void Drop() //удаляем 1 элемент из словаря
    {
        m_real_size--;
        Cell* tmp_m_s = (Cell*)realloc(m_s, sizeof(Cell) * m_real_size);
        if (tmp_m_s != NULL)         m_s = tmp_m_s;
        else free(tmp_m_s);
    }
    void Print() // печать словаря
    {
        for (int i = 0; i < m_real_size; i++)
        {
            m_s[i].Cell::Print();
        }
        cout<<'\n';
        return;
    }
    /*~Table()
    {
        free(m_s);
        std::cout << "Destructor -- OK\n";
    }*/
};



Table create_wordlist() //создание wordlist словаря зарезервировнных имен
{
    int const s_size = 31;  //28; -- количество зарезервированных слов
    //string s[s_size] = { "/", "{", "} ", "cr", ".", ".\"" , "\"", ":", ";", "begin", "repeat", "while", "until", "do", "loop", "if", "else", "then", "variable", "@", "! ", "value", "=", ">", "<", ">=", "<=", "not", "and", "or", "drop", "drop-all", "swap", "dup", "over", "rot", "depth", "+", "-", "/", "*", "div", "mod", "neq", "(", ")", "TRUE", "ELSE" };
    // string s[s_size] = { "{", "}", "cr", ".", ".\"", "\"", "+", "-", "/", "*", "mod", "drop","dup", "over","rot", "swap", "=", "<>", "<", ">","0=","0<","0>","and","or","?dup", ":", ";" };

    string s[s_size] = { "{", "}", "cr", ".", ".\"", "\"", "+", "-", "/", "*", "mod", "drop","dup", "over","rot", "swap", "=", "<>", "<", ">","0=","0<","0>","and","or","?dup", ":", ";" , "if", "else", "then"};
    //строка зарезерированных слов
    Cell* tmp= (Cell*)malloc(sizeof(Cell) * (s_size + 1)); // временный массив
    if (tmp != NULL)
    {

        Cell* tmpCell = tmp;

        for (int i = 0; i < s_size; i++)
        {
            tmpCell->c_sym(s[i]);
            tmpCell->c_ind(-(i + 1));
            tmpCell++;
        }

        Table table(s_size, tmp);
        free(tmp);
        return table;
    }
    else
    {
        free(tmp);
        exit(1);
    }
}

string* forming(string filename) // читамем файл и возвращает его в виде списка строк
{
    FILE* source;
    char c;

    source = fopen(filename.c_str(), "r");
    int input_size = 1;
    string* input_str = (string*)malloc(sizeof(string) * (max_file));
    string s = "";
    // cout << "Forming start OK \n";
    while ((c = fgetc(source)) != EOF && input_size<max_file)
    {
        c = tolower(c); //переводим все в нижний регистр
        if (c >= '!' && c <= '}') // c - цифра, латинская буква или арифметический символ
        {
            s = s + c; //сохраняем по символьно слово
        }
        else
        {
            if (s != "" && s != " " && s != "\n" && s != "\0") // слово завершилось и не пустое
            {
                input_size++; // увеличили количество слов
                if (input_str!=NULL) input_str[input_size - 2] = s; // положили слово в конец
                s = ""; // очистили буфер
            }
            else
                s = "";
        }
    }
    if (input_size == max_file) cout <<"Forming: input file is too long\n";
    string* tmp_input_str = (string*)realloc(input_str, sizeof(string) * (input_size + 1));
    if (tmp_input_str != NULL)
        input_str = tmp_input_str;
    else
    {
        free(tmp_input_str);
        exit(1);
    }
    input_str[input_size - 1] = "OK"; // завершающее слово
    fclose(source);
    cout << "Reading -- OK \n";
    return input_str;
}


int searching(int n, string* str, string s1) // ищем строку в массиве строк, -1 - ее нет, есть - индекс
{
    for (int i = 0; i < n; i++)
    {
        if (str[i] == s1)
        {
            return i;
        }
    }
    return -1;

}

#endif //COMPILER_TABLES_H
