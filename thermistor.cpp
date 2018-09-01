#import "thermistor.h"

/**
 * code based on http://edukit.thingsos.de/sketches/thermistor/
 *
 * @param port e.g. A0, A1, ...
 */
float thermistor_measureTemperature(uint32_t port) {
    int analogValue = analogRead(port);

    float c1 = 1.009249522e-03;
    float c2 = 2.378405444e-04;
    float c3 = 2.019202697e-07;

    float r = 10000 * (1023.0 / (float)analogValue - 1.0);
    float logR = log(r);
    float tempK = (1.0 / (c1 + c2 * logR + c3 * logR * logR * logR));
    float tempC = tempK - 273.15;

    Serial.print("Temp: ");
    Serial.println(tempC);

    return tempC;
}
