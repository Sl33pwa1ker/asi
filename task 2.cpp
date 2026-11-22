#include <algorithm>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <numeric>
#include <thread>
#include <vector>

template <typename Iterator, typename Function>
void parallel_for_each_impl(Iterator first, Iterator last,
                            Function& function, unsigned available_tasks) {
    const auto length = std::distance(first, last);

    // Маленький блок обрабатываем в текущем потоке.
    if (length <= 1000 || available_tasks <= 1) {
        std::for_each(first, last, std::ref(function));
        return;
    }

    auto middle = std::next(first, length / 2);
    const unsigned left_tasks = available_tasks / 2;
    const unsigned right_tasks = available_tasks - left_tasks;

    // Левую часть запускаем асинхронно, правую делим рекурсивно.
    auto left_task = std::async(std::launch::async,
        [first, middle, &function, left_tasks] {
            parallel_for_each_impl(first, middle, function, left_tasks);
        });

    parallel_for_each_impl(middle, last, function, right_tasks);
    left_task.get();
}

template <typename Iterator, typename Function>
void parallel_for_each(Iterator first, Iterator last, Function& function) {
    unsigned available_tasks = std::thread::hardware_concurrency();
    if (available_tasks == 0) {
        available_tasks = 2;
    }
    parallel_for_each_impl(first, last, function, available_tasks);
}

int main() {
    std::vector<int> numbers(10000);
    std::iota(numbers.begin(), numbers.end(), 1);

    auto multiply_by_two = [](int& number) { number *= 2; };
    parallel_for_each(numbers.begin(), numbers.end(), multiply_by_two);

    std::cout << "First five numbers: ";
    for (std::size_t i = 0; i < 5; ++i) {
        std::cout << numbers[i] << ' ';
    }
    std::cout << "\nLast number: " << numbers.back() << '\n';
}
