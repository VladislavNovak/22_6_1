#include <windows.h>
#include <iostream>
#include <map>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

// Изменить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool changeEntries(std::map<string, string> &entries, const string &key, const string &value) {
    if (entries.count(key) == 0) return false;

    auto it = entries.find(key);
    it->second = value;

    return true;
}

// Добавить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool addToEntries(std::map<string, string> &entries, const string &key, const string &value) {
    if (entries.count(key) == 1) return false;

    entries.insert(std::make_pair(key, value));

    return true;
}

// Удалить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool deleteFromEntries(const string &key, std::map<string, string> &from) {
    const auto it = from.find(key);

    if (it == from.end()) return false;

    from.erase(it);

    return true;
}

// Вернуть одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool retrieveValueByKey(string &out, const string &key, const std::map<string, string> &from) {
    const auto it = from.find(key);

    if (it == from.end()) return false;

    out = it->second;

    return true;
}

// Вернуть список значений по запрашиваемому значение. Возвращает количество обнаруженных совпадений
int retrieveKeysByValue(vector<string> &out, const string &desiredValue, const std::map<string, string> &from) {
    int found = 0;
    for (const auto &[key, value] : from)
        if (value == desiredValue) {
            out.emplace_back(key);
            ++found;
        }

    return found;
}

// Выводит отчет по записям
void displayEntries(const std::map<string, string> &entries) {
    cout << "Отчет:" << endl;
    for (const auto &[key, value] : entries) cout << key << ": " << value << endl;
}

bool changeEntriesExtended(std::map<string, string> &out, const string &key, const string &value) {
    if (changeEntries(out, key, value)) return true;

    cout << "Изменение не удалось, т.к. записи с таким ключом (" << key << ") не существует" << endl;
    return false;
}

bool addToEntriesExtended(std::map<string, string> &entries, const string &key, const string &value) {
    if (addToEntries(entries, key, value)) return true;

    cout << "Добавление не удалось, т.к. запись с таким ключом (" << key << ") уже существует" << endl;
    return false;
}

bool deleteFromEntriesExtended(const string &key, std::map<string, string> &from) {
    if (deleteFromEntries(key, from)) return true;

    cout << "Удаление не удалось, т.к. записи с таким ключом (" << key << ") не существует" << endl;
    return false;
}

bool retrieveValueByKeyExtended(string &out, const string &key, const std::map<string, string> &from) {
    if (retrieveValueByKey(out, key, from)) return true;

    cout << "Извлечение не удалось, т.к. записи с таким ключом (" << key << ") не существует" << endl;
    return false;
}

int retrieveKeysByValueExtended(vector<string> &out, const string &desiredValue, const std::map<string, string> &from) {
    int result = retrieveKeysByValue(out, desiredValue, from);
    if (result) return result;

    cout << "Извлечение не удалось, т.к. записей с таким значением (" << desiredValue << ") не существует" << endl;
    return false;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::map<string, string> phonebook {
            { "12-11", "Prokhorov" },
            { "12-12", "Ivanov" },
            { "12-15", "Petrov" },
    };

    addToEntriesExtended(phonebook, "12-18", "Avanesov");
    addToEntriesExtended(phonebook, "12_16", "Smirnov");
    addToEntriesExtended(phonebook, "12-13", "Ivanov");
    addToEntriesExtended(phonebook, "23-23", "Ivanov");
    addToEntriesExtended(phonebook, "12-11", "Petrosian");

    displayEntries(phonebook);

    changeEntriesExtended(phonebook, "12-18", "Avanesian");
    changeEntriesExtended(phonebook, "13-18", "Bykov");

    displayEntries(phonebook);

    deleteFromEntriesExtended( "12-18", phonebook);
    deleteFromEntriesExtended("14-18", phonebook);

    displayEntries(phonebook);

    string resultOfRetrieve;
    bool isSuccess = retrieveValueByKeyExtended(resultOfRetrieve, "12-12", phonebook);
    if (isSuccess) cout << "Результат извлечения по ключу " << "(12-12): " << resultOfRetrieve << endl;

    isSuccess = retrieveValueByKeyExtended(resultOfRetrieve, "20-20", phonebook);
    if (isSuccess) cout << "Результат извлечения по ключу " << "(20-20): " << resultOfRetrieve << endl;

    vector<string> listOfKeys;
    int keysCount = retrieveKeysByValueExtended(listOfKeys, "Ivanov", phonebook);
    if (keysCount) {
        cout << "По ключу " << "Ivanov" << " извлечено " << keysCount << " записей:" << endl;
        for (const auto &item : listOfKeys) cout << item << " ";
        cout << endl;
    }

    keysCount = retrieveKeysByValueExtended(listOfKeys, "Fedorov", phonebook);
    if (keysCount) {
        cout << "По ключу " << "Fedorov" << " извлечено " << keysCount << " записей:" << endl;
        for (const auto &item : listOfKeys) cout << item << " ";
        cout << endl;
    }
}
