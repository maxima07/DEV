#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Stack {
public:
    auto begin(){
        return elements_.begin();
    }

    auto end(){
        return elements_.end();
    }

    void Push(const Type& element) {
        // напишите реализацию
        auto it = lower_bound(begin(), end(), element);
        if(IsEmpty()){
            elements_.push_back(element);
        } else {
            elements_.insert(it, element); 
        }
        
    }

    void Pop() {
        // напишите реализацию
        elements_.pop_back();
    }
    const Type& Peek() const {
        // напишите реализацию
        return elements_.back();
    }
    Type& Peek() {
        // напишите реализацию
        return elements_.back();
    }
    void Print() const {
        // напишите реализацию
        PrintRange(elements_.begin(), elements_.end());
    }
    uint64_t Size() const {
        // напишите реализацию
        return elements_.size();
    }
    bool IsEmpty() const {
        // напишите реализацию
        return elements_.empty();
    }

private:
    vector<Type> elements_;
};

template <typename Type>
class StackMin {
public:
    void Push(const Type& element) {
    // напишите реализацию метода
        elements_.Push(element);
        min_elements_[element].push_back(elements_.Peek());
    }

    void Pop() {
    // напишите реализацию метода

        min_elements_[elements_.Peek()].pop_back();
        if(min_elements_[elements_.Peek()].size() == 0){
            min_elements_.erase(elements_.Peek());
        }
        elements_.Pop();
    }

    const Type& Peek() const {
        return elements_.Peek();
    }
    Type& Peek() {
        return elements_.Peek();
    }
    void Print() const {
        elements_.Print();
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        return elements_.IsEmpty();
    }
    const Type& PeekMin() const {
    // напишите реализацию метода
        const auto min_elem = *min_elements_.begin();
        return min_elements_.at(min_elem.first).front();
    }
    Type& PeekMin() {
    // напишите реализацию метода
        const auto min_elem = *min_elements_.begin();
        return min_elements_.at(min_elem.first).front();
    }
private:
    Stack<Type> elements_;
    map<Type, vector<Type>> min_elements_;
}; 

template <typename Type>
class SortedSack {
public:
    void Push(const Type& element) {
    // напишите реализацию метода
        elements_.Push(element);
    }


    void Pop() {
    elements_.Pop();
    }

    const Type& Peek() const {
        return elements_.Peek();
    }

    Type& Peek() {
        return elements_.Peek();
    }

    void Print() const {
        elements_.Print();
    }

    uint64_t Size() const {
        return elements_.Size();
    }

    bool IsEmpty() const {
        return elements_.IsEmpty();
    }
private:
    Stack<Type> elements_;
}; 

int main() {
    SortedSack<int> sack;
    vector<int> values(5);
    // заполняем вектор для тестирования нашего класса
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);

    // заполняем класс и проверяем, что сортировка сохраняется после каждой вставки
    for (int i = 0; i < 5; ++i) {
        cout << "Вставляемый элемент = "s << values[i] << endl;
        sack.Push(values[i]);
        sack.Print();
    }
}