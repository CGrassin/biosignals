#include "FIRFilter.h"

FIRFilter::FIRFilter(int* filter_taps, unsigned char filter_taps_number) : filter_taps(filter_taps), filter_taps_number(filter_taps_number), last_index(0)
{
  // Allocate memory for the filter history buffer
  this->history = new int[filter_taps_number];
  
  // Copy the filter taps into the class
  // this->filter_taps = new int[filter_taps_number];
  // memcpy(this->filter_taps, filter_taps, filter_taps_number * sizeof(int));
  this->init();
}

FIRFilter::~FIRFilter() {
    // Free memory allocated for the filter history buffer and taps array
    delete[] history;
    //delete[] filter_taps;
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

int FIRFilter::get(int value){
  this->put(value);
  return this->get();
}

