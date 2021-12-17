//
// Created by ka-7a on 17.12.2021.
//

#ifndef COMPILER_FUNCTION_H
#define COMPILER_FUNCTION_H

/* Это файл с описанием функционала который известен системе (что делать если встретился + или cr )
*/

#include "dictionary.h"
int look_rules(struct NewRules R, int name_value);
void start();
struct Stack //структура для стека
{
    int size; // размер стека
    double* values; // сам стек
};

struct Instruct //структура для новых инструкций
{
    int i_name; //"имя" - есть в Namelist
    int i_size; //размер списка команд
    int* i_data; //список команд, вызываемых, если в коде встретилось "имя"
};
struct NewRules // здесь хранятся новые правило
{
    int amount;// их число
    struct Instruct* rules;// список инструкций
    struct Stack steck; //записали сюда стек для удобства передачи в функции поиска
};

char* str_check(string str) // проверяет является ли строка числом ("-10.5"), 1-- не число, 0 -- число
{
    char* my_str = (char*)malloc((int)str.length()+1);

    if (my_str != NULL)
    {
        for (signed int i = 0; i < (int)str.length(); i++)
        {
            my_str[i] = str[i];
            if (!isdigit(my_str[i]) && my_str[i] != '-' && my_str[i] != '.' && my_str[i] != '\n' && my_str[i] != '\0')
            {
                return (char*)"a";
            }
        }
        return my_str;
    }
    else
    {
        free(my_str);
        exit(1);
    }
}



void error_message(FILE* f, int feedback, int value) //фукнция печати обнаруженных ошибок
{
    switch (feedback)
    {
        case 1:
            fprintf(f, "Error #1%d || stack is empty or less than the required value\r\n", value);
            break;
        case -1:
            fprintf(f, "Error #-1%d || there is no character ending the command\r\n", value);
            break;
        case 2:
            fprintf(f, "Error #2%d || this word is already taken\r\n", value);
            break;
        case 3:
            fprintf(f, "Error #3%d || invalid name for the word\r\n", value);
            break;
        case 4:
            fprintf(f, "Error #4%d || unknown command\r\n", value);
            break;

        default:
            break;
    }
}

string name_table(Table n, int i) // для удобства - возвращает имя конкретной клетки из таблицы
{
    Cell tmp = n.take(i - 1);
    string ret = tmp.sym();
    return ret;
}

int printn(FILE* f) // word is cr
{
    fprintf(f, "\r\n");
    return 0;
}
int print(FILE* f, struct Stack S) // word is .
{
    if (S.size == 0) return 1;
    fprintf(f, "%lf ", S.values[S.size - 1]);
    //if (stack.size() == 0) return 0;
    //cout << stack[stack.size()] << "\n";
    return 0;
}

int print_(FILE* f, struct Files Im, int* code, int MAX, int n_start, FILE* f1) // ." ... " - печать
{
    /*FILE * f - куда печатать (display) , struct Files Im - все важные данные после обработки автомата
     int* code - код в виде потока лексем, int MAX - размер кода, int n_start - откуда читаем,
     FILE * f1 - куда выводить ошибки*/

    int i = n_start;
    string s = "";

    while ((code[i] != -6) && i < MAX) // пока не встретим " и пока не уйдем до конца
    {

        if (code[i] < 0) // it is word code[i]~ -10
        {
            Cell wi = Im.Wordlist.take(-(code[i] + 1)); //просто печатаем символы
            s = wi.sym();

        }
        else // it is name
        {
            Cell wi = Im.Namelist.take(code[i] - 1);
            s = wi.sym();
        }
        char* ss;
        ss = (char*)malloc((int)s.length() + 1);
        if (ss != NULL)
        {
            for (signed int i = 0; i < (int)s.length(); i++)
                ss[i] = s[i];
            ss[s.length()] = '\0';
            fprintf(f, "%s ", ss); //собственно напечатали посимвольно на экран полученное слово
            free(ss);
        }
        s = "";
        i++;

    }
    if (code[i] != -6) // сообщили об ошибке елси не встретили завершающего символа
    {
        error_message(f1, -1, code[n_start]);
        return n_start;
    }
    return i;
}


