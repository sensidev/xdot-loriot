#include "dot_utils.h"

// deepsleep consumes slightly less current than sleep
// in sleep mode, IO state is maintained, RAM is retained, and application will resume after waking up
// in deepsleep mode, IOs float, RAM is lost, and application will start from beginning after waking up
// if deep_sleep == true, device will enter deepsleep mode
static bool deep_sleep = true;

mDot* dot = NULL;

Serial pc(USBTX, USBRX);

I2C i2c(I2C_SDA, I2C_SCL);
ISL29011 lux(i2c);

uint16_t read_light_sensor_data() {
    uint16_t light;

    // configure the ISL29011 sensor on the xDot-DK for continuous ambient light sampling, 16 bit conversion, and maximum range
    lux.setMode(ISL29011::ALS_CONT);
    lux.setResolution(ISL29011::ADC_16BIT);
    lux.setRange(ISL29011::RNG_64000);

    // get the latest light sample and send it to the gateway
    light = lux.getData();
    logInfo("light: %lu [0x%04X]", light, light);

    // put the LSL29011 ambient light sensor into a low power state
    lux.setMode(ISL29011::PWR_DOWN);

    return light;
}

int main() {
    uint16_t light;
    std::vector<uint8_t> tx_data;

    pc.baud(115200);

    mts::MTSLog::setLogLevel(mts::MTSLog::TRACE_LEVEL);

    dot = mDot::getInstance();

    config();

    while (true) {

        // Wait for the light sensor to be ready.
        wait(1);

        light = read_light_sensor_data();

        tx_data.clear();
        tx_data.push_back((light >> 8) & 0xFF);
        tx_data.push_back(light & 0xFF);

        send_data(tx_data);

        sleep(deep_sleep);
    }
}
