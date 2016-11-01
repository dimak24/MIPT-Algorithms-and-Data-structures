#pragma once

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include "timsort.h"

bool comp(const int& a, const int& b) {
    return a > b;
}

void testOnRandomArrays(const int& tests_number = 1) {
    std::cout << "\nTests on random arrays:\n***********************\n";
    for (int k = 0; k < tests_number; ++k) {
        std::cout << "Test " << k << ":\n";
        srand(time(0));
        size_t sizes[] = {1 + rand() % 10, 50 + rand() % 50, 10000 + rand() % 10000, 100000 + 100000 % rand(), 1000000 + rand() % 1000000 };
        size_t sizes_number = 5;

        for (size_t i = 0; i < sizes_number; ++i) {
            std::cout << "n = " << sizes[i] << "\n";

            int* randomArrayForTim = new int[sizes[i]];
            int* randomArrayForTimWithComparator = new int[sizes[i]];
            int* randomArrayForStd = new int[sizes[i]];

            for (int j = 0; j < sizes[i]; ++j) {
                randomArrayForStd[j] = rand();
                randomArrayForTim[j] = randomArrayForStd[j];
                randomArrayForTimWithComparator[j] = randomArrayForStd[j];
            }

            time_t t = clock();
            std::sort(randomArrayForStd, randomArrayForStd + sizes[i]);
            std::cout << "std:sort:\n    time = " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n\n";

            t = clock();
            TimSort(randomArrayForTim, randomArrayForTim + sizes[i]);
            std::cout << "timSort:\n    time = " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";
            bool correct = true;
            for (int g = 0; g < sizes[i] && correct; ++g)
                if (randomArrayForStd[g] != randomArrayForTim[g])
                    correct = false;
            std::cout << (correct ? "correct\n" : "INCORRECT\n");

            t = clock();
            TimSort(randomArrayForTimWithComparator, randomArrayForTimWithComparator + sizes[i], comp);
            std::cout << "timSort (with comparator):\n    time = " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";
            correct = true;
            std::sort(randomArrayForStd, randomArrayForStd + sizes[i], comp);
            for (int g = 0; g < sizes[i] && correct; ++g)
                if (randomArrayForStd[g] != randomArrayForTimWithComparator[g])
                    correct = false;
            std::cout << (correct ? "correct\n\n" : "INCORRECT\n\n");

            delete[] randomArrayForStd;
            delete[] randomArrayForTim;
            delete[] randomArrayForTimWithComparator;
        }
    }
}

void testOnRandomVectors(const int& tests_number = 1) {
    std::cout << "\nTests on random vectors:\n***********************\n";
    for (int k = 0; k < tests_number; ++k) {
        std::cout << "Test " << k << ":\n";
        srand(time(0));
        size_t sizes[] = { 1 + rand() % 10, 50 + rand() % 50, 10000 + rand() % 10000, 100000 + 100000 % rand()};
        size_t sizes_number = 4;

        for (size_t i = 0; i < sizes_number; ++i) {
            std::cout << "n = " << sizes[i] << "\n";

            std::vector<int> randomArrayForTim(sizes[i]);
            std::vector<int> randomArrayForTimWithComparator(sizes[i]);
            std::vector<int> randomArrayForStd(sizes[i]);

            for (int j = 0; j < sizes[i]; ++j) {
                randomArrayForStd[j] = rand();
                randomArrayForTim[j] = randomArrayForStd[j];
                randomArrayForTimWithComparator[j] = randomArrayForStd[j];
            }

            time_t t = clock();
            std::sort(randomArrayForStd.begin(), randomArrayForStd.end());
            std::cout << "std:sort:\n    time = " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n\n";

            t = clock();
            TimSort(randomArrayForTim.begin(), randomArrayForTim.end());
            std::cout << "timSort:\n    time = " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";
            bool correct = true;
            for (int g = 0; g < sizes[i] && correct; ++g)
                if (randomArrayForStd[g] != randomArrayForTim[g])
                    correct = false;
            std::cout << (correct ? "correct\n" : "INCORRECT\n");

            t = clock();
            TimSort(randomArrayForTimWithComparator.begin(), randomArrayForTimWithComparator.end(), comp);
            std::cout << "timSort (with comparator):\n    time = " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";
            correct = true;
            std::sort(randomArrayForStd.begin(), randomArrayForStd.end(), comp);
            for (int g = 0; g < sizes[i] && correct; ++g)
                if (randomArrayForStd[g] != randomArrayForTimWithComparator[g])
                    correct = false;
            std::cout << (correct ? "correct\n\n" : "INCORRECT\n\n");
        }
    }
}

