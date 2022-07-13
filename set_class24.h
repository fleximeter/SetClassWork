/*
* File: set_class24.h
* Author: Jeff Martin
* This file contains functionality for working with microtonal set-classes.
*
* Copyright © 2022 by Jeffrey Martin. All rights reserved.
* Email: jmartin@jeffreymartincomposer.com
* Website: https://jeffreymartincomposer.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef SET_CLASS24
#define SET_CLASS24
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

int* calculatePrimeForm24(int* pcset, int n);
std::string calculatePrimeFormName24(int* pcset, int n);
void subsetsPrime24(int* pcset, int n);

// Calculates the prime form of a pcset
// pcset: A pcset
// n: The number of pcs in the pcset
// Returns a pointer to a new array containing the sorted prime form of the pcset.
int* calculatePrimeForm24(int* pcset, int n)
{
    // The prime form
    int* primeForm = nullptr;
    int m = 2 * n;
    if (n > 0)
    {
        int** listsToWeight = new int* [m];
        int* inverted = new int[n];
        for (int i = 0; i < n; i++)
            inverted[i] = (pcset[i] * 23) % 24;

        // Add regular forms
        for (int i = 0; i < n; i++)
        {
            listsToWeight[i] = new int[n];
            int initialPitch = 0;
            for (int i2 = i; i2 < n; i2++)
                listsToWeight[i][i2 - i] = pcset[i2];
            for (int i2 = 0; i2 < i; i2++)
                listsToWeight[i][n - i + i2] = pcset[i2];
            initialPitch = listsToWeight[i][0];
            for (int i2 = 0; i2 < n; i2++)
                listsToWeight[i][i2] = (listsToWeight[i][i2] - initialPitch + 24) % 24;
            std::sort(listsToWeight[i], listsToWeight[i] + n);
        }

        // Add inverted forms
        for (int i = 0; i < n; i++)
        {
            listsToWeight[i + n] = new int[n];
            int initialPitch = 0;
            for (int i2 = i; i2 < n; i2++)
                listsToWeight[i + n][i2 - i] = inverted[i2];
            for (int i2 = 0; i2 < i; i2++)
                listsToWeight[i + n][n - i + i2] = inverted[i2];
            initialPitch = listsToWeight[i + n][0];
            for (int i2 = 0; i2 < n; i2++)
                listsToWeight[i + n][i2] = (listsToWeight[i + n][i2] - initialPitch + 24) % 24;
            std::sort(listsToWeight[i + n], listsToWeight[i + n] + n);
        }

        // Weight from right
        for (int i = n - 1; i > -1; i--)
        {
            if (m > 1)
            {
                // Identify the smallest item at the current index
                int smallestItem = 23;
                int j = 0;
                for (; j < m; j++)
                {
                    if (listsToWeight[j][i] < smallestItem)
                        smallestItem = listsToWeight[j][i];
                }
                j = 0;

                // Delete all arrays that do not have the smallest item at the current index
                while (j < m)
                {
                    if (listsToWeight[j][i] > smallestItem)
                    {
                        delete[] listsToWeight[j];
                        for (int k = j + 1; k < 2 * n; k++)
                            listsToWeight[k - 1] = listsToWeight[k];
                        m--;
                    }
                    else
                        j += 1;
                }
            }
            else
                break;
        }

        primeForm = listsToWeight[0];
        for (int i = 1; i < m; i++)
            delete[] listsToWeight[i];
        delete[] listsToWeight;
        delete[] inverted;
    }
    return primeForm;
}

// Calculates the prime form name of a pcset
// pcset: A pcset
// n: The number of pcs in the pcset
// Returns a string with the prime-form name of the pcset.
std::string calculatePrimeFormName24(int* pcset, int n)
{
    int* primeForm = calculatePrimeForm24(pcset, n);
    std::string name = "[";
    for (int i = 0; i < n - 1; i++)
    {
        if (primeForm[i] < 10)
        {
            name += '0';
            name += (primeForm[i] + '0');
        }
        else
        {
            name += primeForm[i] / 10 + '0';
            name += primeForm[i] % 10 + '0';
        }
        name += ", ";
    }
    if (n > 0)
    {
        if (primeForm[n - 1] < 10)
        {
            name += '0';
            name += primeForm[n - 1] + '0';
        }
        else
        {
            name += primeForm[n - 1] / 10 + '0';
            name += primeForm[n - 1] % 10 + '0';
        }
    }
    name += ']';
    delete[] primeForm;
    return name;
}

// Gets a list of all prime-form subsets of a pcset,
// and outputs them to a file.
// pcset: A pcset
// n: The length of the pcset
// Uses the bit vector solution from https://www.geeksforgeeks.org/power-set/
void subsetsPrime24(int* pcset, int n)
{
    int* subset = new int[n];
    std::unordered_map<std::string, bool> setClasses;
    std::string name;
    unsigned int size = std::pow(2, n);
    int currentSize;
    std::ofstream file;

    for (int c = 0; c < size; c++)
    {
        currentSize = 0;
        for (int j = 0; j < n; j++)
        {
            if (c & (1 << j))
                subset[currentSize++] = pcset[j];
        }
        name = calculatePrimeFormName24(subset, currentSize);
        setClasses.insert({ name, true });
    }

    // Write to file
    file.open("abstract_subset24.json");
    file << "{\n  \"setClasses24\": [\n";
    for (const auto pair : setClasses)
        file << "    " << pair.first << ",\n";
    file << "  ]\n}\n";
    file.close();
    delete[] subset;
}

#endif
