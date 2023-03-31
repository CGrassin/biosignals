#include "FIRFilter.h"

FIRFilter::FIRFilter(int* filter_taps, unsigned char filter_taps_number) {
  this->filter_taps = filter_taps;
  this->filter_taps_number = filter_taps_number;
  this->init();
}

void FIRFilter::init() {
  for(int i = 0; i < filter_taps_number; ++i)
    this->history[i] = 0;
  this->last_index = 0;
}

void FIRFilter::put(int value) {
  this->history[this->last_index++] = value;
  if(this->last_index == filter_taps_number)
    this->last_index = 0;
}

int FIRFilter::get(){
  long long acc = 0;
  int index = this->last_index, i;
  for(i = 0; i < filter_taps_number; ++i) {
    index = index != 0 ? index-1 : filter_taps_number-1;
    acc += (long long)this->history[index] * this->filter_taps[i];
  };
  return acc >> 16;
}