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

// Добавить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool addEntryToMap(const std::pair<F, S> &entry, std::map<F, S> &target) {
    if (target.count(entry.first) == 1) return false;

    target.insert(entry);

    return true;
}

// Изменить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool changeEntryInMap(const std::pair<F, S> &entry, std::map<F, S> &target) {
    if (target.count(entry.first) == 0) return false;

    auto it = target.find(entry.first);
    it->second = entry.second;

    return true;
}

// Удалить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool removeEntryFromMap(const F &key, std::map<F, S> &target) {
    const auto it = target.find(key);

    if (it == target.end()) return false;

    target.erase(it);

    return true;
}

// Вернуть одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool retrieveMapValueByKey(S &target, const F &key, const std::map<F, S> &source) {
    const auto it = source.find(key);

    if (it == source.end()) return false;

    target = it->second;

    return true;
}

// Вернуть список значений по запрашиваемому значение. Возвращает количество обнаруженных совпадений
template<typename F, typename S>
int retrieveMapKeysByValue(vector<F> &collectionOfKeys, const S &desiredValue, const std::map<F, S> &source) {
    int found = 0;
    for (const auto &[key, value] : source) {
        if (value == desiredValue) {
            collectionOfKeys.emplace_back(key);
            ++found;
        }
    }

    return found;
}

// Выводит отчет по записям
void displayEntries(const std::map<string, string> &source) {
    cout << "Отчет:" << endl;
    for (const auto &[key, value] : source) cout << key << ": " << value << endl;
}

