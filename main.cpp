#include <windows.h>
#include <iostream>
#include <map>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

enum class opType { add, edit, remove, retrieve };

std::string getTrimmedString(std::string str, std::string const &whiteSpaces = " \r\n\t\v\f") {
    auto start = str.find_first_not_of(whiteSpaces);
    str.erase(0, start);
    auto end = str.find_last_not_of(whiteSpaces);
    str.erase(end + 1);

    return str;
}

std::string getUserLineString(const std::string &msg) {
    while (true) {
        std::string userLineString;
        printf("%s: ", msg.c_str());
        std::getline(std::cin, userLineString);

        userLineString = getTrimmedString(userLineString);
        if (userLineString.empty()) {
            std::cout << "Строка не может быть пустой. Попробуйте снова!" << std::endl;
            continue;
        }

        return userLineString;
    }
}

// modified
void outputListToStream(std::ostream &out, std::vector<std::string> const &list, const std::string &delim = ",") {
    for (int i = 0; i < list.size(); ++i) out << list[i] << (i != list.size() - 1 ? delim : "");
    out << std::endl;
}

int selectFromList(std::vector<std::string> const &list) {
    bool isList = list.size() > 1;

    while (true) {
        cout << (isList ? "Выберите одну из опций: " : "Введите команду : ");
        outputListToStream(std::cout, list, (isList ? "|" : ""));

        auto userInput = getUserLineString("Наберите и нажмите enter");
        // return index from list, if word found
        for (int i = 0; i < list.size(); ++i) if (list[i] == userInput) return i;

        cout << "Неверно. Попробуйте снова!" << endl;
    }
}

// Изменить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool changeEntries(const std::pair<string, string> &newEntry, std::map<string, string> &target) {
    if (target.count(newEntry.first) == 0) return false;

    auto it = target.find(newEntry.first);
    it->second = newEntry.second;

    return true;
}

// Добавить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
bool addToEntries(const std::pair<string, string> &newEntry, std::map<string, string> &target) {
    if (target.count(newEntry.first) == 1) return false;

    target.insert(newEntry);

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

bool addToEntriesExtended(const std::pair<string, string> &newEntry, std::map<string, string> &out) {
    auto status = addToEntries(newEntry, out);
    operationReport(opType::add, status, newEntry.first);
    return status;
}

bool changeEntriesExtended(const std::pair<string, string> &newEntry, std::map<string, string> &out) {
    auto status = changeEntries(newEntry, out);
    operationReport(opType::edit, status, newEntry.first);
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

    std::map<string, string> phonebook;

    vector<string> commands = { "add", "edit", "remove", "retrieve", "about", "exit"};
    vector<string> commandsRu = { "добавить", "изменить", "удалить", "получить по ключу", "инфо", "выход"};

    while(true) {
        cout << "-----MENU-----" << endl;
        cout << "С записями возможны операции: ";
        outputListToStream(std::cout, commandsRu);
        auto index = selectFromList(commands);

        if (commands[index] == "add") {
            if (!phonebook.empty()) {
                cout << "Перед добавлением показать весь список?" << endl;
                if (selectFromList({ "yes", "no" }) == 0) displayEntries(phonebook);
            } else {
                cout << "Телефонный справочник пока пуст. Сейчас добавим новую запись." << endl;
            }

            while (true) {
                std::pair<string, string> newEntry;

                newEntry.first = getUserLineString("Введите телефон (простая строка)");
                newEntry.second = getUserLineString("Введите фамилию владельца");

                addToEntriesExtended(newEntry, phonebook);

                cout << "Закончили добавление?" << endl;
                if (selectFromList({ "yes", "no" }) == 0) break;
            }
        }
        else if (commands[index] == "edit") {
            if (!phonebook.empty()) {
                cout << "Перед добавлением показать весь список?" << endl;
                if (selectFromList({ "yes", "no" }) == 0) displayEntries(phonebook);
            } else {
                cout << "Телефонный справочник пока пуст" << endl;
                continue;
            }

            while (true) {
                std::pair<string, string> newEntry;

                // Вообще исключаем введение неверных ключей
                vector<string> keys;
                keys.reserve(phonebook.size());
                for (const auto &[key, value] : phonebook) keys.emplace_back(key);
                cout << "Введите один из телефонов, который собираетесь редактировать:" << endl;
                newEntry.first = keys[selectFromList(keys)];
                newEntry.second = getUserLineString("Введите новую фамилию владельца");

                changeEntriesExtended(newEntry, phonebook);

                cout << "Закончили редактирование?" << endl;
                if (selectFromList({ "yes", "no" }) == 0) break;
            }
        }
        else if (commands[index] == "remove") {

        }
        else if (commands[index] == "retrieve") {
            // добавить выбор by key | by value
        }
        else if (commands[index] == "about") {
            displayEntries(phonebook);
        }
        else if (commands[index] == "exit") {
            cout << "Программа завершает свою работу. До новых встреч" << endl;
            break;
        }
    }


    // removeFromEntriesExtended( "12-18", phonebook);
    // removeFromEntriesExtended("14-18", phonebook);
    //
    // displayEntries(phonebook);
    //
    // string resultOfRetrieve;
    // bool isSuccess = retrieveValueByKeyExtended(resultOfRetrieve, "12-12", phonebook);
    // if (isSuccess) cout << "Результат извлечения по ключу " << "(12-12): " << resultOfRetrieve << endl;
    //
    // isSuccess = retrieveValueByKeyExtended(resultOfRetrieve, "20-20", phonebook);
    // if (isSuccess) cout << "Результат извлечения по ключу " << "(20-20): " << resultOfRetrieve << endl;
    //
    // vector<string> listOfKeys;
    // int keysCount = retrieveKeysByValueExtended(listOfKeys, "Ivanov", phonebook);
    // if (keysCount) {
    //     cout << "По ключу " << "Ivanov" << " извлечено " << keysCount << " записей:" << endl;
    //     for (const auto &item : listOfKeys) cout << item << " ";
    //     cout << endl;
    // }
    //
    // keysCount = retrieveKeysByValueExtended(listOfKeys, "Fedorov", phonebook);
    // if (keysCount) {
    //     cout << "По ключу " << "Fedorov" << " извлечено " << keysCount << " записей:" << endl;
    //     for (const auto &item : listOfKeys) cout << item << " ";
    //     cout << endl;
    // }
}
