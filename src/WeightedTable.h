#ifndef WEIGHTEDTABLE_H_INCLUDED
#define WEIGHTEDTABLE_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "Const.h"

struct entry {
    int value;
    int weight;
};

// TODO: Add sort functions
class WeightedTable
{
    public:
        std::vector<entry> table;

    ///Constructor Function
    // Creates a Weight Table of specified size. All entries are initialized with identical weights.
    // Default size is 0, default weight is 1.
    WeightedTable(int s = 0, int w = 1){

        for (int val = 0; val < s; val++)
            addEntry(val, w);

    }

    ///Deconstructor
    ~WeightedTable(){
        printf("WeightedTable Object Deconstructing...\n");
        table.clear();
    }

    // Returns the number of entries in the table
    int size(){ return table.size(); }

    // Clears all entries in the table
    void clear(){ table.clear(); }

    // Get/Set the value of a specified entry
    int getValue(int i) { return table[i].value; }
    void setValue(int i, int v) { table[i].value = v; }

    // Get/Set the weight of a specified entry
    int getWeight(int i) { return table[i].weight; }
    void setWeight(int i, int w) { table[i].weight = w; }

    // Returns the sum of weight values of all entries in the table
    int totalWeight() {

        int s = table.size();
        int total = 0;

        for (int i = 0; i < s; i++)
            total += table[i].weight;

        return total;
    }

    // Appends a new entry with a specified value and weight at the end of the table
    void addEntry(int v, int w) {
        entry input;
        input.value = v;
        input.weight = w;
        table.push_back(input);
    }

    // Delete a specified entry
    void removeEntry(int i) { table.erase(table.begin()+i); }

    // Returns the index location of a specified value. If the value does not exist in the table, return -1
    int findValue(int v) {

        int result = -1;
        int s = table.size();

        for (int i = 0; i < s; i++) {
            if (table[i].value == v) {
                result = i;
                break;
            }
        }

        return result;
    }

    // Return a value stored in the table. The result is determined randomly, but skewed by associated weights.
    int roll() {
        int count = ( rand() % totalWeight() );
        int s = table.size();
        int result = -1;

        printf("Raw roll value: %d\n", count);

        for (int i = 0; i < s; i++) {
            count -= table[i].weight;
            if (count < 0) {
                result = table[i].value;
                break;
            }
        }
        return result;
    }

    // Print contents of table in debug console
    void print() {

        printf("Entry\tValue\tWeight\n");

        int s = table.size();

        for (int i = 0; i < s; i++)
            printf("%d\t%d\t%d\n", i, table[i].value, table[i].weight);

        printf("\n");
    }

};
#endif // WEIGHTEDTABLE_H_INCLUDED