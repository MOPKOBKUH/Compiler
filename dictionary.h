//
// Created by ka-7a on 17.12.2021.
//

#ifndef COMPILER_DICTIONARY_H
#define COMPILER_DICTIONARY_H

/*Это конечный автомат - СКАНЕР.
Цель: обработка и анализ входного поток лексем (записан в виде string* Code размера CodeSize )
из этого файла дальше используется структура struct Files в которой хранятся наши словари
Хотим по итоге иметь: словарь имен (переменные и названия функций - незарезервированных слов)
автомат запускается функции RunMachine
*/

#include "tables.h"


using std::string;
using std::cout;

struct Dictionary //временная структура для записи словаря
{
    int d_header; // позволяет определить сдвиг относительно начала
    string* d_Input; // входной поток данных
    int d_sizeNames; // количество имен, найденных в коде программы
    string* d_Names; // сами имена
    int d_sizeCode; // размер осмысленного кода
    int* d_Code; //код в виде потока лексем
};

// структура, описывающая результат выполениия состояния, а именно:
struct ReturnType
{
    struct ReturnType(*ResultState)(struct Dictionary); //  это - указатель на функцию. Указывает на следующее состояние, что следует выполнить машине.
    struct Dictionary Obj; // информационная часть, которую мы обрабатываем и возвращаем измененную, если состояние подразумевало изменение данных
    //int MachineStep; // Шаг головки: вправо, влево, остались на месте. +1 -> Right, 0 - stay
    //char StateName[40];
};
struct Machine // структура, отвечающая за сам автомат
{
    struct ReturnType(*MachineState)(struct Dictionary); // это указатель на функцию. Описывает текущее состояние машины.
    int HeaderPos; // координата головки (была нужна для удосбства вывода)
    //char MachineStateName[40]; // название состояния для удобства вывода на экран
};
Table Wordlist = create_wordlist(); //создали таблицу зарезервированных слов благодаря функции из файла table.cpp

//несколько прототипов функций - состояний автомата
struct ReturnType Q_First(struct Dictionary D);
struct ReturnType Q_ItisWord(struct Dictionary D);

struct ReturnType Q_End(struct Dictionary D) // состояние, завершающее работу автомата
{
    //cout << "Q_End is ok\n";
    struct ReturnType temp = { NULL, D };
    return temp;
}
struct ReturnType Q_ConditionMoving(struct Dictionary D) // двухтактовая проверка что мы не ушли до конца
{
    int i = D.d_header; // сдвиг относительно входного потока
    string slovo = D.d_Input[i]; // само слово

    if (slovo == "OK") //искусственно созданное слово которым завершается программа
    {
        struct ReturnType temp = { Q_End, D };//завершаем работу
        return temp;
    }
    else // слова в прграмме еще есть -
    {
        D.d_header++; //сдвигаемся на следующее
        struct ReturnType temp = { Q_First, D}; // и обрабатыаем его
        return temp;
    }

}

struct ReturnType Q_nocommets(struct Dictionary D) // комментарий не встретился
{

    int i = D.d_header; // сдвиг относительно входного потока
    if (D.d_Input[i] == "") exit(1);
    string slovo = D.d_Input[i]; // изучаемое слово

    // пишем в код
    D.d_sizeCode++; // увеличиваем размер кода
    int founded = Wordlist.search(slovo); // находим это слово в списке зарезериваронных (это состояние вызвыается только елси такое слово точно есть)
    D.d_Code[D.d_sizeCode - 1] = Wordlist.lexemvalue(founded); // пишем в код значение полученного слова. примеер + и -7

    struct ReturnType temp = { Q_ConditionMoving, D }; // продолжаем движение
    return temp;


}

struct ReturnType Q_multiline(struct Dictionary D) // в случае многострочного коментария
{
    int i = D.d_header; // сдвиг относительного входного потока
    string slovo = D.d_Input[i]; // изучаемое слово

