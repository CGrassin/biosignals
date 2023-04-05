#ifndef FIRFILTER_H
#define FIRFILTER_H

/** A basic FIR filter implementation inspired by the C implementation of http://t-filter.engineerjs.com/
*/
class FIRFilter{
private:
  // Parameters
  unsigned char filter_taps_number; // Number of filter taps
  int* filter_taps; // Filter taps

  // States
  unsigned char last_index; // Current tap position
  int* history; // Filter history buffer
public:
  FIRFilter(int* filter_taps, unsigned char filter_taps_number);
  ~FIRFilter();
  void init();
  void put(int value);
  int get();
  int get(int value);
};
#endif