#include <iostream>
#include <fstream>
#include <vector>   // шаблонный класс-массив(класс-вектор)
#include <map>  // шаблонный класс-словарь
#include <list> // шаблонный класс-список

using namespace std;

class Haffman_Tree {    /// вспомогательный класс дерево
public:
    int key;
    char s;
    Haffman_Tree* left, * right;
    Haffman_Tree() { left = NULL; right = NULL; }
    Haffman_Tree(Haffman_Tree* l, Haffman_Tree* r) {
            /// конструктор на скрепление двух ветвей в одну
        left = l;
        right = r;
        key = l->key + r->key;
    }

    ~Haffman_Tree() {
        //while (left)
       //     delete left;
       // while (right)
       //     delete right;
        if (left)delete left;
        // в рекурсии удаляем память под ветви
                    // именно рекурсия, потому что left и right имеют тип данных
                    // сам класс, поэтому удаляя left и right он будет вызывать 
                    // себя же для удаления уже от их имени
        if (right) delete right;
        left = NULL;
        right = NULL;
    }
};

class Haffman {      /// агрегированный класс под коды и дерево
    Haffman_Tree* root; /// корень нашего дерева Хаффмана
    map<char, int> alphabet;   /// алфавит символов входной строки с счётчиком повторений
                        /// (доступ по ключу - символу)
    map<char, int> ::iterator ii;       /// итератор для удобства прохода по дереву

    vector<bool> code;      /// массив из нулей и единиц для задания кода
    map<char, vector<bool> > buf;   /// при закодировании мы будем "заталкивать" все коды
                /// в char, то есть в переменную 8 бит, отсюда выйдут остатки кодов, если есть длины не кратные 8

public:
    Haffman();   /// конструктор по умолчанию(вызывает неявно конструкторы для всех используемых
                /// классов
    ~Haffman();  /// деструктор, также вызывает неявно все деструкторы

    double buildForEncode(ifstream& f, ostream& out_f) {
            /// здесь будем закодировать
        // * позже вынести из тела класса !
    
    }


    void buildForEncode(ifstream& f);   /// строим дерево для закодирования входной строки
            //void buildForDecode(); 2-я фаза прописать !!
    
    struct Sort {       /// в Хаффмане нужна сортировка, на дереве это коды lfet - 0, right - 1 и т.д.
        bool operator() (const Haffman_Tree* l, const Haffman_Tree* r) {
            return l->key < r->key;
        }
    };
};




Haffman::Haffman() {    // по умолчанию дерево пустое
    root = NULL;
}

Haffman::~Haffman() {   // деструктор рекурсивный(т.к. дерево) поэтому 
                        // описываем его вне тела класса ( рекомендация inline 
                        // не применима в данном случае )
    delete root;
    root = NULL;
}

void Haffman::buildForEncode(ifstream& f) {
    if (root != NULL) { // если там что-то лежит - удаляем
        delete root;
        root = NULL;
    }
    while (!f.eof()) {  /// считываем с файла всю входную строку
        char c = f.get();
        alphabet[c]++;
    }
    list<Haffman_Tree*> L;
    for (ii = alphabet.begin(); ii != alphabet.end(); ii++) {
        Haffman_Tree* p = new Haffman_Tree;
        p->s = ii->first;
        p->key = ii->second;
        L.push_back(p);
    }
    while (L.size() != 1) {
        L.sort(Sort());
        Haffman_Tree* Left = L.front();
        L.pop_front();
        Haffman_Tree* Right = L.front();
        L.pop_front();
        Haffman_Tree* pr = new Haffman_Tree(Left, Right);
        L.push_back(pr);
    }
    root = L.front();
}

/// map<type1, type2> - класс-шаблон из std для словаря
/*
    Будем считать, что ключ - название товара, а значение - цена товара.
    То есть в данном случае элементу с ключом "bread" присваивается значение 30.
    При этом не важно, что ранее создан пустой словарь, и в нем нет никакого элемента с ключом "bread"
    - если его нет, то он создается. Если же элемент с данным ключом уже есть, то меняется его значение.

Чтобы получить элемент по определенному ключу, используем тот же синтаксис.
Например, поскольку значение элемента - число, то мы можем, обратившись по ключу, получить это число
cout<<mapa[key], где key - type1, выведется на консоль - type2



    */
int main() {

    // =====================================================
    // * * * * * * * I N T E R F A C E * * * * * * * * * * *
    //======================================================

    char* path = new char[200];     // переменная под строку пути файла
    cout << "Enter path(C://folder//...//file.txt";
    cin >> path;

    char quest_for_action = false;
    cout << "What do we do? (0 - encode,  1 - decode)";
    
    cin >> quest_for_action;
    quest_for_action -= 48;

    if (!quest_for_action) {       /// ввели 0 - закодирование файла
        ifstream file_in(path, ios::in | ios::binary); // открытие для чтения
                    // т.к. разные символы кодируется по разному, в том числе 
                    // по кол-ву бит на символ, то открываем файл в режиме битов
                    // чтобы не заморачиваться над форматом кодировки самого файла
        Haffman haf;



    return 0;
}