int plus(struct Stack *S)
{
    if (S->size < 2) return 1;
    S->values[S->size - 2] = S->values[S->size - 2] + S->values[S->size - 1];
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}
int minus(struct Stack *S)
{
    if (S->size < 2) return 1;
    S->values[S->size - 2] = S->values[S->size - 2] - S->values[S->size - 1];
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}
int mul(struct Stack *S)
{
    if (S->size < 2) return 1;
    S->values[S->size - 2] = S->values[S->size - 2] * S->values[S->size - 1];
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}
int div(struct Stack *S)
{
    if (S->size < 2) return 1;
    S->values[S->size - 2] = 1.0 * S->values[S->size - 2] / S->values[S->size - 1];
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}

int mod(struct Stack *S) //остаток от деление
{
    if (S->size < 2) return 1;
    S->values[S->size - 2] =fmod(S->values[S->size - 2], S->values[S->size - 1]);
    S->values[S->size - 1] = 0;
    S->size--;// S->values[S->size - 2] % S->values[S->size - 1]
    return 0;
}


int drop(struct Stack *S) // word is drop //удаляет элемент на вершине стека
{
    if (S->size == 0) return 1;
    S->size--;
    S->values[S->size] = 0;
    return 0;
}

int swap(struct Stack *S) // word is swap// Меняет местами два элемента на вершине стека
{
    if (S->size < 2) return 1;
    double tmp;
    tmp = S->values[S->size - 1];
    S->values[S->size - 2] = tmp;
    S->values[S->size - 1] = S->values[S->size - 2];
    return 0;
}
int dup(struct Stack *S) // word is dup// Дублирует элемент на вершине стека
{
    if (S->size == 0) return 1;
    S->size++;
    S->values[S->size - 1] = S->values[S->size - 2];
    return 0;
}
int over(struct Stack *S)//over(n2 n1) →(n1 n2 n1) Копирует предпоследний элемент на вершину стека
{
    if (S->size < 2) return 1;
    S->size++;
    S->values[S->size - 1] = S->values[S->size - 3];
    return 0;
}
int rot(struct Stack *S) //rot(n3 n2 n1) →(n1 n2 n3) Меняет местами первый и третий элемент от головы стека
{
    if (S->size < 3) return 1;
    double tmp;
    tmp = S->values[S->size - 1];
    S->values[S->size - 3] = tmp;
    S->values[S->size - 1] = S->values[S->size - 3];
    return 0;
}

int equal(struct Stack *S) // = -> кладет 1 если два последних элемента из стека равны и 0 если не равны
{
    if (S->size < 2) return 1;
    if (S->values[S->size - 2] == S->values[S->size - 1])
        S->values[S->size - 2] = 1;
    else
        S->values[S->size - 2] = 0;
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}
int notequal(struct Stack *S) // <> -> кладет 1 если два последних элемента из стека не равны и 0 если  равны
{
    if (S->size < 2) return 1;
    if (S->values[S->size - 2] != S->values[S->size - 1])
        S->values[S->size - 2] = 1;
    else
        S->values[S->size - 2] = 0;
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}
int more(struct Stack *S) // > ->  сравнивает два верхних в стеке элемента 1 - истина, 0 -ложь
{
    if (S->size < 2) return 1;
    if (S->values[S->size - 2] > S->values[S->size - 1])
        S->values[S->size - 2] = 1;
    else
        S->values[S->size - 2] = 0;
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}
int less(struct Stack *S) // < ->  сравнивает два верхних в стеке элемента 1 - истина, 0 -ложь
{
    if (S->size < 2) return 1;
    if (S->values[S->size - 2] < S->values[S->size - 1])
        S->values[S->size - 2] = 1;
    else
        S->values[S->size - 2] = 0;
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}
int zeroequal(struct Stack *S) // 0= ->  сравнивает верхний в стеке элемент с нулем 1 - истина, 0 -ложь
{
    if (S->size < 1) return 1;
    if (S->values[S->size - 1]==0)
        S->values[S->size - 1] = 1;
    else
        S->values[S->size - 1] = 0;

    return 0;
}
int zeromore(struct Stack *S) // 0> ->  сравнивает верхний в стеке элемент с нулем 1 - истина, 0 -ложь
{
    if (S->size < 1) return 1;
    if (S->values[S->size - 1] > 0)
        S->values[S->size - 1] = 1;
    else
        S->values[S->size - 1] = 0;

    return 0;
}
int zeroless(struct Stack *S) // 0< ->  сравнивает верхний в стеке элемент с нулем 1 - истина, 0 -ложь
{
    if (S->size < 1) return 1;
    if (S->values[S->size - 1] < 0)
        S->values[S->size - 1] = 1;
    else
        S->values[S->size - 1] = 0;

    return 0;
}


