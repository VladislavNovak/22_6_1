#include <windows.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

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

template<typename T>
int findKeyIndexInVector(const T &key, std::vector<T> const &list) {
    const int NOT_FOUND = -1;
    auto it = std::find_if(list.cbegin(), list.cend(), [key](const T &i){ return i == key; });

    if (it != list.cend()) {
        return (int)std::distance(list.cbegin(), it);
    }

    return NOT_FOUND;
}

template<typename T>
bool removeKeyFromVector(const T &key, vector<T> &list) {
    auto foundIndex = findKeyIndexInVector(key, list);
    if (foundIndex == -1) return false;

    list.erase(list.begin() + foundIndex);

    return true;
}

// simplified
void outputListToStream(std::ostream &out, std::vector<std::string> const &list, const std::string &delim = ",") {
    for (int i = 0; i < list.size(); ++i) out << list[i] << (i != list.size() - 1 ? delim : "\n");
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

template<typename F, typename S>
vector<F> getMapKeys(const std::map<F, S> &source) {
    vector<F> keys;
    keys.reserve(source.size());
    for (const auto &[key, _] : source) keys.emplace_back(key);

    return keys;
}

// Добавить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool addEntryToMap(const std::pair<F, S> &entry, std::map<F, S> &target) {
    if (target.count(entry.first) == 1) return false;

    target.insert(entry);

    return true;
}

template<typename F, typename S>
void addEntryToComplexMap(const std::pair<F, S> &entry, std::map<S, vector<F>> &target) {
    // Нашли запись по фамилии (entry.second)
    auto it = target.find(entry.second);
    // Если записи пока нет - добавляем новую запись из фамилии (entry.second) и телефона в vector {entry.first}
    if (it == target.end()) {
        vector<S> values = { entry.first };
        target.insert(std::make_pair(entry.second, values));
    }
    // Если же запись есть - добавляем в vector новый телефон
    else it->second.emplace_back(entry.first);
}

// Удалить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool removeEntryFromMap(const F &key, std::map<F, S> &target) {
    const auto it = target.find(key);

    if (it == target.end()) return false;

    target.erase(it);

    return true;
}

template<typename F, typename S>
void removeEntryFromComplexMap(const std::pair<F, S> &entry, std::map<S, vector<F>> &target) {
    // Нашли запись по фамилии (entry.second)
    auto it = target.find(entry.second);
    // Если массив target.values имеет более одного значения, удаляем из него значение по ключу
    if (it->second.size() > 1) removeKeyFromVector(entry.first, it->second);
    // В противном случае (если в target.values) это одно значение и всю запись из target можно полностью удалить
    else target.erase(it);
}

// Вернуть одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool retrieveValueByKeyFromMap(S &target, const F &key, const std::map<F, S> &source) {
    const auto it = source.find(key);

    if (it == source.end()) return false;

    target = it->second;

    return true;
}

// Вернуть список значений по запрашиваемому значение. Возвращает количество обнаруженных совпадений
template<typename F, typename S>
int retrieveValuesByKeyFromComplexMap(vector<F> &target, const S &key, const std::map<S, vector<F>> &source) {
    int amount = 0;
    auto it = source.find(key);

    if (it == source.end()) return amount;

    target = it->second;

    return target.size();
}

// Изменить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool changeEntryInMap(const std::pair<F, S> &entry, std::map<F, S> &target) {
    if (target.count(entry.first) == 0) return false;

    auto it = target.find(entry.first);
    it->second = entry.second;

    return true;
}

template<typename F, typename S>
void changeEntryInComplexMap(const std::pair<F, S> &oldEntry, const std::pair<F, S> &newEntry, std::map<S, vector<F>> &target) {
    removeEntryFromComplexMap(oldEntry, target);
    addEntryToComplexMap(newEntry, target);
}

// Выводит отчет по записям
void displayMap(const std::map<string, string> &source) {
    cout << "Отчет (телефон: абонент) :" << endl;
    for (const auto &[key, value] : source) cout << key << ": " << value << endl;
}

void displayComplexMap(const std::map<string, vector<string>> &source) {
    cout << "Отчет (абонент: список телефонов) :" << endl;
    for (const auto &[key, values] : source) {
        cout << key << ": ";
        outputListToStream(std::cout, values);
    }
}

