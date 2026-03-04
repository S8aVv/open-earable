#include "Equalizer.h"

#include <utility>

const float a[EQ_ORDER][3] = {{1,	-1.88005337532644,	0.886850855731273},
                        {1,	0.383474723870624,	0.240950274098301},
                        {1,	0.320433065251179,	0}};

const float b[EQ_ORDER][3] = {{1.13762426437059,	-1.84946471350470,	0.779815253182425},
                        {0.668254486486803,	0.383474723870624,	0.572695787611498},
                        {0.720639527015944,	0.599793538235235,	0}};


Equalizer::Equalizer() {
    //reset();
}

Equalizer::~Equalizer() {
    //reset();
}

void Equalizer::reset() {
    #pragma unroll
    for(int i = 0; i < EQ_ORDER; i++) {
        buffer[i][0] = 0;
        buffer[i][1] = 0;
    }
}

// void Equalizer::update(int16_t * data, int length) {
//     float y[EQ_ORDER+1] = {0};
//     for (int n = 0; n < length; n++) {
//         y[0] = data[n];

//         #pragma unroll
//         for (int k = 0; k < EQ_ORDER; k++) {
//             y[k+1] = b[k][0] * y[k] + buffer[k][0];
//             //#if (a[k][2] != 0 || b[k][2] == 0)
//             buffer[k][0] = b[k][1] * y[k] - a[k][1] * y[k+1] + buffer[k][1];
//             buffer[k][1] = b[k][2] * y[k] - a[k][2] * y[k+1];
//             /*#else
//             buffer[k][0] = b[k][1] * y[k] - a[k][1] * y[k+1];
//             //buffer[k][1] = 0;
//             #endif*/
//         }

//         //if (abs(0.095 * y[EQ_ORDER]) > (1<<15)-1) Serial.println("Limit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

//         data[n] = constrain(0.0005 * y[EQ_ORDER],-1*(1<<15),(1<<15)-1);
//     }
// }

void Equalizer::update(int16_t * data, int length) {
    // === DEBUG: 只打印一次，确认 EQ 被调用 + constrain 上下限 ===
    static bool first_print = true;
    const float k = 0.0005f;
    static float v_max = -1e9f;
    static float v_min =  1e9f;
    static unsigned long counter = 0;

    float y[EQ_ORDER+1] = {0};
    for (int n = 0; n < length; n++) {
        y[0] = data[n];

        #pragma unroll
        for (int k_i = 0; k_i < EQ_ORDER; k_i++) {
            y[k_i+1] = b[k_i][0] * y[k_i] + buffer[k_i][0];
            buffer[k_i][0] = b[k_i][1] * y[k_i] - a[k_i][1] * y[k_i+1] + buffer[k_i][1];
            buffer[k_i][1] = b[k_i][2] * y[k_i] - a[k_i][2] * y[k_i+1];
        }

        float v = k * y[EQ_ORDER];

        if (first_print) {
            Serial.println("EQ update called");
            Serial.print("k = ");
            Serial.println(k, 6);
            Serial.print("constrain lower = ");
            Serial.println(-1 * (1 << 15));
            Serial.print("constrain upper = ");
            Serial.println((1 << 15) - 1);
            Serial.flush();
            first_print = false;
        }

        if (v > v_max) v_max = v;
        if (v < v_min) v_min = v;
        counter++;
        if (counter % 2000 == 0) {
            Serial.print("v_max = ");
            Serial.print(v_max);
            Serial.print(" , v_min = ");
            Serial.println(v_min);
            Serial.flush();
        }

        data[n] = constrain(v, -1 * (1 << 15), (1 << 15) - 1);
    }
}
