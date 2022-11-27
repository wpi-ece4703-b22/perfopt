#include "xlaudio.h"
#include "xlaudio_armdsp.h"

#define NUMTAPS 8

float32_t taps[NUMTAPS];
float32_t B[NUMTAPS];


uint16_t processSample(uint16_t x) {
    float32_t input = xlaudio_adc14_to_f32(x);

    taps[0] = input;

    float32_t q = 0.0;
    uint16_t i;
    for (i = 0; i<NUMTAPS; i++)
        q += taps[i] * B[i];

    for (i = NUMTAPS-1; i>0; i--)
        taps[i] = taps[i-1];

    return xlaudio_f32_to_dac14(q);
}

uint16_t head = 0;

uint16_t processSampleCircular(uint16_t x) {
    float32_t input = xlaudio_adc14_to_f32(x);

    taps[(NUMTAPS - head) % NUMTAPS] = input;

    float32_t q = 0.0;
    uint16_t i;
    for (i = 0; i<NUMTAPS; i++)
        q += taps[i] * B[(i + head) % NUMTAPS];

    head = (head + 1) % NUMTAPS;

    return xlaudio_f32_to_dac14(q);
}

#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    int c = xlaudio_measurePerfSample(processSample);
    printf("Cycles %d\n", c);

    xlaudio_init_intr(FS_16000_HZ, XLAUDIO_J1_2_IN, processSample);
    xlaudio_run();


    return 1;
}