void testOnPartlySortedArrays(const int& tests_number = 1) {
    std::cout << "\nTests on partly sorted arrays:\n***********************\n";
    for (int k = 0; k < tests_number; ++k) {

        std::cout << "Test " << k << ":\n";

        srand(time(0));

        int sizes[] = { 20, 40, 80, 128, 1024 };
        int sizes_number = 5;
        int amounts[] = { 2, 4, 100, 1000 };
        int amounts_number = 4;

        for (int x = 0; x < sizes_number; ++x)
            for (int y = 0; y < amounts_number; ++y) {
                int size = sizes[x];
                int amount = amounts[y];
                int** a1 = new int*[amount];
                for (int j = 0; j < amount; ++j) {
                    a1[j] = new int[size];
                    for (int g = 0; g < size; ++g)
                        a1[j][g] = rand();
                    std::sort(a1[j], a1[j] + size);
                }
                int* forTim = new int[size * amount];
                int* forStd = new int[size * amount];
                int* forTimWithComp = new int[size * amount];
                for (int j = 0; j < size * amount; ++j) {
                    forTim[j] = a1[j / size][j % size];
                    forStd[j] = a1[j / size][j % size];
                    forTimWithComp[j] = a1[j / size][j % size];
                }

                std::cout << amount << " sorted arrays; " << "the size of each equals " << size << "\n";

                time_t t = clock();
                std::sort(forStd, forStd + size * amount);
                std::cout << "std::sort\n    time: " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";

                t = clock();
                TimSort(forTim, forTim + size * amount);
                std::cout << "TimSort\n    time: " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";
                bool correct = true;
                for (int g = 0; g < size * amount && correct; ++g)
                    if (forStd[g] != forTim[g])
                        correct = false;
                std::cout << (correct ? "correct\n" : "INCORRECT\n");

                t = clock();
                TimSort(forTimWithComp, forTimWithComp + size * amount, comp);
                std::cout << "TimSort (with comparator)\n    time: " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";
                correct = true;
                std::sort(forStd, forStd + size * amount, comp);
                for (int g = 0; g < size * amount && correct; ++g)
                    if (forStd[g] != forTimWithComp[g])
                        correct = false;
                std::cout << (correct ? "correct\n\n" : "INCORRECT\n\n");

                delete[] forTim;
                delete[] forStd;
                delete[] forTimWithComp;
                for (int g = 0; g < amount; ++g)
                    delete[] a1[g];
            }
    }
}

void testOnPartlySortedVectors(const int& tests_number = 1) {
    std::cout << "\nTests on partly sorted vectors:\n***********************\n";
    for (int k = 0; k < tests_number; ++k) {

        std::cout << "Test " << k << ":\n";

        srand(time(0));

        int sizes[] = { 20, 40, 80, 128, 1024 };
        int sizes_number = 5;
        int amounts[] = { 2, 4, 100, 1000 };
        int amounts_number = 4;

        for (int x = 0; x < sizes_number; ++x)
            for (int y = 0; y < amounts_number; ++y) {
                int size = sizes[x];
                int amount = amounts[y];
                int** a1 = new int*[amount];
                for (int j = 0; j < amount; ++j) {
                    a1[j] = new int[size];
                    for (int g = 0; g < size; ++g)
                        a1[j][g] = rand();
                    std::sort(a1[j], a1[j] + size);
                }

                std::vector<int> forTim(size * amount);
                std::vector<int> forStd(size * amount);
                std::vector<int> forTimWithComp(size * amount);
                for (int j = 0; j < size * amount; ++j) {
                    forTim[j] = a1[j / size][j % size];
                    forStd[j] = a1[j / size][j % size];
                    forTimWithComp[j] = a1[j / size][j % size];
                }

                std::cout << amount << " sorted vectors; " << "the size of each equals " << size << "\n";

                time_t t = clock();
                std::sort(forStd.begin(), forStd.end());
                std::cout << "std::sort\n    time: " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";

                t = clock();
                TimSort(forTim.begin(), forTim.end());
                std::cout << "TimSort\n    time: " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";
                bool correct = true;
                for (int g = 0; g < size * amount && correct; ++g)
                    if (forStd[g] != forTim[g])
                        correct = false;
                std::cout << (correct ? "correct\n" : "INCORRECT\n");

                t = clock();
                TimSort(forTimWithComp.begin(), forTimWithComp.end(), comp);
                std::cout << "TimSort\n    time: " << double(clock() - t) / double(CLOCKS_PER_SEC) << " s\n";
                correct = true;
                std::sort(forStd.begin(), forStd.end(), comp);
                for (int g = 0; g < size * amount && correct; ++g)
                    if (forStd[g] != forTimWithComp[g])
                        correct = false;
                std::cout << (correct ? "correct\n\n" : "INCORRECT\n\n");

                for (int g = 0; g < amount; ++g)
                    delete[] a1[g];
            }
    }
}