// В зависимости от операции выводит на дисплей результат
void printLog(opType mode, bool status, const string &keyword, int itemsAmount = 0) {
    vector<string> operationNames = { "Добавление", "Изменение", "Удаление", "Извлечение" };
    string operation = operationNames[static_cast<int>(mode)];
    const char* msg = "LOG:                  %s по ключу %s %s";
    const char* onSuccess = "прошло успешно.\n";
    string onSuccessExtended = "прошло успешно. Количество обработанных записей: " + std::to_string(itemsAmount) + "\n";
    vector<string> causes = { "такая запись уже существует", "такой записи не существует"};
    string onFailure = "не удалось, т.к. " + ((mode == opType::add) ? causes[0] : causes[1]) + "\n";

    if (status) {
        if (itemsAmount == 0) printf(msg, operation.c_str(), keyword.c_str(), onSuccess);
        else printf(msg, operation.c_str(), keyword.c_str(), onSuccessExtended.c_str());
    }
    else printf(msg, operation.c_str(), keyword.c_str(), onFailure.c_str());
}

template<typename F, typename S>
bool addEntryToPhonebook(const std::pair<F, S> &entry, std::map<F, S> &target) {
    auto status = addEntryToMap(entry, target);
    printLog(opType::add, status, entry.first);
    return status;
}

template<typename F, typename S>
bool changeEntryInPhonebook(const std::pair<F, S> &entry, std::map<F, S> &target) {
    auto status = changeEntryInMap(entry, target);
    printLog(opType::edit, status, entry.first);
    return status;
}

template<typename F, typename S>
bool removeEntryFromPhonebook(const F &key, std::map<F, S> &target) {
    auto status = removeEntryFromMap(key, target);
    printLog(opType::remove, status, key);
    return status;
}

template<typename F, typename S>
bool retrieveValueByKeyFromPhonebook(S &target, const F &key, const std::map<F, S> &source) {
    auto status = retrieveValueByKeyFromMap(target, key, source);
    printLog(opType::retrieve, status, key);
    return status;
}

template<typename F, typename S>
int retrieveValuesByKeyFromNotebook(vector<F> &target, const S &key, const std::map<S, vector<F>> &source) {
    int itemsAmount = retrieveValuesByKeyFromComplexMap(target, key, source);
    printLog(opType::retrieve, itemsAmount > 0, key, itemsAmount);
    return itemsAmount;
}

bool abortIfDataEmpty(opType mode, const std::map<string, string> &phonebook, const std::map<string, vector<string>> &notebook) {
    vector<string> operationNames = { "добавлением", "изменением", "удалением", "извлечением" };
    const char* operation = operationNames[static_cast<int>(mode)].c_str();

    if (!phonebook.empty()) {
        cout << "Перед " << operation << " показать весь список?" << endl;
        if (selectFromList({ "yes", "no" }) == 0) {
            displayMap(phonebook);
            displayComplexMap(notebook);
        }
        return false;
    } else {
        cout << "Телефонный справочник пока пуст" << endl;
        return true;
    }
}

void menuAdd(std::map<string, string> &phonebook, std::map<string, vector<string>> &notebook) {
    while (true) {
        string selectedPhone = getUserLineString("Введите телефон (простая строка)");
        string newName = getUserLineString("Введите фамилию абонента");
        std::pair<string, string> newEntry(selectedPhone, newName);

        bool isSuccess = addEntryToPhonebook(newEntry, phonebook);
        // Если запись добавилась, вносим изменения в параллельный список
        if (isSuccess) addEntryToComplexMap(newEntry, notebook);

        cout << "Закончили добавление?" << endl;
        if (selectFromList({ "yes", "no" }) == 0) break;
    }
}

void menuRemove(std::map<string, string> &phonebook, std::map<string, vector<string>> &notebook) {
    while (true) {
        // Вообще исключаем введение неверных ключей
        vector<string> keys = getMapKeys(phonebook);
        cout << "Введите один из телефонов, который собираетесь удалить:" << endl;
        string selectedPhone = keys[selectFromList(keys)];

        string oldName;
        // Извлекаем и сохраняем старое значение
        bool isRetrieveSuccessfully = retrieveValueByKeyFromPhonebook(oldName, selectedPhone, phonebook);

        bool isRemoveSuccess = removeEntryFromPhonebook(selectedPhone, phonebook);
        if (isRemoveSuccess && isRetrieveSuccessfully) {
            std::pair<string, string> oldEntry(selectedPhone, oldName);
            removeEntryFromComplexMap(oldEntry, notebook);
        }

        cout << "Закончили удаление?" << endl;
        if (selectFromList({ "yes", "no" }) == 0) break;
    }
}