    if (slovo != "}" && slovo!="OK") // пока слово не станет завершаюшим символом для комментария
    { //или не уйдем в самый конец
        D.d_header++; // проходим на следующие слова без обработки

        struct ReturnType temp = { Q_multiline, D }; // повторяем
        return temp;
    }
    else // встретили завершающий символ
    {
        if (slovo == "}")
        {
            D.d_header++; // сдвинули головку на осмысленное слово
            struct ReturnType temp = { Q_First, D }; //идем анализировать по первоначальному протоколу
            return temp;
        }
        else // не встретили завершающего символа
        {
            struct ReturnType temp = { Q_End, D }; //идем анализировать по первоначальному протоколу
            return temp;
        }
    }
}

struct ReturnType Q_ItisWord(struct Dictionary D) // обработка слов - зарезервированных слов систем
{

    int i = D.d_header; // сдвиг отноистельного входного потока
    string slovo = D.d_Input[i]; // изучаемое слово
    if (slovo == "{") // начало многострочного комментария
    {
        D.d_header++; // передвигамеся на следующее слово
        struct ReturnType temp = { Q_multiline, D }; // и отправляемся к состоянию обрабатывающему комментарии
        return temp;
    }
    else
    {
        struct ReturnType temp = { Q_nocommets, D }; // комментарий не встречен -> идем к стандартной обработке слов
        return temp;
    }
    //}


    struct ReturnType temp = { Q_End, D };
    return temp;
}

struct ReturnType Q_NameEnd(struct Dictionary D) // завершаем работу с именем (было незнакомое имя)
{


    int i = D.d_header; //сдвиг относительно входного потока
    string slovo = D.d_Input[i]; // изучаемое слово
    D.d_sizeNames++; // увеличиваем количество имен
    D.d_Names[D.d_sizeNames - 1] = slovo; // кладем в значение поля само слово
    D.d_header++; // сдвинули головку на следующее слово
    struct ReturnType temp = { Q_First, D }; // обрабатываем его с самого начала
    return temp;


}
struct ReturnType Q_ItisName(struct Dictionary D) // обработка незнакомого слова - имени
{

    int i = D.d_header; // сдвиг относительно начала входного потока
    string slovo = D.d_Input[i]; // изучаемое слово

    int j = searching(D.d_sizeNames, D.d_Names, slovo); // смотрим есть ли такое слово уже в списке имен

    if( j!=-1) // такое слово уже есть
    {

        D.d_sizeCode++; // добавляем новый элемент в код - поток лексем
        D.d_Code[D.d_sizeCode - 1] = j+1; // его значение - лексема, отвечающая этому слову. к примеру solution и 6
        D.d_header++; //сдвигаем головку автомата, чтобы читать следующее слово
        struct ReturnType temp = { Q_First, D }; // возвращаемя к первоначльной обработке слова
        return temp;
    }
    else // такого слова нет
    {

        D.d_sizeCode++; // мы все равно печатаем в код значение
        D.d_Code[D.d_sizeCode - 1] = D.d_sizeNames+1;
        struct ReturnType temp = { Q_NameEnd, D }; // но возвращаемся не в начало, так как нужно еще записать это новое слово
        return temp;

    }

    D.d_header++; // и сдвигаем головку
    struct ReturnType temp = { Q_NameEnd, D };
    return temp;

}


struct ReturnType Q_First(struct Dictionary D) //первоначальный анализ слова из кода
{
    int i = D.d_header; // смещение относительно входного поток данных
    string slovo = D.d_Input[i]; // анализируемое слово
    if (slovo == "OK") // словом OK мы завершили входной поток данных при успешном формировании
        // условие сработает, если мы прочитали код до конца
    {
        struct ReturnType temp = { Q_End, D }; // тогда мы отправляемя к завершающему работу состоянию
        return temp;
    }

    if (Wordlist.search(slovo)!=-1) // если такое слово есть в списке зарезервирвоанных слов
    {
        struct ReturnType temp = { Q_ItisWord, D }; // отправляемся к состоянию, которое обрабатывает слова
        return temp;
    }
    else // если такого слова в списке зарезериваронных слов нет, -> это имя
    {
        struct ReturnType temp = { Q_ItisName, D };// отправляемся к состоянию, которое обрабатывает имена
        return temp;
    }

}

