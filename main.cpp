#include <windows.h>
#include <iostream>
#include <map>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

enum class opType { add, edit, remove, retrieve };

// Изменить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool changeEntries(const string &key, const string &value, std::map<string, string> &out) {
    if (out.count(key) == 0) return false;

    auto it = out.find(key);
    it->second = value;

    return true;
}

// Добавить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool addToEntries(const string &key, const string &value, std::map<string, string> &out) {
    if (out.count(key) == 1) return false;

    out.insert(std::make_pair(key, value));

    return true;
}

// Удалить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool deleteFromEntries(const string &key, std::map<string, string> &out) {
    const auto it = out.find(key);

    if (it == out.end()) return false;

    out.erase(it);

    return true;
}

// Вернуть одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool retrieveValueByKey(string &target, const string &key, const std::map<string, string> &source) {
    const auto it = source.find(key);

    if (it == source.end()) return false;

    target = it->second;

    return true;
}

// Вернуть список значений по запрашиваемому значение. Возвращает количество обнаруженных совпадений
int retrieveKeysByValue(vector<string> &target, const string &desiredValue, const std::map<string, string> &source) {
    int found = 0;
    for (const auto &[key, value] : source)
        if (value == desiredValue) {
            target.emplace_back(key);
            ++found;
        }

    return found;
}

// Выводит отчет по записям
void displayEntries(const std::map<string, string> &source) {
    cout << "Отчет:" << endl;
    for (const auto &[key, value] : source) cout << key << ": " << value << endl;
}

// В зависимости от операции выводит на дисплей результат
void operationReport(opType mode, bool status, const string &keyword, int itemsAmount = 0) {
    vector<string> operationNames = { "Добавление", "Изменение", "Удаление", "Извлечение" };
    const char* operation = operationNames[static_cast<int>(mode)].c_str();
    const char* msg = "%s по ключу %s %s";
    const char* onSuccess = "прошло успешно.\n";
    string onSuccessExtended = "прошло успешно. Количество обработанных записей: " + std::to_string(itemsAmount) + "\n";
    vector<string> causes = { "такая запись уже существует", "такой записи не существует"};
    string onFailure = "не удалось, т.к. " + ((mode == opType::add) ? causes[0] : causes[1]) + "\n";

    if (status) {
        if (itemsAmount == 0) printf(msg, operation, keyword.c_str(), onSuccess);
        else printf(msg, operation, keyword.c_str(), onSuccessExtended.c_str());
    }
    else printf(msg, operation, keyword.c_str(), onFailure.c_str());
}

bool addToEntriesExtended(const string &key, const string &value, std::map<string, string> &out) {
    auto status = addToEntries(key, value, out);
    operationReport(opType::add, status, key);
    return status;
}

bool changeEntriesExtended(const string &key, const string &value, std::map<string, string> &out) {
    auto status = changeEntries(key, value, out);
    operationReport(opType::edit, status, key);
    return status;
}

bool removeFromEntriesExtended(const string &key, std::map<string, string> &out) {
    auto status = deleteFromEntries(key, out);
    operationReport(opType::remove, status, key);
    return status;
}

bool retrieveValueByKeyExtended(string &out, const string &key, const std::map<string, string> &from) {
    auto status = retrieveValueByKey(out, key, from);
    operationReport(opType::retrieve, status, key);
    return status;
}

int retrieveKeysByValueExtended(vector<string> &out, const string &desiredValue, const std::map<string, string> &from) {
    int itemsAmount = retrieveKeysByValue(out, desiredValue, from);
    operationReport(opType::retrieve, itemsAmount > 0, desiredValue, itemsAmount);
    return itemsAmount;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::map<string, string> phonebook {
            { "12-11", "Prokhorov" },
            { "12-12", "Ivanov" },
            { "12-15", "Petrov" },
    };

    addToEntriesExtended("12-18", "Avanesov", phonebook);
    addToEntriesExtended("12_16", "Smirnov", phonebook);
    addToEntriesExtended("12-13", "Ivanov", phonebook);
    addToEntriesExtended("23-23", "Ivanov", phonebook);
    addToEntriesExtended("12-11", "Petrosian", phonebook);

    displayEntries(phonebook);

    changeEntriesExtended("12-18", "Avanesian", phonebook);
    changeEntriesExtended("13-18", "Bykov", phonebook);

    displayEntries(phonebook);

    removeFromEntriesExtended( "12-18", phonebook);
    removeFromEntriesExtended("14-18", phonebook);

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
