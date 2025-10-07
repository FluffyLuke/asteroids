#include <math.h>

#include "utils.hpp"

Counter::Counter() {
    this->value = 0;
    this->maxValue = 0; 
}

Counter::Counter(f32 maxValue) {
    this->value = 0;
    this->maxValue = maxValue; 
}

Counter::Counter(f32 minValue, f32 maxValue) {
    this->value = minValue;
    this->maxValue = maxValue; 
}

void Counter::ResetCounter() {
    this->value = 0;
}

bool Counter::UpdateCounter(f32 v) {
    this->value += v;
    return (this->value >= this->maxValue);
}

bool Counter::Check() {
    return (this->value >= this->maxValue);
}

Vector2 ScreenCenter() {
    return {
        GetScreenWidth() / 2.0f,
        GetScreenHeight() / 2.0f,
    };
}

Vector2 RandomPointOnCircleEdge(Vector2 center, f32 radius) {
    float randomAngle = GetRandomValue(0, 360);
    // degrees to radians
    randomAngle *= PI / 180;
    float edgeX = center.x + radius * cosf(randomAngle);
    float edgeY = center.y + radius * sinf(randomAngle);

    return {edgeX, edgeY};
}

bool InsideRec(Rectangle rec, Vector2 point) {

    bool a = point.x > rec.x;
    bool b = point.y > rec.y;
    bool c = point.x < rec.x + rec.width;
    bool d = point.y < rec.y + rec.height;

    return (a && b && c && d);
}