struct ReturnType Q_Start(struct Dictionary D) //начальное состояние для работы автомата
{
    struct ReturnType temp = {Q_First, D };
    return temp;
}

struct Dictionary RunMachine(string filename) // функция запускающая автомат
{

    int D_d_header = 0; //сдвига пока нет
    string* D_d_Input = forming(filename); //входной поток данных это string*
    int D_d_sizeNames = 0; // еще нет ни одного имени
    int D_d_sizeCode = 0; //и в потоке лексем пока пусто
    string* D_d_Names = (string*)malloc(sizeof(string) * max_size); // но выделили под них какое-то большое количество памяти
    int* D_d_Code = (int*)malloc(sizeof(int) * max_size);
    struct Dictionary D {D_d_header, D_d_Input, D_d_sizeNames, D_d_Names, D_d_sizeCode, D_d_Code}; // структура для записаи данных, пока пустая

    if (D.d_Input[0] == "") exit(1); // если почему-то входных данных нет - прекращаем работу
    struct Machine M; //структура для самой машины
    M.HeaderPos = 0; // головка равна 0
    M.MachineState = Q_Start; // начнем с самого первого состояния
    struct ReturnType MidResult; // структура для хранения типа


    while (M.MachineState != Q_End) // пока не окажемся в завершающем работу состоянии
    {
        // вызываем метод состояния на выполнение. Передаем ему значение текущей ячейки ленты
        MidResult = M.MachineState(D);
        // применяем результаты к нашей машине
        D = MidResult.Obj;
        //M.HeaderPos = M.HeaderPos + MidResult.MachineStep;
        M.MachineState = MidResult.ResultState;

        //cout << "Machine did step\n";
        //M.MachineStateName= MidResult.StateName;

        //условие для выхода чтобы не печатать на экран лишний раз
        if (M.MachineState == Q_End) break;
    }

    string* string_tmp = (string*)realloc(D.d_Names, sizeof(string) * D.d_sizeNames);
    if (string_tmp!=NULL)
        D.d_Names = string_tmp; // после отработки машины мы знаем истинные значения списков, поэтому перевыделили память
    else free(string_tmp);
    int* int_tmp = (int*)realloc(D.d_Code, sizeof(int) * D.d_sizeCode);
    if (int_tmp != NULL) D.d_Code = int_tmp;
    else free(int_tmp);

    cout << "Machine -- OK\n"; // машина нормально завершила работу
    return D; // вернули полученные нами данные
}

Table create_namelist(struct Dictionary D) // по уже полученной строке из автомата создаем таблицу имен по аналогии с таблицей слов
{
    Cell* tmpName = (Cell*)malloc(sizeof(Cell) * (D.d_sizeNames + 1));
    if (tmpName != NULL)
    {
        for (int i = 0; i < D.d_sizeNames; i++)
        {

            tmpName[i].c_sym(D.d_Names[i]);
            tmpName[i].c_ind(i + 1);
            //cout << (tmpName.take(i)).index() << "  --  " << (tmpName.take(i)).sym << "\n";
        }
        //cout << "no problem\n";
        Table Names(D.d_sizeNames, tmpName);
        free(tmpName);
        return Names;
    }
    else
    {
        free(tmpName);
        exit(1);
    }
}


/*
struct Dictionary KMA()
{
	//Table Wordlist = create_wordlist();
	//string* res = forming();
	/*struct Machine M {};
	M.HeaderPos = 0;
	M.MachineState = Q_Start;
	return RunMachine(M);*/
//	cout << "\nMachine -- OK\n";

//free(res);
//}

struct Files // структура для хранения всех важных вещей по итогу данной программы
{
    struct Dictionary D; //= RunMachine(); //некоторые полезные вещи -> на случай необходимости возвращения к исходным данным
    Table Wordlist; //= create_wordlist(); // словарь зарезервированных слов
    Table Namelist;// = create_namelist(D); // список имен в коде
    int CSize;// = D.d_sizeCode; //длина кода
    int* Code;// = D.d_Code; // код в виде потока лексем

};

#endif //COMPILER_DICTIONARY_H
