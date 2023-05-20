#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <string>
#include <map>

using namespace std;

string inputFile = "data.txt";
string outputFile = "output.txt";
string timeStampsFile = "timestamps.txt";

ofstream fout(outputFile);

int batchNum = 7;

int dims[7] = { 100, 500, 1000, 5000, 10000, 50000, 100000 };

//date in unix format (second from epoch)

class Flight {
    int number;
    string companyName;
    unsigned long long date;
    unsigned long long time;
    int passengerNumber;

public:
    Flight()
    {
        this->number = 0;
        this->companyName = "companyName";
        this->date = 0;
        this->time = 0;
        this->passengerNumber = 0;
    }

    Flight(int number,
    string companyName,
    unsigned long long date,
    unsigned long long time,
    int passengerNumber)
    {
        this->number = number;
        this->companyName = companyName;
        this->date = date;
        this->time = time;
        this->passengerNumber = passengerNumber;
    }

    friend bool operator== (const Flight& a, const Flight& b) {
        if (a.time == b.time && a.companyName == b.companyName && a.date == b.date && a.passengerNumber == b.passengerNumber)
            return true;
        return false;
    }

    friend bool operator< (const Flight& a, const Flight& b) {
    if (a.date < b.date || ((a.date == b.date) && (a.time < b.time)) ||
        ((a.date == b.date) && (a.time == b.time) && (a.companyName < b.companyName)) ||
        ((a.date == b.date) && (a.time == b.time) && (a.companyName == b.companyName) && (a.passengerNumber < b.passengerNumber)))
        return true;
    return false;
    }
    friend bool operator> (const Flight& a, const Flight& b) {
        if (b < a)
            return true;
        return false;
    }

    friend bool operator<= (const Flight& a, const Flight& b) {
        return !(b < a);
    }

    friend bool operator>= (const Flight& a, const Flight& b) {
        return !(a < b);
    }

    friend ostream& operator<<(ostream& os, const Flight& a) {
        os << a.date << " " << a.time << " " << a.companyName << " " << a.passengerNumber << "\n";
        return os;
    }

    string getCompanyName() const {
        return this->companyName;
    }
};

//записывает время по 2 врменным точкам
void writeTime(string title, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    fout << title << endl;

    fout << chrono::duration_cast<chrono::microseconds>(end - start).count() << " [микросекунд]\n\n";
}

//функция прямого поиска, возвращает массив найденных индексов
vector<int> straightSearch(vector<Flight>& v, string key) {
    vector<int> res;

    for (int i = 0; i < v.size(); ++i)
        if (v[i].getCompanyName() == key)
            res.push_back(i);

    return res;
}

//найти нижнюю границу удовлетворящих значений двоичным поиском
int getLowerBound(vector<Flight>& v, string key) {
    int mid = 0, left = 0, right = v.size();
    while (1)
    {
        mid = (left + right) / 2;

        if (mid < 0 || mid >= v.size())
            return -1;

        if (key <= v[mid].getCompanyName())
            right = mid - 1;
        else if (key > v[mid].getCompanyName() && mid + 1 < v.size() && key == v[mid + 1].getCompanyName())
            return mid;
        else if (key > v[mid].getCompanyName())
            left = mid + 1;

        if (left > right)
            return -1;
    }
}

//найти верхнюю границу удовлетворящих значений двоичным поиском
int getUpperBound(vector<Flight>& v, string key) {
    int mid = 0, left = 0, right = v.size();
    while (1)
    {
        mid = (left + right) / 2;

        if (mid < 0 || mid >= v.size())
            return -1;

        if (key < v[mid].getCompanyName() && mid - 1 >= 0 && key == v[mid - 1].getCompanyName())
            return mid;
        else if (key < v[mid].getCompanyName())
            right = mid - 1;
        else if (key >= v[mid].getCompanyName())
            left = mid + 1;

        if (left > right)
            return -1;
    }
}

//сам двоичный поиск
vector<Flight> binarySearch(vector<Flight>& v, string key) {
    int left = getLowerBound(v, key);
    int right = getUpperBound(v, key);

    if (left == -1 || right == -1)
        return vector<Flight>();

    auto first = v.cbegin() + left + 1;
    auto last = v.cbegin() + right;

    vector<Flight> res(first, last);

    return res;
}

//функция для сравнения 2 объектов класса
bool comp(Flight a, Flight b) { return (a.getCompanyName() < b.getCompanyName()); }

