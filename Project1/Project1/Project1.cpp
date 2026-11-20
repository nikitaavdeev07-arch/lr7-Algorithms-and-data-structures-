// Project1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <algorithm>
using namespace std;
using namespace chrono;

// Счётчики для анализа 
long long comparisons = 0;
long long swaps_count = 0;

void resetCounters() { comparisons = 0; swaps_count = 0; }

void swapEl(int& a, int& b) {
    int t = a; a = b; b = t;
    swaps_count++;
}

// ==================== 1. Опорный элемент — последний ====================
int partitionLast(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        comparisons++;
        if (arr[j] <= pivot) {
            i++;
            swapEl(arr[i], arr[j]);
        }
    }
    swapEl(arr[i + 1], arr[high]);
    return i + 1;
}

// ==================== 2. Опорный элемент — первый ====================
int partitionFirst(int arr[], int low, int high) {
    swapEl(arr[low], arr[high]); // переставляем первый в конец
    return partitionLast(arr, low, high);
}

// ==================== 3. Опорный элемент — медиана трёх ====================
int medianOfThree(int arr[], int low, int high) {
    int mid = low + (high - low) / 2;
    // Сортируем три элемента: arr[low], arr[mid], arr[high]
    if (arr[low] > arr[mid]) swapEl(arr[low], arr[mid]);
    if (arr[low] > arr[high]) swapEl(arr[low], arr[high]);
    if (arr[mid] > arr[high]) swapEl(arr[mid], arr[high]);
    // Ставим медиану в конец (в позицию high)
    swapEl(arr[mid], arr[high]);
    return arr[high];
}

int partitionMedian(int arr[], int low, int high) {
    medianOfThree(arr, low, high);
    return partitionLast(arr, low, high);
}

// ==================== Обёртки QuickSort ====================
void quickSortLast(int arr[], int low, int high) {
    if (low < high) {
        int pi = partitionLast(arr, low, high);
        quickSortLast(arr, low, pi - 1);
        quickSortLast(arr, pi + 1, high);
    }
}

void quickSortFirst(int arr[], int low, int high) {
    if (low < high) {
        int pi = partitionFirst(arr, low, high);
        quickSortFirst(arr, low, pi - 1);
        quickSortFirst(arr, pi + 1, high);
    }
}

void quickSortMedian(int arr[], int low, int high) {
    if (low < high) {
        int pi = partitionMedian(arr, low, high);
        quickSortMedian(arr, low, pi - 1);
        quickSortMedian(arr, pi + 1, high);
    }
}

// ==================== Генераторы данных ====================
void fillRandom(int arr[], int n) {
    for (int i = 0; i < n; i++) arr[i] = rand() % (n * 10);
}

void fillSorted(int arr[], int n) {
    for (int i = 0; i < n; i++) arr[i] = i;
}

void fillReverse(int arr[], int n) {
    for (int i = 0; i < n; i++) arr[i] = n - i;
}

void fillNearlySorted(int arr[], int n) {
    fillSorted(arr, n);
    // Немного перемешиваем: меняем ~5% элементов
    int swaps = n / 20;
    for (int k = 0; k < swaps; k++) {
        int i = rand() % n;
        int j = rand() % n;
        swap(arr[i], arr[j]);
    }
}

// ==================== Тестирование одного случая ====================
struct Result {
    double timeMs;
    long long comps;
    long long swps;
};

Result runTest(void (*sortFunc)(int[], int, int), int arr[], int n) {
    int* copy = new int[n];
    for (int i = 0; i < n; i++) copy[i] = arr[i];

    resetCounters();
    auto start = high_resolution_clock::now();
    sortFunc(copy, 0, n - 1);
    auto end = high_resolution_clock::now();

    Result r;
    r.timeMs = duration_cast<microseconds>(end - start).count() / 1000.0;
    r.comps = comparisons;
    r.swps = swaps_count;

    delete[] copy;
    return r;
}

void printHeader() {
    cout << "--------------------------------------------------------------" << endl;
    cout << "Тип данных      | Алгоритм          | Время(мс) | Сравнений | Обменов" << endl;
    cout << "--------------------------------------------------------------" << endl;
}

void runAll(const char* name, int base[], int n) {
    cout << name << endl;
    Result r1 = runTest(quickSortLast, base, n);
    Result r2 = runTest(quickSortFirst, base, n);
    Result r3 = runTest(quickSortMedian, base, n);

    cout << "                | Pivot = последний | "
        << r1.timeMs << "      | " << r1.comps << "    | " << r1.swps << endl;
    cout << "                | Pivot = первый    | "
        << r2.timeMs << "      | " << r2.comps << "    | " << r2.swps << endl;
    cout << "                | Pivot = медиана 3 | "
        << r3.timeMs << "      | " << r3.comps << "    | " << r3.swps << endl;
    cout << "--------------------------------------------------------------" << endl;
}

int main() {
    srand(42);
    const int N = 10000;
    int* base = new int[N];

    cout << "=== Сравнение 3 вариантов выбора опорного элемента (QuickSort) ===" << endl;
    cout << "Размер массива: N = " << N << endl << endl;
    printHeader();

    // 1. Случайные данные
    fillRandom(base, N);
    runAll("Случайный массив:", base, N);

    // 2. Отсортированный массив (худший случай для pivot=последний/первый)
    fillSorted(base, N);
    runAll("Отсортированный массив:", base, N);

    // 3. Обратно отсортированный массив
    fillReverse(base, N);
    runAll("Обратно отсортированный массив:", base, N);

    // 4. Почти отсортированный массив
    fillNearlySorted(base, N);
    runAll("Почти отсортированный массив:", base, N);

    delete[] base;
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
