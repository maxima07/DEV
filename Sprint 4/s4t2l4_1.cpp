#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>

std::set<int>::const_iterator FindNearestElement(const std::set<int>& numbers, int border) {
    // set<int>::const_iterator — тип итераторов для константного множества целых чисел
    if(numbers.empty()){
        return numbers.end();
    }

    if (border <= *numbers.begin()){
        return numbers.begin();
    }

    if (border >= *prev(numbers.end(), 1)){
        return prev(numbers.end());
    }
    
    auto it = numbers.lower_bound(border);
    
    if(border == *it){
        return it;
    }
    if(border - *std::prev(it, 1) <= *it - border){
        return std::prev(it, 1);
    }
    return it;
}

int main() {
    std::set<int> numbers = {1, 4, 6};
    std::cout << *FindNearestElement(numbers, 0) << " " << *FindNearestElement(numbers, 3) << " "
         << *FindNearestElement(numbers, 5) << " " << *FindNearestElement(numbers, 6) << " "
         << *FindNearestElement(numbers, 100) << std::endl;
    std::set<int> empty_set;
    std::cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << std::endl;
    return 0;
} 