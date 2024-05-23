#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std::literals;

std::vector<int> ReverseVector(const std::vector<int>& source_vector) {
    std::vector<int> res;
    for (int i : source_vector) {
        res.insert(res.begin(), i);
    }

    return res;
}

int CountPops(const std::vector<int>& source_vector, int begin, int end) {
    int res = 0;

    for (int i = begin; i < end; ++i) {
        if (source_vector[i]) {
            ++res;
        }
    }

    return res;
}

void AppendRandom(std::vector<int>& v, int n) {
    for (int i = 0; i < n; ++i) {
        // получаем случайное число с помощью функции rand.
        // с помощью (rand() % 2) получим целое число в диапазоне 0..1.
        // в C++ имеются более современные генераторы случайных чисел,
        // но в данном уроке не будем их касаться
        v.push_back(rand() % 2);
    }
}

void Operate() {
    std::vector<int> random_bits;

    // операция << для целых чисел это сдвиг всех бит в двоичной
    // записи числа. Запишем с её помощью число 2 в степени 17 (131072)
    static const int N = 1 << 17;
     
    const auto start_time_append_random = std::chrono::steady_clock::now(); 
    // заполним вектор случайными числами 0 и 1
    AppendRandom(random_bits, N);
    const auto end_time_append_random = std::chrono::steady_clock::now();
    const auto duration_append_random = end_time_append_random - start_time_append_random;

    std::cerr << "Append random: "s << std::chrono::duration_cast<std::chrono::microseconds>(duration_append_random).count() << " ms"s << std::endl;
    
    const auto start_time_reverse = std::chrono::steady_clock::now();
    // перевернём вектор задом наперёд
    std::vector<int> reversed_bits = ReverseVector(random_bits);
    const auto end_time_reverse = std::chrono::steady_clock::now();
    const auto duration_reverse = end_time_reverse - start_time_reverse;

    std::cerr << "Reverse: "s << std::chrono::duration_cast<std::chrono::microseconds>(duration_reverse).count() << " ms"s << std::endl;
    
    const auto start_time_rate = std::chrono::steady_clock::now();
    // посчитаем процент единиц на начальных отрезках вектора
    for (int i = 1, step = 1; i <= N; i += step, step *= 2) {
        // чтобы вычислить проценты, мы умножаем на литерал 100. типа double;
        // целочисленное значение функции CountPops при этом автоматически
        // преобразуется к double, как и i
        
        double rate = CountPops(reversed_bits, 0, i) * 100. / i;
        std::cout << "After "s << i << " bits we found "s << rate << "% pops"s
             << std::endl;
    }
    const auto end_time_rate = std::chrono::steady_clock::now();
    const auto duration_rate = end_time_rate - start_time_rate;  
    std::cerr << "Counting: "s << std::chrono::duration_cast<std::chrono::microseconds>(duration_rate).count() << " ms"s << std::endl;
}

int main() {
    Operate();
    return 0;
}