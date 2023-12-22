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
    Haffman_Tree* left;
    Haffman_Tree *right;
    Haffman_Tree() { left = NULL; right = NULL; }
    Haffman_Tree(Haffman_Tree* l, Haffman_Tree* r) {
            /// конструктор на скрепление двух ветвей в одну
        left = l;
        right = r;
        key = l->key + r->key;
    }

    ~Haffman_Tree() {
        //while (left)    delete left;   while (right)    delete right; // НЕ РАБОТАЕТ
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
    map<char, vector<bool> > buff;   /// при закодировании мы будем "заталкивать" все коды
                /// в char, то есть в переменную 8 бит, отсюда выйдут остатки кодов, если есть длины не кратные 8

public:
    Haffman();   /// конструктор по умолчанию(вызывает неявно конструкторы для всех используемых
                /// классов
    ~Haffman();  /// деструктор, также вызывает неявно все деструкторы

    void TreeHaf(Haffman_Tree *);  // табличка готова, приступаем к раздаче кодов для символов для раскодирования
    void pre_build_Tree_ForEncode(ifstream &); // done

    void pre_build_Tree_ForDecode(ifstream&);
    // заготовка под раскодирование


    /// строим дерево для закодирования входной строки
            //void pre_buld_Tree_ForDncode(); 2-я фаза прописать !!
    
    void encode(ifstream&, ofstream&);  // done
    /// сама функция кодирования файла по уже построенному дереву
    void decode(ifstream&, ofstream&);


    struct Sort {       /// в Хаффмане нужна сортировка, на дереве это коды lfet - 0, right - 1 и т.д.
        bool operator() (const Haffman_Tree *l, const Haffman_Tree *r) {
            return (l->key < r->key);
        }
    };
};
void Haffman::TreeHaf(Haffman_Tree *r) {    // раздача кодов по для символов по построенному дереву Хаффмана
    if (r->left != NULL) {
        code.push_back(0);  // векторы удобные!
        TreeHaf(r->left); // рекурсия для достижения позиции листика с одним символов и присвоения ему кода
    }
    if (r->right != NULL) { // проходим по дереву к листьям от корня
        code.push_back(1);
        TreeHaf(r->right);// аналогичная рекурсия
    }
    if (r->right == NULL && r->left == NULL) {  // дошли до листика с одним символом
        buff[r->s] = code;  // |=> даём ему наш "набранный" код
    }
    if (!code.empty()) // коды раздали, теперь вспомогательную память(она здесь более не понадобится)
        code.pop_back();
}
void Haffman::encode(ifstream& in_f, ofstream& out_f) {
    pre_build_Tree_ForEncode(in_f);  // строим дерево по входному файлу
    TreeHaf(root); 


    int count = 0;
    for (ii = alphabet.begin(); ii != alphabet.end(); ii++) {
        if (ii->second != 0) count += 40;
    }
    //count пригодится для раскодирования
    // т.к в file.write() нужно указывать кол-во символов
    // то преобразуя int в последовательность цифр (char*) 
    // нужно учитывать, что каждый разряд count будет давать ещё
    // один байт
    out_f.write((char*)(&count), sizeof(count)); /// выводим длину имеющейся строки

    for (int i = 0; i < 256; i++) {
        if (alphabet[char(i)] > 0) {
            char c = char(i);
            out_f.write((char*)(&c), sizeof(c));
            out_f.write((char*)(&alphabet[char(i)]), sizeof(alphabet[char(i)]));
        }// здесь мы пишем табличку кодов
    }
    in_f.clear();//очищаем входной файл
    in_f.seekg(0); // устанавливаем курсор на начало файла
    count = 0;

    char tx = 0;    // запись самого шифра из нулей и единиц по байтам
    while (!(in_f.eof())) {
        char c = in_f.get();

        vector<bool> x = buff[c]; // получаем текущий код символа 
        // Haffman :: map <char, vector<bool> > buff;

        for (int j = 0; j < x.size(); j++) {
            tx = tx | x[j] << (7 - count);//формируем байт вывода
            count++;
            if (count == 8)
            {//если байт переполнился - выводим его в файл
                count = 0;
                out_f << tx;
                tx = 0;
            }
        }
    }
    in_f.clear();// очищаем флаги ошибок потока
    // о необходиости этого действия узнал из документации
    // если при чтении дошли до конца, то имеет другое состояние
    // с которым работать дальше нельзя, поэтому ifstream_file.clear() 
    // очищает эти флаги ошибок и всё такое
    in_f.seekg(0, std::ios::end); // устанавливаем курсор на начальную позицию

    out_f.seekp(0, std::ios::end); // устанавливаем указатель на начальную позицию
    double sizeF = in_f.tellg();
    double sizeG = out_f.tellp();
    in_f.close();
    out_f.close();
    
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

void Haffman::pre_build_Tree_ForEncode(ifstream& in_f) {

    if (root != NULL) { // если там чё-то есть - чистим
        delete root;
        root = NULL;
    }
    while (!in_f.eof()) { // считываем файл
        char c = in_f.get();
        alphabet[c]++;      // считаем счётчики повторений
    }
    list<Haffman_Tree*> L;
    for (ii = alphabet.begin(); ii != alphabet.end(); ii++) {
        Haffman_Tree* p = new Haffman_Tree;
        p->s = ii->first;   // строим дерево по полученному алфавиту и счётчикам
        p->key = ii->second;
        L.push_back(p);
    }
    while (L.size() != 1) {
        L.sort(Sort());     // сортируем дерево 
        Haffman_Tree* Left = L.front();
        L.pop_front();
        Haffman_Tree* Right = L.front();
        L.pop_front();
        Haffman_Tree* pr = new Haffman_Tree(Left, Right);
        L.push_back(pr);
    }
    root = L.front();
}
void Haffman::pre_build_Tree_ForDecode(ifstream& in_f_coded) {
    if (root != NULL) {
        delete root;    // чистим ненужные данные
        root = NULL;
    }
    if (alphabet.size() != 0) // чистим ненужные данные
        alphabet.clear();
    if (code.size() != 0)
        code.clear();
    if (buff.size() != 0)
        buff.clear();

    int x1, x2;
    char s;
    in_f_coded.read((char*)&x1, sizeof(x1));
    while (x1 > 0) {
        in_f_coded.read((char*)&s, sizeof(s));
        in_f_coded.read((char*)&x2, sizeof(x2));
        x1 -= 40;
        alphabet[s] = x2;
    }
    list<Haffman_Tree*> L;
    for (ii = alphabet.begin(); ii != alphabet.end(); ii++) {
        Haffman_Tree* p = new Haffman_Tree;
        p->s = ii->first;
        p->key = ii->second;
        L.push_back(p);
    }
    while (L.size() != 1) { //  Аналогично строим дерево
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



    

void Haffman::decode(ifstream& f_in_coded, ofstream& f_out) {
    if (f_in_coded.is_open()) {
        pre_build_Tree_ForDecode(f_in_coded);
    }
    else
        return;
    TreeHaf(root);  // имеем дерево после pre_buld_Tree_ForDncode(f_in_coded)

    char crypt_byte;
    int count = 0;
    Haffman_Tree *p = root;
    crypt_byte = f_in_coded.get();
    while (!f_in_coded.eof()) { // начинаем проходить по файлу с шифром
        bool b = crypt_byte & (1 << (7 - count)); // заносим в выводимый байт коды
        if (b)
            p = p->right; // проверка на текущий бит, чтобы по дереву достичь символа из таблицы
        else
            p = p->left;
        if (p->right == NULL && p->left == NULL) { // дошли до листа с символом из таблицы
            f_out << p->s; // выведи символ, идём заного по циклу
            p = root; // вернулись в корень дерева после вывода символа
        }
        count++; // счётчик последующего бита
        if (count == 8) { //если переполнился рабочий байт, то обнуляем его
            count = 0;  // даже если код символа >8k, (k - целое), то позиция в дереве (*p) всё равно сохранена
                        // и просто ищем дальше, но считываем следующий байт шифра с файла
            crypt_byte = f_in_coded.get();
        }
    }

}


int main() {

    // =====================================================
    // * * * * * * * I N T E R F A C E * * * * * * * * * * *
    //======================================================
    cout << "Hello!" << endl;
    char *path = new char[200];     // переменная под строку пути файла
    char* path2 = new char[200];    // строка для выходного файла
    cout << "Enter path to input_file: "<<endl;
    cin >> path;

    char quest_for_action = false;
    cout << "What do we do? (0 - encode,  1 - decode):  ";
    
    cin >> quest_for_action;
    quest_for_action -= 48;

    if (!quest_for_action) {       /// ввели 0 - закодирование файла
        ifstream file_in(path, ios::in | ios::binary); // открытие для чтения
        // т.к. разные символы кодируется по разному, в том числе 
        // по кол-ву бит на символ, то открываем файл в режиме битов
        // чтобы не заморачиваться над форматом кодировки самого файла
        cout << "Enter path to encoded (Please, do not enter the same path): " << endl;
        cin >> path2;

        ofstream file_out(path2, ios::out | ios::binary);
        Haffman haf;
        haf.encode(file_in, file_out);

        cout << "Coded!" << endl << "path : " << path2 << endl;
        return 0;
    }
    if (quest_for_action == 1) {
        ifstream file_in(path, ios::in | ios::binary);
        cout << "enter path to decoded (Please, do not enter the same path): " << endl;
        cin >> path2;
        ofstream file_out(path2, ios::out | ios::binary);
        Haffman decoded_thing;
        decoded_thing.decode(file_in, file_out);
        cout << "Decoded!" << endl << "path: " << path2 << endl;
        return 0;
    }
    cout << "Error with input" << endl;

    /// ФОРМИРУЕТ ТАБЛИЧКУ И КОДИРУЕТ ФАЙЛ!!!!!!!


    return 0;
}