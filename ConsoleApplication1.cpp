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


    void buildForEncode(ifstream& f);
    /// строим дерево для закодирования входной строки
            //void buildForDecode(); 2-я фаза прописать !!
    
    void encode(ifstream& in_f, ofstream& out_f);
    /// сама функция кодирования файла по уже построенному дереву


    struct Sort {       /// в Хаффмане нужна сортировка, на дереве это коды lfet - 0, right - 1 и т.д.
        bool operator() (const Haffman_Tree* l, const Haffman_Tree* r) {
            return l->key < r->key;
        }
    };
};

void Haffman::encode(ifstream& f, ofstream& g) {
    buildForEncode(f);  // строим дерево по входному файлу

    Haffman_Tree(root); 


    int count = 0;
    for (ii = alphabet.begin(); ii != alphabet.end(); ii++) {
        if (ii->second != 0) count += 40;
    }
    //count пригодится для раскодирования
    // т.к в file.write() нужно указывать кол-во символов
    // то преобразуя int в последовательность цифр (char*) 
    // нужно учитывать, что каждый разряд count будет давать ещё
    // один байт
    g.write((char*)(&count), sizeof(count));

    for (int i = 0; i < 256; i++) {
        if (alphabet[char(i)] > 0) {
            char c = char(i);
            g.write((char*)(&c), sizeof(c));
            g.write((char*)(&alphabet[char(i)]), sizeof(alphabet[char(i)]));
        }// здесь мы пишем табличку кодов
    }
    f.clear();//очищаем входной файл
    f.seekg(0); // устанавливаем курсор на начало файла
    count = 0;

    char tx = 0;    // запись самого шифра из нулей и единиц по байтам
    while (!f.eof()) {
        char c = f.get();

        vector<bool> x = buf[c]; // получаем текущий код символа 
        // Haffman :: map <char, vector<bool> > buf;

        for (int j = 0; j < x.size(); j++) {
            tx = tx | x[j] << (7 - count);//формируем байт вывода
            count++;
            if (count == 8)
            {//если байт переполнился - выводим его в файл
                count = 0;
                g << tx;
                tx = 0;
            }
        }
    }
    f.clear();// очищаем флаги ошибок потока
    // о необходиости этого действия узнал из документации
    // если при чтении дошли до конца, то имеет другое состояние
    // с которым работать дальше нельзя, поэтому ifstream_file.clear() 
    // очищает эти флаги ошибок и всё такое
    f.seekg(0, std::ios::end); // устанавливаем курсор на начальную позицию

    g.seekp(0, std::ios::end); // устанавливаем указатель на начальную позицию
    double sizeF = f.tellg();
    double sizeG = g.tellp();
    f.close();
    g.close();
    
}


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
    list<Haffman_Tree*> L;  // алфавит 
    for (ii = alphabet.begin(); ii != alphabet.end(); ii++) {
        Haffman_Tree* p = new Haffman_Tree;
        p->s = ii->first;   // заносим в алфавит текущий символ
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
        // формируем дерево начиная с листьев, пока размер дерева
        // не достигнет единицы (дальше некуда сворачивать)
    }
    root = L.front();   // наконец, устанавливаем наш корень в начало
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

    char *path = new char[200];     // переменная под строку пути файла
    char* path2 = new char[200];    // строка для выходного файла
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
        cout << "enter path to encode (do not Enter the same path please)" << endl;
        cin >> path2;

        ofstream file_out(path2, ios::out | ios::binary);
        Haffman haf;
        haf.encode(file_in, file_out);

        cout << "Coded!" << endl << "path : " << path2 << endl;
        return 0;
    }



    return 0;
}