void menuRetrieve(std::map<string, string> &phonebook, std::map<string, vector<string>> &notebook) {
    while (true) {
        vector<string> retrieveCommands = { "by_phone", "by_name" };
        vector<string> retrieveCommandsRu = {
                "фамилию абонента если ввести его телефон",
                "список всех телефонов по фамилии абонента" };

        cout << "Получить данные возможно одним из способов: ";
        outputListToStream(std::cout, retrieveCommandsRu);
        auto retrieveCommandIndex = selectFromList(retrieveCommands);

        if (retrieveCommands[retrieveCommandIndex] == "by_phone") {
            cout << "MENU/RETRIEVE/BY PHONE:" << endl;
            // Вообще исключаем введение неверных ключей
            vector<string> keys = getMapKeys(phonebook);
            cout << "Введите один из телефонов, который собираетесь получить:" << endl;
            string selectedKey = keys[selectFromList(keys)];

            string retrievedValue;
            retrieveValueByKeyFromPhonebook(retrievedValue, selectedKey, phonebook);

            cout << "Результат извлечения по телефону " << selectedKey << ": " << retrievedValue << endl;

            cout << "Закончили извлечение?" << endl;
            if (selectFromList({ "yes", "no" }) == 0) break;
        } else {
            cout << "MENU/RETRIEVE/PHONES BY NAME:" << endl;
            vector<string> listOfPhones;
            string subscriber = getUserLineString("Введите фамилию абонента");
            int foundPhonesAmount = retrieveValuesByKeyFromNotebook(listOfPhones, subscriber, notebook);

            if (foundPhonesAmount > 0) {
                cout << "По ключу " << subscriber << " найдено " << foundPhonesAmount << " записей:" << endl;
                outputListToStream(std::cout, listOfPhones);
            }

            cout << "Закончили извлечение?" << endl;
            if (selectFromList({ "yes", "no" }) == 0) break;
        }
    }
}

void menuEdit(std::map<string, string> &phonebook, std::map<string, vector<string>> &notebook) {
    while (true) {
        // Вообще исключаем введение неверных ключей
        vector<string> keys = getMapKeys(phonebook);
        cout << "Введите один из телефонов, который собираетесь редактировать:" << endl;
        string selectedPhone = keys[selectFromList(keys)];
        string newName = getUserLineString("Введите новую фамилию абонента");

        string oldName;
        // Извлекаем и сохраняем старое значение
        bool isRetrieveSuccessfully = retrieveValueByKeyFromPhonebook(oldName, selectedPhone, phonebook);

        bool isChangeSuccessfully = changeEntryInPhonebook(std::make_pair(selectedPhone, newName), phonebook);
        // Если запись добавилась в phonebook, вносим изменения в параллельный список
        if (isChangeSuccessfully && isRetrieveSuccessfully) {
            std::pair<string, string> oldEntry(selectedPhone, oldName);
            std::pair<string, string> newEntry(selectedPhone, newName);
            changeEntryInComplexMap(oldEntry, newEntry, notebook);
        }

        cout << "Закончили редактирование?" << endl;
        if (selectFromList({ "yes", "no" }) == 0) break;
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    // 123-34: Ivanov
    std::map<string, string> phonebook;
    // Ivanov: { 123-34, 34-56 }
    std::map<string, vector<string>> notebook;

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
            menuAdd(phonebook, notebook);
        }
        else if (commands[index] == "edit") {
            cout << "MENU/EDIT:" << endl;
            if (abortIfDataEmpty(opType::edit, phonebook, notebook)) continue;
            menuEdit(phonebook, notebook);
        }
        else if (commands[index] == "remove") {
            cout << "MENU/REMOVE:" << endl;
            if (abortIfDataEmpty(opType::remove, phonebook, notebook)) continue;
            menuRemove(phonebook, notebook);
        }
        else if (commands[index] == "retrieve") {
            cout << "MENU/RETRIEVE:" << endl;
            if (abortIfDataEmpty(opType::retrieve, phonebook, notebook)) continue;
            menuRetrieve(phonebook, notebook);
        }
        else if (commands[index] == "about") {
            if (!phonebook.empty()) {
                displayMap(phonebook);
                displayComplexMap(notebook);
            }
            else cout << "Телефонный справочник пока пуст" << endl;
        }
        else if (commands[index] == "exit") {
            cout << "Программа завершает свою работу. До новых встреч" << endl;
            break;
        }
    }
}
