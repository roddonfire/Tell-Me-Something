#include "MIC.h"
#include "Arduino.h"
#include <base64.h>
#include "OurServer.h"

MIC::MIC(String &mic_result) {
    SAMPLE_FREQ = 8000;                                            // Hz, telephone sample rate
    SAMPLE_DURATION = 4;                                           // duration of fixed sampling
    NUM_SAMPLES = SAMPLE_FREQ * SAMPLE_DURATION;                   // number of of samples
    ENC_LEN = (NUM_SAMPLES + 2 - ((NUM_SAMPLES + 2) % 3)) / 3 * 4; // Encoded length of clip
    AUDIO_IN = A0;
    _speech_data = &mic_result;
}

void MIC::record(int duration) { 
    SAMPLE_DURATION = duration;
    NUM_SAMPLES = SAMPLE_FREQ*SAMPLE_DURATION;
    int sample_num = 0;                  // counter for samples
    float time_between_samples = 1000000 / SAMPLE_FREQ;
    int value = 0;
    uint8_t raw_samples[3]; // 8-bit raw sample data array
    String enc_samples;     // encoded sample data array
    time_since_sample = micros();
    Serial.println(NUM_SAMPLES);
    *_speech_data = "";
    while (sample_num < NUM_SAMPLES)
    {                                                             //read in NUM_SAMPLES worth of audio data
        value = analogRead(AUDIO_IN);                             //make measurement
        raw_samples[sample_num % 3] = mulaw_encode(value - 1241); //remove 1.0V offset (from 12 bit reading)
        sample_num++;
        if (sample_num % 3 == 0)
        {
            *_speech_data += base64::encode(raw_samples, 3);
        }

        // wait till next time to read
        while (micros() - time_since_sample <= time_between_samples)
            ; //wait...
        time_since_sample = micros();
    }
    // Serial.println("Audio snippet: " + *_speech_data.substring(0, 200));
    return;
}

int8_t MIC::mulaw_encode(int16_t sample) {
    // const uint16_t MULAW_MAX = 0x1FFF;
    // const uint16_t MULAW_BIAS = 33;
    // uint16_t mask = 0x1000;
    // uint8_t sign = 0;
    // uint8_t position = 12;
    // uint8_t lsb = 0;
    // if (sample < 0)
    // {
    //     sample = -sample;
    //     sign = 0x80;
    // }
    // sample += MULAW_BIAS;
    // if (sample > MULAW_MAX)
    // {
    //     sample = MULAW_MAX;
    // }
    // for (; ((sample & mask) != mask && position >= 5); mask >>= 1, position--)
    //     ;
    // lsb = (sample >> (position - 4)) & 0x0f;
    // return (~(sign | ((position - 5) << 4) | lsb));
    return (int8_t)(sample/64);
}