#include "utils.hpp"

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