//функция для генерации случайной строки с заданной длиной
string randomString(const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string tmp_s;
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return tmp_s;
}

//функция для генерации случайного числа типа unsigned long long
unsigned long long randomLongLong(double begin, double end)
{
    return begin >= end ? 0 : (unsigned long long)(begin + (unsigned long long)rand() * ((end - begin) / RAND_MAX));
}

//функция, создающая текстовый файл для чтения после
//1684013632 - это 00:34 от 14.05.2023 
void makeTxtFile() {
    srand((unsigned)time(NULL));
    ofstream fout(inputFile);

    for (int i = 0; i < batchNum; ++i) {
        //Вывод числа объектов
        fout << dims[i] << "\n";
        for (int j = 0; j < dims[i]; ++j) {
            //Вывод полей объекта
            fout << rand() % 1000 + 1 << "\n";              //Номер рейса
            fout << randomString(10) << "\n";               //Название авиакомпании
            fout << randomLongLong(0, 1684013632) << "\n";  //Дата прилета
            fout << randomLongLong(0, 1684013632) << "\n";  //Время  прилета по расписанию
            fout << rand() % 200 << "\n";                   //Число пассажиров на борту
        }
    }
}

//функция для чтения объектов из текстового файла
vector<vector<Flight>> readTxtFile() {
    ifstream fin(inputFile);

    vector<vector<Flight>> result;

    int dim;
    int number;
    string companyName;
    unsigned long long date;
    unsigned long long time;
    int passengerNumber;

    for (int i = 0; i < batchNum; ++i) {
        //Ввод числа объектов
        fin >> dim;
        vector<Flight> v;
        for (int j = 0; j < dim; ++j) {
            //Ввод полей объекта
            fin >> number >> companyName >> date >> time >> passengerNumber;
            Flight temp(number, companyName, date, time, passengerNumber);
            v.push_back(temp);
        }
        result.push_back(v);
    }

    return result;
}

int main()
{
    setlocale(LC_ALL, "Russian");

    makeTxtFile();

    //массив объектов
    vector<vector<Flight>> initialArray = readTxtFile();

    //массив с ключами, которые будут искаться 
    vector<string> selectedKeys;

    for (int i = 0; i < batchNum; ++i) {
        selectedKeys.push_back(initialArray[i][rand() % initialArray[i].size()].getCompanyName());
    }

    std::chrono::steady_clock::time_point start, end;
    Flight foundValue;

    fout << "Прямой поиск:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        vector<int> temp = straightSearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Искался ключ: " << selectedKeys[i] << endl;
        if (!temp.empty())
            for (int j = 0; j < temp.size(); ++j)
                fout << "Найден элемент: " << initialArray[i][temp[j]];
        writeTime("На прямой поиск с " + to_string(initialArray[i].size()) + " значениями ушло: ", start, end);
        start = end;
    }

    fout << "Сортировка:" << endl << endl;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        std::sort(initialArray[i].begin(), initialArray[i].end(), comp);
        binarySearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        writeTime("На сортировку и двоичный поиск " + to_string(initialArray[i].size()) + " значениий ушло: ", start, end);
        start = end;
    }

    fout << "Бинарный поиск в отсортированном массиве:" << endl;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        vector<Flight> temp = binarySearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Искался ключ: " << selectedKeys[i] << "\n";
        if (!temp.empty())
            for (int j = 0; j < temp.size(); ++j)
                fout << "Найден элемент: " << temp[j];
        writeTime("На бинарный поиск с " + to_string(initialArray[i].size()) + " значениями ушло: ", start, end);
        start = end;
    }

    vector<multimap<string, Flight>> m;

    for (int i = 0; i < batchNum; ++i) {
        multimap<string, Flight> temp;
        for (int j = 0; j < initialArray[i].size(); ++j)
            temp.insert({ initialArray[i][j].getCompanyName(), initialArray[i][j] });
        m.push_back(temp);
    }

    initialArray.clear();

    fout << "Поиск в ассоциативном массиве:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        auto bounds = m[i].equal_range(selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Искался ключ: " << selectedKeys[i] << "\n";
        if (bounds.first != m[i].end())
            for (auto it = bounds.first; it != bounds.second; ++it)
                fout << "Найден элемент: " << it->second;
        writeTime("На поиск в ассоциативном массиве с " + to_string(m[i].size()) + " значениями ушло: ", start, end);
        start = end;
    }

    return 0;
}