int F_and(struct Stack *S) // and ->  сравнивает два верхних в стеке элемента 1 - истина, 0 -ложь
{
    if (S->size < 2) return 1;
    if (S->values[S->size - 2] && S->values[S->size - 1])
        S->values[S->size - 2] = 1;
    else
        S->values[S->size - 2] = 0;
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}
int F_or(struct Stack *S) // or ->  сравнивает два верхних в стеке элемента 1 - истина, 0 -ложь
{
    if (S->size < 2) return 1;
    if (S->values[S->size - 2] || S->values[S->size - 1])
        S->values[S->size - 2] = 1;
    else
        S->values[S->size - 2] = 0;
    S->values[S->size - 1] = 0;
    S->size--;
    return 0;
}
int isdup(struct Stack *S) // word is dup// Дублирует элемент на вершине стека, если он истина
{

    if (S->size == 0) return 1;
    if (S->values[S->size - 1] != 0)
    {
        S->size++;
        S->values[S->size - 1] = S->values[S->size - 2];
        return 0;
    }
    return 0;
}

// : name .... ;
struct NewRules newinit(struct Stack S, struct NewRules R, struct Files Im, int * Code, int MAX, int n_start, FILE* er, int *index)
{
    /* struct Stack S - текущий стек, struct NewRules R - все новые правила, struct Files Im - таблицы имен,
    int * Code - код в виде потка лексем, int MAX - размер этого кода, int n_start - сдвига относительно начала, FILE* er - куда печатать ошибки,
    int *index - перекидывает по итогу на завершающий символ*/

    int i = n_start;
    int Iname = 0; // будущие поля структуры
    int Isize = 0;
    int* Itmp = (int*)malloc(sizeof(int) * max_size); //выделили изначльно много памяти
    while ((Code[i] != -28) && i < MAX) // пока не встретим ; и пока не уйдем до конца
    {
        if (i == n_start) //запишем имя правила
        {
            if (Code[i] < 0)	// Проверка на недопустимое имя
            {
                while (Code[i] != -28 && i < MAX) i++;
                error_message(er, 3, Code[n_start]);
                if (Code[i] != -28) //отправляем сообщение об ошибке если функция завершилась, не встретив ;
                    error_message(er, -1, Code[n_start]);
                *index = i;
                free(Itmp);
                return R;
            }

            Iname = Code[i]; // сохраняем "имя" инструкции (название нового правила)
            //if (Im.Namelist.search(Im.Namelist.take(Iname-1).sym()) != -1) //имя уже занято
            if (look_rules(R, Iname) != -1)
            {
                while (Code[i] != -28 && i < MAX) i++;
                error_message(er, 2, Code[n_start]); // такое правило уже есть
                if (Code[i] != -28) //отправляем сообщение об ошибке если функция завершилась, не встретив ;
                    error_message(er, -1, Code[n_start]);
                *index = i;
                free(Itmp);
                return R;
            }

        }
        else
        {
            Itmp[i - n_start - 1] = Code[i]; //переносим из кода инструкции "что делать", когда встретиться "имя"
            Isize++;
        }
        i++;
    }
    if (Code[i] != -28) //отправляем сообщение об ошибке если функция завершилась, не встретив ;
    {
        error_message(er, -1, Code[n_start]);
        *index = i;
        free(Itmp);
        return R;
    }
    int* tmp_Itmp= (int*)realloc(Itmp, sizeof(int) * Isize); //перевыделили память, так как знаем настоящую длину
    if (tmp_Itmp != NULL)
    {
        Itmp = tmp_Itmp;
    }
    else
    {
        free(Itmp);
        exit(1);
    }
    //теперь есть данные для записи в структуру
    struct Instruct I{};
    I.i_name = Iname;
    I.i_size = Isize;
    I.i_data = Itmp;
    // это одно новое правило

