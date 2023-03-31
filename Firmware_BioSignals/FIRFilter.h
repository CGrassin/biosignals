#ifndef FIRFILTER_H
#define FIRFILTER_H

/** A basic FIR filter implementation inspired by the C implementation of http://t-filter.engineerjs.com/
*/
class FIRFilter{
private:
  // Parameters
  unsigned char filter_taps_number;
  int* filter_taps;

  // States
  unsigned char last_index;
  int history[];
public:
  FIRFilter(int* filter_taps, unsigned char filter_taps_number);
  void init();
  void put(int value);
  int get();
};
#endif