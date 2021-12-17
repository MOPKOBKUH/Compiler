/* Интерпретатор языка Форт - зачетная работа Золотухиной и Голубурдо по Теории Компиляторов 2020 год */
// Пример компиляции и запуска программы: g++ main.cpp && ./a.out

/* Язык является видоизмененным ограниченным подмножеством языка Forth.
В языке операции осуществляются с рациональными числами.
Используется постфиксная запись операторов. Все вычисления осуществляются на стеке данных.*/

/* Тезисы реализуемого языка ФОРТ (в данной версии программы)
1) Зарезервированными словами языка являются только указанные в словаре слова
2) все команды отделяются пробелами
3) регистр не важен
4) Форт интерактивен, то есть сразу начинает работать
5) Постфискная форма записи
6) Стековая нотация
7) запрещаем называть переменные/функции зарезервированными словами системы
8) вывод на экран игнорирует все встроенные команды (кроме перевода строки)
9) многострочные комменатрии - допустимы
10) ошибки выводятся в файл errors.txt, отправленные на экран результаты печатаются в файл display.txt
11) если уже было объявлено такое слово, то слово не будет перезаписано
*/

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include "function.h"
#include "tables.h"
#include "dictionary.h"

struct Stack choose(int value, struct Stack S, FILE* fee, FILE* er) // запуск неотложенных команд
{
    //все функции здесь типа int они возвращают код ошибки, если такая есть. 0 - если все хорошо
    int feedback;
    switch (value)
    {
        case -3: // word is cr
            feedback = printn(fee);
            error_message(er, feedback, value); break;
        case -4: // word is .
            feedback = print(fee, S);
            error_message(er, feedback, value); break;

        case -7: // word is +
            feedback = plus(&S);
            error_message(er, feedback, value); break;
        case -8: // word is -
            feedback = minus(&S);
            error_message(er, feedback, value); break;
        case -9: // word is /
            feedback = div(&S);
            error_message(er, feedback, value); break;
        case -10: // word is *
            feedback = mul(&S);
            error_message(er, feedback, value); break;
        case -11: // word is mod
            feedback = mod(&S);
            error_message(er, feedback, value); break;
        case -12: // word is drop
            feedback = drop(&S);
            error_message(er, feedback, value); break;
        case -13: // word is dup
            feedback = dup(&S);
        case -14: // word is over
            feedback = over(&S);
            error_message(er, feedback, value); break;
        case -15: // word is rot
            feedback = rot(&S);
            error_message(er, feedback, value); break;
        case -16: // word is swap
            feedback = swap(&S);
            error_message(er, feedback, value); break;
        case -17: // =
            feedback = equal(&S);
            error_message(er, feedback, value); break;
        case -18: // <>
            feedback = notequal(&S);
            error_message(er, feedback, value); break;
        case -19: // <
            feedback = less(&S);
            error_message(er, feedback, value); break;
        case -20: // >
            feedback = more(&S);
            error_message(er, feedback, value); break;
        case -21: // 0=
            feedback = zeroequal(&S);
            error_message(er, feedback, value); break;
        case -22: // 0<
            feedback = zeromore(&S);
            error_message(er, feedback, value); break;
        case -23: // 0=
            feedback = zeroless(&S);
            error_message(er, feedback, value); break;
        case -24: // and
            feedback = F_and(&S);
            error_message(er, feedback, value); break;
        case -25: // or
            feedback = F_or(&S);
            error_message(er, feedback, value); break;
        case -26: // ?dup
            feedback = isdup(&S);
            error_message(er, feedback, value); break;

        default:
            break;
    }
    return S;
}



struct NewRules work(int *i, struct Files Im, int* value, int valuesize, struct NewRules R, FILE* fee, FILE* errors)
{
    struct Stack S = R.steck;
    if (value[*i] < 0) //это слово зарезервированное слово
    {
        //cout << value[*i] << "\n";
        switch (value[*i]) //здесь "сложные" правила языка, то есть требующие изменений в правилах
        { //...и отложенные команды


            case -5: // from ." to " (печать конструкции в кавычках)
                *i = print_(fee, Im, value, valuesize, *i + 1, errors); //по итогу мы сдвинулись на завершующий символ "
                break;
            case -27: // from : to ; (инициализация нового слова)
                R = newinit(S, R, Im, value, valuesize, *i + 1, errors, i); // создали новое правило с которым потом будем работать
                break;
            case -29:// from if to then (include or exclude else) (условные конструкции)
                int flag;
                R = condition(S, R, Im, value, valuesize, *i + 1, errors, i, &flag);
                if (flag == 1) //ключ, что правила надо выполнить, то есть истина
                {
                    //int k = 0, * j;
                    int j = 0;
                    for (j =0; j < R.rules[R.amount - 1].i_size;j++) //тогда выполняем тот же алгоритм для списка команд из последнего запсианого правила
                    {
                        //S = choose(R.rules[rule_index].i_data[j], S, fee, errors);
                        R = work(&j, Im, R.rules[R.amount - 1].i_data, R.rules[R.amount - 1].i_size, R, fee, errors);
                        S = R.steck; //взяли стек из правил грамматик
                    }
                    flag=0;
                } //если ничего не надо выполнять, то ничего не выполняем
                break;
            case -30: // попали на слово else (особенность нашего if-а) не нужно ничего выполнять, так как сработала другая ветка уже
                // нужно промотать условия пока не встретим then
                while (value[*i] != -31) //пока не встретим then
                {
                    (*i)++; //проматываем без выполнения
                }
                break;
            default: //для остальных работает правило "моментального выполнения команды"
                S = choose(value[*i], S, fee, errors); //прописаны инструкции в этой функции
                break;

        }
    }

