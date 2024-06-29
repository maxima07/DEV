#pragma once

#include "array_ptr.h"

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

// Класс-обертка для конструктора SimpleVector с использованием Reserve
class ReserveProxyObj {
public:
    ReserveProxyObj(const size_t capacity_to_reserve) 
        : capacity_to_reserve_(capacity_to_reserve){
    }

    size_t GetCapacity() {
        return capacity_to_reserve_;
    }

private:
    size_t capacity_to_reserve_ = 0;
};

template <typename Type>
class SimpleVector {
public:

    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(std::size_t size) 
        : SimpleVector(size, Type{}){
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(std::size_t size, const Type& value)
    
        : size_(size), capacity_(size) {
            ArrayPtr<Type> tmp(size);
            std::fill(tmp.Get(), tmp.Get() + size, value);
            tmp.swap(simple_vector_);
    }

    SimpleVector(std::size_t size, Type&& value)
        : size_(size), capacity_(size) {
            ArrayPtr<Type> tmp(size);
            
            for(auto it = tmp.Get(); it != tmp.Get() + size; ++it){
                *it = value;
            }

            tmp.swap(simple_vector_);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : simple_vector_(init.size())
        , size_(init.size())
        , capacity_(init.size())
    {
        ArrayPtr<Type> tmp(init.size());
        
        //std::copy(init.begin(), init.end(), tmp.Get());
        std::move(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), tmp.Get());

        tmp.swap(simple_vector_);
    }

    // Конструктор копирования
    SimpleVector(const SimpleVector& other)
        : simple_vector_(other.size_)
        , size_(other.size_)
        , capacity_(other.capacity_){
            std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector(SimpleVector&& other) 
        : simple_vector_(other.size_)
        , size_(std::move(other.size_))
        , capacity_(std::move(other.capacity_)){
            //std::move(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()), begin());
            other.Clear();
    }

    // Конструктор с использованием класса-обертки для искобчения конфликта конструкторов, и
    // спользующих в качестве аргумента size
    SimpleVector(ReserveProxyObj obj){
        Reserve(obj.GetCapacity());
    }

    //Оператор присваивания. 
    //Должен обеспечивать строгую гарантию безопасности исключений.
    SimpleVector& operator=(const SimpleVector& rhs) {
        if(this != &rhs){
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        if(this != &rhs){
            SimpleVector tmp(rhs.size_);
            std::move(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()), tmp.begin());
            swap(tmp);
        }   
        return *this;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return simple_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return simple_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_){
            throw std::out_of_range("Error: going beyond borders");
        }
        return simple_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_){
            throw std::out_of_range("Error: going beyond borders");
        }
        return simple_vector_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        Resize(0);
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if(new_size < size_){
            size_ = new_size;
        } else if (new_size <= capacity_) {
            //std::fill(simple_vector_.Get() + size_, simple_vector_.Get() + new_size, Type{});

            for(auto it = simple_vector_.Get() + size_; it != simple_vector_.Get() + new_size; ++it){
                *it = Type{};
            }

            size_ = new_size;
        } else {
            ArrayPtr<Type> tmp(new_size * 2);
            //std::copy(begin(), end(), tmp.Get());
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp.Get());

            //std::fill(tmp.Get() + size_, tmp.Get() + size_ + new_size, Type{});
            for(auto it = tmp.Get() + size_; it != tmp.Get() + size_ + new_size; ++it){
                *it = Type{};
            }

            size_ = new_size;
            capacity_ = new_size;
            simple_vector_.swap(tmp);
        }
    }

    void Reserve(size_t new_capacity){
        if(new_capacity > capacity_){
            ArrayPtr<Type> tmp(new_capacity);
            
            //std::copy(begin(), end(), tmp.Get());
            std::move(std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp.Get());

            simple_vector_.swap(tmp);
            capacity_ = new_capacity;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return simple_vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return simple_vector_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return simple_vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return simple_vector_.Get() + size_;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if(size_ < capacity_){
            simple_vector_[size_] = item;
            ++size_;
        } else if (capacity_ == 0){
            ArrayPtr<Type> tmp(1);
            tmp[0] = item;
            simple_vector_.swap(tmp);
            ++size_;
            ++capacity_;
        } else {
            ArrayPtr<Type> tmp(capacity_ * 2);
            std::copy(begin(), end(), tmp.Get());
            tmp[size_] = item;
            simple_vector_.swap(tmp);
            ++size_;
            capacity_ *= 2;
        }
    }

    void PushBack(Type&& item) {
        if(size_ < capacity_){
            simple_vector_[size_] = std::move(item);
            ++size_;
        } else if (capacity_ == 0){
            ArrayPtr<Type> tmp(1);
            std::move(begin(), end(), &tmp[0]); 
            //tmp[0] = std::move(item);
            simple_vector_.swap(tmp);
            ++size_;
            ++capacity_;
        } else {
            ArrayPtr<Type> tmp(capacity_ *= 2);
            //std::move(std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp.Get());
            std::move(begin(), end(), &tmp[0]);
            tmp[size_] = std::move(item);
            simple_vector_.swap(tmp);
            ++size_;
            //capacity_ *= 2;
        }
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t new_pos = static_cast<size_t>(std::distance(cbegin(), pos));
        if(size_ < capacity_){
            std::copy_backward(begin() + new_pos, end(), end() + 1);
            simple_vector_[new_pos] = value;
            ++size_;
            return &simple_vector_[new_pos];
        } else if (capacity_ == 0) {
            ArrayPtr<Type> tmp(1);
            tmp[0] = value;          
            simple_vector_.swap(tmp);
            ++size_;
            ++capacity_;
            return &simple_vector_[new_pos];
        } else {
            ArrayPtr<Type> tmp(capacity_ * 2);
            std::copy(begin(), begin() + new_pos, tmp.Get());
            std::copy(begin() + new_pos, end(), tmp.Get() + new_pos + 1);
            tmp[new_pos] = value;          
            simple_vector_.swap(tmp);
            ++size_;
            capacity_ *= 2;
            return &simple_vector_[new_pos];
        }
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t new_pos = static_cast<size_t>(std::distance(cbegin(), pos));
        if(size_ < capacity_){
            std::move(std::make_move_iterator(begin() + new_pos), std::make_move_iterator(end()), end() + 1);
            simple_vector_[new_pos] = std::move(value);
            ++size_;
            return &simple_vector_[new_pos];
        } else if (capacity_ == 0) {
            ArrayPtr<Type> tmp(1);
            tmp[0] = std::move(value);
            simple_vector_.swap(tmp);
            ++size_;
            ++capacity_;
            return &simple_vector_[new_pos];
        } else {
            ArrayPtr<Type> tmp(capacity_ * 2);
            std::move(std::make_move_iterator(begin()), std::make_move_iterator(begin() + new_pos), tmp.Get());
            std::move(std::make_move_iterator(begin() + new_pos), std::make_move_iterator(end()), tmp.Get() + new_pos + 1);
            tmp[new_pos] = std::move(value);
            simple_vector_.swap(tmp);
            ++size_;
            capacity_ *= 2;
            return &simple_vector_[new_pos];
        }
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if(!IsEmpty()){
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        if(!IsEmpty()){
            // Iterator tmp_pos = const_cast<Iterator>(pos);
            Iterator tmp_pos = const_cast<Iterator>(pos);
            
            //std::copy(tmp_pos + 1, end(), tmp_pos);
            std::move(std::make_move_iterator(tmp_pos + 1), std::make_move_iterator(end()), tmp_pos);

            --size_;
            return tmp_pos;
        }
        return nullptr;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        simple_vector_.swap(other.simple_vector_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void swap(SimpleVector&& other) noexcept {
        simple_vector_.swap(other.simple_vector_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    ArrayPtr<Type> simple_vector_;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {  
    if(std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end())){
            return true;
        }
    
    return false;
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());;
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}; 