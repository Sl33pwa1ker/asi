#include <algorithm>
#include <future>
#include <iostream>
#include <utility>
#include <vector>

void selection_sort(std::vector<int>& numbers) {
    for (auto current = numbers.begin(); current != numbers.end(); ++current) {
        std::promise<std::vector<int>::iterator> min_promise;
        auto min_future = min_promise.get_future();

        // Поиск минимума выполняется в отдельной задаче.
        auto search_task = std::async(std::launch::async,
            [current, end = numbers.end(), min_promise = std::move(min_promise)]() mutable {
                min_promise.set_value(std::min_element(current, end));
            });

        // Получаем найденный элемент и ставим его на текущее место.
        auto min_position = min_future.get();
        search_task.get();
        std::iter_swap(current, min_position);
    }
}

int main() {
    std::vector<int> numbers{8, -3, 5, 1, 8, 0, -7, 2};

    std::cout << "Before: ";
    for (int number : numbers) {
        std::cout << number << ' ';
    }
    std::cout << '\n';

    selection_sort(numbers);

    std::cout << "After:  ";
    for (int number : numbers) {
        std::cout << number << ' ';
    }
    std::cout << '\n';
}