    else // это "имя" а не зарезривированное
    {
        std::string name = name_table(Im.Namelist, value[*i]); //для удобства записали в строку имя из таблицы
        char* tmp_str = str_check(name); // проверяем число ли наше имя
        if (tmp_str != "a") // она - число -> кладем в стек
        {
            //cout << name << "\n";
            S.size++;
            S.values[S.size - 1] = atof(tmp_str); // положили в тек
        }
        else //-- не число
        {
            //проверяем является ли слово названием правила
            int rule_index = look_rules(R, value[*i]); // =-1 если таакого правила нет, если есть, то индекс в таблице правил
            if (rule_index != -1) //такое правило есть
            {
                //cout << "rules\n";
                //int k = 0, * j;
                int j = 0;
                for (j = 0; j < R.rules[rule_index].i_size; (j)++) // выполняем соответвествующую команду
                {
                    //S = choose(R.rules[rule_index].i_data[j], S, fee, errors);
                    R = work(&j, Im, R.rules[rule_index].i_data, R.rules[rule_index].i_size, R, fee, errors);
                    S = R.steck; //взяли стек после изменений
                }
            }
            else
            {
                //cout <<"name\n";
                error_message(errors, 4, value[*i]); //неизвестная команда
            }

        }

    }
    R.steck = S;
    return R;
}

int start(string filename) //здесь происходит вся основная работа
{

    FILE* fee;
    fee = fopen("display.txt", "wa");//файл для вывод на экран
    FILE* errors;
    errors = fopen("errors.txt", "wa"); //файл для сообщения об ошибках
    struct Dictionary D = RunMachine(filename);
    // Вывод потока лексем
    /*for (int i = 0; i < D.d_sizeCode; i++) // напечатали код в виде потока лексем
    {
        cout << "  --  " << D.d_Code[i] << "  --  \n";
    }*/
    Table Wordlist = create_wordlist(); // словарь зарезервированных слов
    Table Namelist = create_namelist(D); // список имен в коде
    int CSize = D.d_sizeCode; //длина кода
    int *Code = D.d_Code; // код в виде потока лексем

    struct Files Im = { D,Wordlist, Namelist, CSize, Code }; //получаем все значения из работы автомата
    Im.D = D; //некоторые полезные вещи -> на случай необходимости возвращения к исходным данным

    Im.Wordlist.Print();
    //int feedback =0;


    int S_size = 0;
    double *S_values = (double*)malloc(sizeof(double) * max_size); //изначально большого размера
    struct Stack S={S_size, S_values}; //создаем пустой стек


    int R_amount = 0;
    int str_size = (sizeof(struct Instruct) * max_size);
    struct Instruct *R_rules= (struct Instruct*)malloc(str_size); // "пустая", но большого размера изначально
    struct Stack R_stack= S;
    struct NewRules R {R_amount, R_rules, R_stack}; // стуктура "новых" правил, то есть для "функций", "переменных" и т.д.

    Im.Namelist.Print();

    for (int i = 0; i < Im.CSize; i++) //читаем код по словам
    {
        R = work(&i, Im, Im.Code, Im.CSize, R, fee, errors); //выполняем прогон слова по правилам грамматики
        S = R.steck; //взяли стек после изменений
    }
    // печать новых правил
    /*if (R.amount) //если правила есть
    {
        cout << "\n\n NEW RULES \n";
        for (int i = 0; i < R.amount; i++) // напечатали список правил появившихся после инициализации новых переменныхк примеру : newrule 3 + . ;
        {
            if (R.rules[i].i_name ||!R.rules[i].i_name) // если в коде был if, то выполнившиеся инструкции записались сюда с именем 0, они нам здесь не нужны
            {
                cout << R.rules[i].i_name << " -- name\n";
                for (int j = 0; j < R.rules[i].i_size; j++)
                {
                    cout << R.rules[i].i_data[j] << " ";
                }
                cout << "\n\n";
            }
        }
    }*/

    cout << "\n\n STACK \n"; // и напечатали сам стек
    for (int i = 0; i < S.size; i++)
    {

        cout << S.values[i] << " ";

    }
    cout << "\n\nAnalyse -- OK\n";

    free(R.rules);

    fclose(fee);
    fclose(errors);
    return 0;
}

int main(int argc, char **argv)
{
    start(argv[1]); //запустили работу программы
    return 0;
}
