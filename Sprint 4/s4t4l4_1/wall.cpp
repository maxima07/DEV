#include "wall.h"

Wall::Wall(double width, double height)
    : width_(width)
    , height_(height)
    , color_(Color::WHITE) {
}

/*Напишите определение методов класса Wall здесь,
пользуясь примером конструктора, данного выше*/

double Wall::GetHeight() const {
    return height_;
}

double Wall::GetWidth() const {
    return width_;
}

void Wall::SetColor(Wall::Color color){
    Wall::color_ = color;
}

Wall::Color Wall::GetColor() const{
    return color_;
}