// В зависимости от операции выводит на дисплей результат
void printLog(opType mode, bool status, const string &keyword, int itemsAmount = 0) {
    vector<string> operationNames = { "Добавление", "Изменение", "Удаление", "Извлечение" };
    const char* operation = operationNames[static_cast<int>(mode)].c_str();
    const char* msg = "LOG: %s по ключу %s %s";
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

// Функции Extended аналогичны базовым прототипам, но имеют ещё логирование
template<typename F, typename S>
bool addEntryToMapExtend(const std::pair<F, S> &entry, std::map<F, S> &target) {
    auto status = addEntryToMap(entry, target);
    printLog(opType::add, status, entry.first);
    return status;
}

template<typename F, typename S>
bool changeEntryInMapExtended(const std::pair<F, S> &entry, std::map<F, S> &target) {
    auto status = changeEntryInMap(entry, target);
    printLog(opType::edit, status, entry.first);
    return status;
}

template<typename F, typename S>
bool removeEntryFromMapExtended(const F &key, std::map<F, S> &target) {
    auto status = removeEntryFromMap(key, target);
    printLog(opType::remove, status, key);
    return status;
}

template<typename F, typename S>
bool retrieveMapValueByKeyExtended(S &target, const F &key, const std::map<F, S> &source) {
    auto status = retrieveMapValueByKey(target, key, source);
    printLog(opType::retrieve, status, key);
    return status;
}

template<typename F, typename S>
int retrieveMapKeysByValueExtended(vector<F> &collectionOfKeys, const S &desiredValue, const std::map<F, S> &source) {
    int itemsAmount = retrieveMapKeysByValue(collectionOfKeys, desiredValue, source);
    printLog(opType::retrieve, itemsAmount > 0, desiredValue, itemsAmount);
    return itemsAmount;
}

bool abortIfDataEmpty(opType mode, const std::map<string, string> &phonebook) {
    vector<string> operationNames = { "добавлением", "изменением", "удалением", "извлечением" };
    const char* operation = operationNames[static_cast<int>(mode)].c_str();

    if (!phonebook.empty()) {
        cout << "Перед " << operation << " показать весь список?" << endl;
        if (selectFromList({ "yes", "no" }) == 0) displayEntries(phonebook);
        return false;
    } else {
        cout << "Телефонный справочник пока пуст" << endl;
        return true;
    }
}

void menuAdd(std::map<string, string> &phonebook) {
    while (true) {
        std::pair<string, string> newEntry;

        newEntry.first = getUserLineString("Введите телефон (простая строка)");
        newEntry.second = getUserLineString("Введите фамилию абонента");

        addEntryToMapExtend(newEntry, phonebook);

        cout << "Закончили добавление?" << endl;
        if (selectFromList({ "yes", "no" }) == 0) break;
    }
}

void menuRemove(std::map<string, string> &phonebook) {
    while (true) {
        // Вообще исключаем введение неверных ключей
        vector<string> keys;
        keys.reserve(phonebook.size());
        for (const auto &[key, value] : phonebook) keys.emplace_back(key);
        cout << "Введите один из телефонов, который собираетесь удалить:" << endl;
        string selectedKey = keys[selectFromList(keys)];

        removeEntryFromMapExtended(selectedKey, phonebook);

        cout << "Закончили удаление?" << endl;
        if (selectFromList({ "yes", "no" }) == 0) break;
    }
}

void menuRetrieve(std::map<string, string> &phonebook) {
    while (true) {
        vector<string> retrieveCommands = { "by_phone", "by_name" };
        vector<string> retrieveCommandsRu = {
                "фамилию абонента если ввести его телефон",
                "список всех телефонов по фамилии абонента" };

        cout << "Получить данные возможно одним из способов: ";
        outputListToStream(std::cout, retrieveCommandsRu);
        auto retrieveCommandIndex = selectFromList(retrieveCommands);

        if (retrieveCommands[retrieveCommandIndex] == "by_phone") {
            cout << "MENU/RETRIEVE BY PHONE:" << endl;
            // Вообще исключаем введение неверных ключей
            vector<string> keys;
            keys.reserve(phonebook.size());
            for (const auto &[key, value] : phonebook) keys.emplace_back(key);
            cout << "Введите один из телефонов, который собираетесь получить:" << endl;
            string selectedKey = keys[selectFromList(keys)];

            string valueByKey;
            retrieveMapValueByKeyExtended(valueByKey, selectedKey, phonebook);

            cout << "Результат извлечения по телефону " << selectedKey << ": " << valueByKey << endl;

            cout << "Закончили извлечение?" << endl;
            if (selectFromList({ "yes", "no" }) == 0) break;
        } else {
            cout << "MENU/RETRIEVE PHONES BY NAME:" << endl;
            vector<string> listOfPhones;
            string subscriber = getUserLineString("Введите фамилию абонента");
            int foundPhonesAmount = retrieveMapKeysByValueExtended(listOfPhones, subscriber, phonebook);

            if (foundPhonesAmount > 0) {
                cout << "По ключу " << subscriber << " найдено " << foundPhonesAmount << " записей:" << endl;
                outputListToStream(std::cout, listOfPhones);
            }

            cout << "Закончили извлечение?" << endl;
            if (selectFromList({ "yes", "no" }) == 0) break;
        }
    }
}

void menuEdit(std::map<string, string> &phonebook) {
    while (true) {
        std::pair<string, string> newEntry;

        // Вообще исключаем введение неверных ключей
        vector<string> keys;
        keys.reserve(phonebook.size());
        for (const auto &[key, value] : phonebook) keys.emplace_back(key);
        cout << "Введите один из телефонов, который собираетесь редактировать:" << endl;
        newEntry.first = keys[selectFromList(keys)];
        newEntry.second = getUserLineString("Введите новую фамилию абонента");

        changeEntryInMapExtended(newEntry, phonebook);

        cout << "Закончили редактирование?" << endl;
        if (selectFromList({ "yes", "no" }) == 0) break;
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::map<string, string> phonebook;

    vector<string> commands = { "add", "edit", "remove", "retrieve", "about", "exit"};
    vector<string> commandsRu = {
            "добавить",
            "изменить",
            "удалить",
            "получить (владельца или список телефонов владельца)",
            "инфо",
            "выход"};

    while(true) {
        cout << "MENU:" << endl;
        cout << "С записями возможны операции: ";
        outputListToStream(std::cout, commandsRu);
        auto index = selectFromList(commands);

        if (commands[index] == "add") {
            cout << "MENU/ADD:" << endl;
            menuAdd(phonebook);
        }
        else if (commands[index] == "edit") {
            cout << "MENU/EDIT:" << endl;
            if (abortIfDataEmpty(opType::edit, phonebook)) continue;
            menuEdit(phonebook);
        }
        else if (commands[index] == "remove") {
            cout << "MENU/REMOVE:" << endl;
            if (abortIfDataEmpty(opType::remove, phonebook)) continue;
            menuRemove(phonebook);
        }
        else if (commands[index] == "retrieve") {
            cout << "MENU/RETRIEVE:" << endl;
            if (abortIfDataEmpty(opType::retrieve, phonebook)) continue;
            menuRetrieve(phonebook);
        }
        else if (commands[index] == "about") {
            if (!phonebook.empty()) displayEntries(phonebook);
            else cout << "Телефонный справочник пока пуст" << endl;
        }
        else if (commands[index] == "exit") {
            cout << "Программа завершает свою работу. До новых встреч" << endl;
            break;
        }
    }
}
