#include <iostream>
#include "builder.h"

using namespace std;

int main() {
    Builder tom;
    // Попросите Тома посчитать кирпичи для стены 3.5 х 2.45
    double wall_height = 3.5;
    double wall_width = 2.45;
    cout << tom.CalcBricksNeeded(Wall(wall_height, wall_width)) << endl;
}