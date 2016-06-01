#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "stdlib.h"
#include "backgammon_utilities.h"
#include "generator.h"
#include "judge.h"
#include "linearRegression.h"

int main() {
    RandomStrategy randomS;
    LinearRegression linearS;
    Judge possibility(&linearS, &randomS);
    std::cout << possibility.whitePossibility();
    return 0;
}