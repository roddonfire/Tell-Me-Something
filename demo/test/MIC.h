#ifndef MIC_h
#define MIC_h
#include "Arduino.h"
#include <base64.h>

class MIC
{
  public:
    MIC(String &result);
    void record(int duration);

  private:
    int8_t mulaw_encode(int16_t sample);

    int SAMPLE_FREQ;                    // Hz, telephone sample rate
    int SAMPLE_DURATION;                // duration of fixed sampling
    int NUM_SAMPLES;                    // number of of samples
    int ENC_LEN;                        // Encoded length of clip
    int AUDIO_IN;                       //pin where microphone is connected
    String _speech_data;                // global to hold speech data
    unsigned long time_since_sample;    // used for microsecond timing
};
#endif