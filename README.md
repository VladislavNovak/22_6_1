## <font color="tomato">Телефонный справочник</font>

На вход программе поступают запросы трёх типов:

69-70-30 Ivanov — добавить телефон и фамилию абонента в справочник

69-70-30 — узнать фамилию абонента по номеру телефона

Ivanov — узнать телефон абонента по фамилии 

Необходимо выводить в консоль ответ на запросы 2 и 3. 
Обратите внимание, что абоненты по разным номерам могут иметь одинаковые фамилии. 
В таком случае на запрос 3 необходимо выдать все номера через пробелы.

Операции запросов должны работать за `O(logn)` или `O(logn × количество номеров с этой фамилией)`.

## <font color="tomato">Полезные ресурсы</font>

[Словарь std::map METANIT.COM](https://metanit.com/cpp/tutorial/7.14.php)

[Map в C++: что это и как с этим работать Code Lessons](https://codelessons.ru/cplusplus/map-v-c-chto-eto-i-kak-s-etim-rabotat.html)

## <font color="tomato">Примечания</font>

Создано два словаря:

- std::map<string, string> phonebook. Фактически это структура { телефон: фамилия }
- std::map<string, vector<string>> notebook. Фактически это структура { фамилия: { телефоны } }

Соответственно, в каждой из операций ("add", "edit", "remove", "retrieve") происходит изменение сразу двух словарей.

Несколько объяснений по функциям, которые обрабатывают notebook:

Из `addEntryToComplexMap` исключены любые проверки. 
Это сделано для того, чтобы не дублировать некоторые процедуры из предшествующей `addEntryToMap`. 
Таким образом, выполнение `addEntryToComplexMap` возможно лишь в случае успешного выполнения `addEntryToMap`:

```c++
// Важно: отработает только если до того успешно отработает addEntryToMap
template<typename F, typename S>
void addEntryToComplexMap(const std::pair<F, S> &entry, std::map<S, vector<F>> &target) {
    // где entry это (123: Ivanov), target это (Ivanov: { 123, 456 })
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
```
Тоже самое относится к `removeEntryFromComplexMap`, которая связана с `removeEntryFromMap`:

```c++
// Важно: отработает только если до того успешно отработает removeEntryFromMap
template<typename F, typename S>
void removeEntryFromComplexMap(const std::pair<F, S> &entry, std::map<S, vector<F>> &target) {
    // Нашли запись по фамилии (entry.second)
    auto it = target.find(entry.second);
    // Если массив target.values имеет более одного значения, удаляем из него значение по ключу
    if (it->second.size() > 1) removeKeyFromVector(entry.first, it->second);
    // В противном случае (если в target.values) это одно значение и всю запись из target можно полностью удалить
    else target.erase(it);
}
```
Тоже самое относится к `changeEntryInComplexMap`, которая связана с `changeEntryInMap`:

```c++
// Важно: отработает только если до того успешно отработает changeEntryInMap
template<typename F, typename S>
void changeEntryInComplexMap(const std::pair<F, S> &oldEntry, const std::pair<F, S> &newEntry, std::map<S, vector<F>> &target) {
    removeEntryFromComplexMap(oldEntry, target);
    addEntryToComplexMap(newEntry, target);
}
```
Итогом работы будут функции `retrieveMapValueByKey` и `retrieveComplexMapValueByKey`, которые 
выводят результат в соответствии с алгоритмом `O(logn)`