    //хотим положить новое правило к старым
    R.amount++;
    R.rules[R.amount - 1] = I;

    *index = i;
    return R;
}

int look_rules(struct NewRules R, int name_value) //смотрит есть ли правило с таким именем
{
    for (int i = 0;i < R.amount; i++)
    {
        if (R.rules[i].i_name == name_value)
            return i;
    }
    return -1;
}
bool stack_check(struct Stack S)
{
    if (S.values[S.size - 1])  // условие перевд if легло в стек -> смотрим истина ли это
    {
        return true;
    }
    else return false;

}
// condition if instruct else alterinstruct then OR cpndition if instruct then
struct NewRules condition(struct Stack S, struct NewRules R, struct Files Im, int* Code, int MAX, int n_start, FILE* er, int* index, int *flag)
{
    int i = n_start;
    int True_name = 0; // будущие поля структуры куда мы запишем тело if
    int True_size = 0;
    int* True_tmp = (int*)malloc(sizeof(int) * max_size);
    int j = 0;
    *flag = 0; // изначально предполагаем что условие не выполнится
    while ((Code[i] != -30 && Code[i] != -31) && i < MAX) // записываем правила пока не else or then
    {
        if(True_tmp!=NULL) True_tmp[j] = Code[i]; //переносим из кода инструкции "что делать"
        True_size++;
        i++;
        j++;
    }
    int* True_tmp2= (int*)realloc(True_tmp, sizeof(int) * True_size);
    if (True_tmp2 != NULL) True_tmp = True_tmp2;
    else
    {
        free(True_tmp2);
        exit(1);
    }
    struct Instruct I;
    if (Code[i] == -30) // в коде есть слово  else
    {
        int new_start = i; //для сдвига индекса
        int False_name = 0; // будущие поля структуры куда мы запишем тело else
        int False_size = 0;
        int* False_tmp = (int*)malloc(sizeof(int) * max_size);
        i++;
        j = 0;
        while (Code[i] != -31 && i < MAX) //  then
        {
            False_tmp[j] = Code[i]; //переносим из кода инструкции "что делать"
            j++;
            False_size++;
            i++;
        }
        if (Code[i] == -31) // есть завершающий символ (then)
        {
            int* False_tmp2 = (int*)realloc(False_tmp, sizeof(int) * False_size);
            if (False_tmp2 != NULL) False_tmp = False_tmp2;
            else
            {
                free(False_tmp2);
                exit(1);
            }
            if (stack_check(S)) // условие перевд if легло в стек -> смотрим истина ли это
            {
                I.i_data = (int*)malloc(sizeof(int) * True_size);
                I.i_name = True_name;
                I.i_size = True_size;
                for (int j = 0; j < I.i_size; j++)
                {
                    I.i_data[j] = True_tmp[j];
                }
                *index = n_start - 1;
            }
            else
            {
                I.i_data = (int*)malloc(sizeof(int) * False_size);
                I.i_name = False_name;
                I.i_size = False_size;
                for (int j = 0; j < I.i_size; j++)
                {
                    I.i_data[j] = False_tmp[j];
                }
                *index = new_start;
            }
            *flag = 1;
            R.amount++;
            R.rules[R.amount - 1] = I;
        }
        else // завершающего символа нет (then)
        {
            error_message(er, -1, Code[n_start]);
            *index = i;
        }

    }
    else // нет слова else
    {
        if (Code[i] == -31) // есть завершающий символ
        {
            if (stack_check(S))
            {
                if (True_size) // если вообще есть инструкции
                {
                    I.i_name = True_name;
                    I.i_size = True_size;
                    I.i_data = (int*)malloc(sizeof(int) * True_size);
                    for (int j = 0; j < True_size; j++)
                    {
                        I.i_data[j] = True_tmp[j];
                    }

                    R.amount++;
                    R.rules[R.amount - 1] = I;

                    *flag = 1;
                    *index = n_start-1;
                }
                else *index = i;
            }
            else *index = i;
        }
        else// мы не встретили then
        {
            error_message(er, -1, Code[n_start]);
            *index = i;
        }
    }

    //*index = i;
    return R; //имя правило будет равно 0 если нужно что-то выполнять

}

#endif //COMPILER_FUNCTION_H
