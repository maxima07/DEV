#include "carpenter.h"
#include "square_calculation.h"

int Carpenter::CalcShelves (Wall& wall) {
    double height = wall.GetHeight();
    double width = wall.GetWidth();

    return CalcSquare(height, width) / 2;
}