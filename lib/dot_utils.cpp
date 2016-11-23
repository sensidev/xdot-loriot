#include "dot_utils.h"
#include "xdot_low_power.h"

void config() {
    if (!dot->getStandbyFlag()) {
        logInfo("mbed-os library version: %d", MBED_LIBRARY_VERSION);

        // start from a well-known state
        logInfo("defaulting Dot configuration");
        dot->resetConfig();
        dot->resetNetworkSession();

        // make sure library logging is turned on
        dot->setLogLevel(mts::MTSLog::INFO_LEVEL);

        // update configuration if necessary
        if (dot->getJoinMode() != mDot::MANUAL) {
            logInfo("changing network join mode to MANUAL");
            if (dot->setJoinMode(mDot::MANUAL) != mDot::MDOT_OK) {
                logError("failed to set network join mode to MANUAL");
            }
        }
        // in MANUAL join mode there is no join request/response transaction
        // as long as the Dot is configured correctly and provisioned correctly on the gateway, it should be able to communicate
        // network address - 4 bytes (00000001 - FFFFFFFE)
        // network session key - 16 bytes
        // data session key - 16 bytes
        // to provision your Dot with a Conduit gateway, follow the following steps
        //   * ssh into the Conduit
        //   * provision the Dot using the lora-query application: http://www.multitech.net/developer/software/lora/lora-network-server/
        //      lora-query -a 01020304 A 0102030401020304 <your Dot's device ID> 01020304010203040102030401020304 01020304010203040102030401020304
        //   * if you change the network address, network session key, or data session key, make sure you update them on the gateway
        // to provision your Dot with a 3rd party gateway, see the gateway or network provider documentation
        update_manual_config(network_address, network_session_key, data_session_key, frequency_sub_band, public_network, ack);

        // save changes to configuration
        logInfo("saving configuration");
        if (!dot->saveConfig()) {
            logError("failed to save configuration");
        }

        // display configuration
        display_config();
    } else {
        // restore the saved session if the dot woke from deepsleep mode
        // useful to use with deepsleep because session info is otherwise lost when the dot enters deepsleep
        logInfo("restoring network session from NVM");
        dot->restoreNetworkSession();
    }
}

void display_config() {
    // display configuration and library version information
    logInfo("=====================");
    logInfo("general configuration");
    logInfo("=====================");
    logInfo("version ------------------ %s", dot->getId().c_str());
    logInfo("device ID/EUI ------------ %s", mts::Text::bin2hexString(dot->getDeviceId()).c_str());
    logInfo("frequency band ----------- %s", mDot::FrequencyBandStr(dot->getFrequencyBand()).c_str());
    if (dot->getFrequencySubBand() != mDot::FB_EU868) {
        logInfo("frequency sub band ------- %u", dot->getFrequencySubBand());
    }
    logInfo("public network ----------- %s", dot->getPublicNetwork() ? "on" : "off");
    logInfo("=========================");
    logInfo("credentials configuration");
    logInfo("=========================");
    logInfo("device class ------------- %s", dot->getClass().c_str());
    logInfo("network join mode -------- %s", mDot::JoinModeStr(dot->getJoinMode()).c_str());
    if (dot->getJoinMode() == mDot::MANUAL || dot->getJoinMode() == mDot::PEER_TO_PEER) {
        logInfo("network address ---------- %s", mts::Text::bin2hexString(dot->getNetworkAddress()).c_str());
        logInfo("network session key------- %s", mts::Text::bin2hexString(dot->getNetworkSessionKey()).c_str());
        logInfo("data session key---------- %s", mts::Text::bin2hexString(dot->getDataSessionKey()).c_str());
    } else {
        logInfo("network name ------------- %s", dot->getNetworkName().c_str());
        logInfo("network phrase ----------- %s", dot->getNetworkPassphrase().c_str());
        logInfo("network EUI -------------- %s", mts::Text::bin2hexString(dot->getNetworkId()).c_str());
        logInfo("network KEY -------------- %s", mts::Text::bin2hexString(dot->getNetworkKey()).c_str());
    }
    logInfo("========================");
    logInfo("communication parameters");
    logInfo("========================");
    if (dot->getJoinMode() == mDot::PEER_TO_PEER) {
        logInfo("TX frequency ------------- %lu", dot->getTxFrequency());
    } else {
        logInfo("acks --------------------- %s, %u attempts", dot->getAck() > 0 ? "on" : "off", dot->getAck());
    }
    logInfo("TX datarate -------------- %s", mDot::DataRateStr(dot->getTxDataRate()).c_str());
    logInfo("TX power ----------------- %lu dBm", dot->getTxPower());
    logInfo("atnenna gain ------------- %u dBm", dot->getAntennaGain());
}

void update_ota_config_name_phrase(std::string network_name, std::string network_passphrase, uint8_t frequency_sub_band, bool public_network, uint8_t ack) {
    std::string current_network_name = dot->getNetworkName();
    std::string current_network_passphrase = dot->getNetworkPassphrase();
    uint8_t current_frequency_sub_band = dot->getFrequencySubBand();
    bool current_public_network = dot->getPublicNetwork();
    uint8_t current_ack = dot->getAck();

    if (current_network_name != network_name) {
        logInfo("changing network name from \"%s\" to \"%s\"", current_network_name.c_str(), network_name.c_str());
        if (dot->setNetworkName(network_name) != mDot::MDOT_OK) {
            logError("failed to set network name to \"%s\"", network_name.c_str());
        }
    }

    if (current_network_passphrase != network_passphrase) {
        logInfo("changing network passphrase from \"%s\" to \"%s\"", current_network_passphrase.c_str(), network_passphrase.c_str());
        if (dot->setNetworkPassphrase(network_passphrase) != mDot::MDOT_OK) {
            logError("failed to set network passphrase to \"%s\"", network_passphrase.c_str());
        }
    }

    if (current_frequency_sub_band != frequency_sub_band) {
        logInfo("changing frequency sub band from %u to %u", current_frequency_sub_band, frequency_sub_band);
        if (dot->setFrequencySubBand(frequency_sub_band) != mDot::MDOT_OK) {
            logError("failed to set frequency sub band to %u", frequency_sub_band);
        }
    }

    if (current_public_network != public_network) {
        logInfo("changing public network from %s to %s", current_public_network ? "on" : "off", public_network ? "on" : "off");
        if (dot->setPublicNetwork(public_network) != mDot::MDOT_OK) {
            logError("failed to set public network to %s", public_network ? "on" : "off");
        }
    }

    if (current_ack != ack) {
        logInfo("changing acks from %u to %u", current_ack, ack);
        if (dot->setAck(ack) != mDot::MDOT_OK) {
            logError("failed to set acks to %u", ack);
        }
    }
}

void update_ota_config_id_key(uint8_t *network_id, uint8_t *network_key, uint8_t frequency_sub_band, bool public_network, uint8_t ack) {
    std::vector<uint8_t> current_network_id = dot->getNetworkId();
    std::vector<uint8_t> current_network_key = dot->getNetworkKey();
    uint8_t current_frequency_sub_band = dot->getFrequencySubBand();
    bool current_public_network = dot->getPublicNetwork();
    uint8_t current_ack = dot->getAck();

    std::vector<uint8_t> network_id_vector(network_id, network_id + 8);
    std::vector<uint8_t> network_key_vector(network_key, network_key + 16);

    if (current_network_id != network_id_vector) {
        logInfo("changing network ID from \"%s\" to \"%s\"", mts::Text::bin2hexString(current_network_id).c_str(), mts::Text::bin2hexString(network_id_vector).c_str());
        if (dot->setNetworkId(network_id_vector) != mDot::MDOT_OK) {
            logError("failed to set network ID to \"%s\"", mts::Text::bin2hexString(network_id_vector).c_str());
        }
    }

    if (current_network_key != network_key_vector) {
        logInfo("changing network KEY from \"%s\" to \"%s\"", mts::Text::bin2hexString(current_network_key).c_str(), mts::Text::bin2hexString(network_key_vector).c_str());
        if (dot->setNetworkKey(network_key_vector) != mDot::MDOT_OK) {
            logError("failed to set network KEY to \"%s\"", mts::Text::bin2hexString(network_key_vector).c_str());
        }
    }

    if (current_frequency_sub_band != frequency_sub_band) {
        logInfo("changing frequency sub band from %u to %u", current_frequency_sub_band, frequency_sub_band);
        if (dot->setFrequencySubBand(frequency_sub_band) != mDot::MDOT_OK) {
            logError("failed to set frequency sub band to %u", frequency_sub_band);
        }
    }

    if (current_public_network != public_network) {
        logInfo("changing public network from %s to %s", current_public_network ? "on" : "off", public_network ? "on" : "off");
        if (dot->setPublicNetwork(public_network) != mDot::MDOT_OK) {
            logError("failed to set public network to %s", public_network ? "on" : "off");
        }
    }

    if (current_ack != ack) {
        logInfo("changing acks from %u to %u", current_ack, ack);
        if (dot->setAck(ack) != mDot::MDOT_OK) {
            logError("failed to set acks to %u", ack);
        }
    }
}

void update_manual_config(uint8_t *network_address, uint8_t *network_session_key, uint8_t *data_session_key, uint8_t frequency_sub_band, bool public_network, uint8_t ack) {
    std::vector<uint8_t> current_network_address = dot->getNetworkAddress();
    std::vector<uint8_t> current_network_session_key = dot->getNetworkSessionKey();
    std::vector<uint8_t> current_data_session_key = dot->getDataSessionKey();
    uint8_t current_frequency_sub_band = dot->getFrequencySubBand();
    bool current_public_network = dot->getPublicNetwork();
    uint8_t current_ack = dot->getAck();

    std::vector<uint8_t> network_address_vector(network_address, network_address + 4);
    std::vector<uint8_t> network_session_key_vector(network_session_key, network_session_key + 16);
    std::vector<uint8_t> data_session_key_vector(data_session_key, data_session_key + 16);

    if (current_network_address != network_address_vector) {
        logInfo("changing network address from \"%s\" to \"%s\"", mts::Text::bin2hexString(current_network_address).c_str(), mts::Text::bin2hexString(network_address_vector).c_str());
        if (dot->setNetworkAddress(network_address_vector) != mDot::MDOT_OK) {
            logError("failed to set network address to \"%s\"", mts::Text::bin2hexString(network_address_vector).c_str());
        }
    }

    if (current_network_session_key != network_session_key_vector) {
        logInfo("changing network session key from \"%s\" to \"%s\"", mts::Text::bin2hexString(current_network_session_key).c_str(), mts::Text::bin2hexString(network_session_key_vector).c_str());
        if (dot->setNetworkSessionKey(network_session_key_vector) != mDot::MDOT_OK) {
            logError("failed to set network session key to \"%s\"", mts::Text::bin2hexString(network_session_key_vector).c_str());
        }
    }

    if (current_data_session_key != data_session_key_vector) {
        logInfo("changing data session key from \"%s\" to \"%s\"", mts::Text::bin2hexString(current_data_session_key).c_str(), mts::Text::bin2hexString(data_session_key_vector).c_str());
        if (dot->setDataSessionKey(data_session_key_vector) != mDot::MDOT_OK) {
            logError("failed to set data session key to \"%s\"", mts::Text::bin2hexString(data_session_key_vector).c_str());
        }
    }

    if (current_frequency_sub_band != frequency_sub_band) {
        logInfo("changing frequency sub band from %u to %u", current_frequency_sub_band, frequency_sub_band);
        if (dot->setFrequencySubBand(frequency_sub_band) != mDot::MDOT_OK) {
            logError("failed to set frequency sub band to %u", frequency_sub_band);
        }
    }

    if (current_public_network != public_network) {
        logInfo("changing public network from %s to %s", current_public_network ? "on" : "off", public_network ? "on" : "off");
        if (dot->setPublicNetwork(public_network) != mDot::MDOT_OK) {
            logError("failed to set public network to %s", public_network ? "on" : "off");
        }
    }

    if (current_ack != ack) {
        logInfo("changing acks from %u to %u", current_ack, ack);
        if (dot->setAck(ack) != mDot::MDOT_OK) {
            logError("failed to set acks to %u", ack);
        }
    }
}

void update_peer_to_peer_config(uint8_t *network_address, uint8_t *network_session_key, uint8_t *data_session_key, uint32_t tx_frequency, uint8_t tx_datarate, uint8_t tx_power) {
    std::vector<uint8_t> current_network_address = dot->getNetworkAddress();
    std::vector<uint8_t> current_network_session_key = dot->getNetworkSessionKey();
    std::vector<uint8_t> current_data_session_key = dot->getDataSessionKey();
    uint32_t current_tx_frequency = dot->getTxFrequency();
    uint8_t current_tx_datarate = dot->getTxDataRate();
    uint8_t current_tx_power = dot->getTxPower();

    std::vector<uint8_t> network_address_vector(network_address, network_address + 4);
    std::vector<uint8_t> network_session_key_vector(network_session_key, network_session_key + 16);
    std::vector<uint8_t> data_session_key_vector(data_session_key, data_session_key + 16);

    if (current_network_address != network_address_vector) {
        logInfo("changing network address from \"%s\" to \"%s\"", mts::Text::bin2hexString(current_network_address).c_str(), mts::Text::bin2hexString(network_address_vector).c_str());
        if (dot->setNetworkAddress(network_address_vector) != mDot::MDOT_OK) {
            logError("failed to set network address to \"%s\"", mts::Text::bin2hexString(network_address_vector).c_str());
        }
    }

    if (current_network_session_key != network_session_key_vector) {
        logInfo("changing network session key from \"%s\" to \"%s\"", mts::Text::bin2hexString(current_network_session_key).c_str(), mts::Text::bin2hexString(network_session_key_vector).c_str());
        if (dot->setNetworkSessionKey(network_session_key_vector) != mDot::MDOT_OK) {
            logError("failed to set network session key to \"%s\"", mts::Text::bin2hexString(network_session_key_vector).c_str());
        }
    }

    if (current_data_session_key != data_session_key_vector) {
        logInfo("changing data session key from \"%s\" to \"%s\"", mts::Text::bin2hexString(current_data_session_key).c_str(), mts::Text::bin2hexString(data_session_key_vector).c_str());
        if (dot->setDataSessionKey(data_session_key_vector) != mDot::MDOT_OK) {
            logError("failed to set data session key to \"%s\"", mts::Text::bin2hexString(data_session_key_vector).c_str());
        }
    }

    if (current_tx_frequency != tx_frequency) {
        logInfo("changing TX frequency from %lu to %lu", current_tx_frequency, tx_frequency);
        if (dot->setTxFrequency(tx_frequency) != mDot::MDOT_OK) {
            logError("failed to set TX frequency to %lu", tx_frequency);
        }
    }

    if (current_tx_datarate != tx_datarate) {
        logInfo("changing TX datarate from %u to %u", current_tx_datarate, tx_datarate);
        if (dot->setTxDataRate(tx_datarate) != mDot::MDOT_OK) {
            logError("failed to set TX datarate to %u", tx_datarate);
        }
    }

    if (current_tx_power != tx_power) {
        logInfo("changing TX power from %u to %u", current_tx_power, tx_power);
        if (dot->setTxPower(tx_power) != mDot::MDOT_OK) {
            logError("failed to set TX power to %u", tx_power);
        }
    }
}

void update_network_link_check_config(uint8_t link_check_count, uint8_t link_check_threshold) {
    uint8_t current_link_check_count = dot->getLinkCheckCount();
    uint8_t current_link_check_threshold = dot->getLinkCheckThreshold();

    if (current_link_check_count != link_check_count) {
        logInfo("changing link check count from %u to %u", current_link_check_count, link_check_count);
        if (dot->setLinkCheckCount(link_check_count) != mDot::MDOT_OK) {
            logError("failed to set link check count to %u", link_check_count);
        }
    }

    if (current_link_check_threshold != link_check_threshold) {
        logInfo("changing link check threshold from %u to %u", current_link_check_threshold, link_check_threshold);
        if (dot->setLinkCheckThreshold(link_check_threshold) != mDot::MDOT_OK) {
            logError("failed to set link check threshold to %u", link_check_threshold);
        }
    }
}

void join_network() {
    int32_t j_attempts = 0;
    int32_t ret = mDot::MDOT_ERROR;

    // attempt to join the network
    while (ret != mDot::MDOT_OK) {
        logInfo("attempt %d to join network", ++j_attempts);
        ret = dot->joinNetwork();
        if (ret != mDot::MDOT_OK) {
            logError("failed to join network %d:%s", ret, mDot::getReturnCodeString(ret).c_str());
            // in some frequency bands we need to wait until another channel is available before transmitting again
            uint32_t delay_s = (dot->getNextTxMs() / 1000) + 1;
            if (delay_s < 2) {
                logInfo("waiting %lu s until next free channel", delay_s);
                wait(delay_s);
            } else {
                logInfo("sleeping %lu s until next free channel", delay_s);
                dot->sleep(delay_s, mDot::RTC_ALARM, false);
            }
        }
    }
}

void sleep(bool deepsleep) {
    // if going into deepsleep mode, save the session so we don't need to join again after waking up
    // not necessary if going into sleep mode since RAM is retained
    if (deepsleep) {
        logInfo("saving network session to NVM");
        dot->saveNetworkSession();
    }

    // ONLY ONE of the three functions below should be uncommented depending on the desired wakeup method
    //sleep_wake_rtc_only(deep_sleep);
    //sleep_wake_interrupt_only(deep_sleep);
    sleep_wake_rtc_or_interrupt(deepsleep);
}

void sleep_wake_rtc_only(bool deepsleep) {
    // in some frequency bands we need to wait until another channel is available before transmitting again
    // wait at least 10s between transmissions
    uint32_t delay_s = dot->getNextTxMs() / 1000;
    if (delay_s < 10) {
        delay_s = 10;
    }

    logInfo("%ssleeping %lus", deepsleep ? "deep" : "", delay_s);
    logInfo("application will %s after waking up", deepsleep ? "execute from beginning" : "resume");

    // lowest current consumption in sleep mode can only be achieved by configuring IOs as analog inputs with no pull resistors
    // the library handles all internal IOs automatically, but the external IOs are the application's responsibility
    // certain IOs may require internal pullup or pulldown resistors because leaving them floating would cause extra current consumption
    // for xDot: UART_*, I2C_*, SPI_*, GPIO*, WAKE
    // for mDot: XBEE_*, USBTX, USBRX, PB_0, PB_1
    // steps are:
    //   * save IO configuration
    //   * configure IOs to reduce current consumption
    //   * sleep
    //   * restore IO configuration
    if (! deepsleep) {
        // save the GPIO state.
        sleep_save_io();

        // configure GPIOs for lowest current
        sleep_configure_io();
    }

    // go to sleep/deepsleep for delay_s seconds and wake using the RTC alarm
    dot->sleep(delay_s, mDot::RTC_ALARM, deepsleep);

    if (! deepsleep) {
        // restore the GPIO state.
        sleep_restore_io();
    }
}

void sleep_wake_interrupt_only(bool deepsleep) {
    if (deepsleep) {
        // for xDot, WAKE pin (connected to S2 on xDot-DK) is the only pin that can wake the processor from deepsleep
        // it is automatically configured when INTERRUPT or RTC_ALARM_OR_INTERRUPT is the wakeup source and deepsleep is true in the mDot::sleep call
    } else {
        // configure WAKE pin (connected to S2 on xDot-DK) as the pin that will wake the xDot from low power modes
        //      other pins can be confgured instead: GPIO0-3 or UART_RX
        dot->setWakePin(WAKE);
    }

    logInfo("%ssleeping until interrupt on %s pin", deepsleep ? "deep" : "", deepsleep ? "WAKE" : mDot::pinName2Str(dot->getWakePin()).c_str());

    logInfo("application will %s after waking up", deepsleep ? "execute from beginning" : "resume");

    // lowest current consumption in sleep mode can only be achieved by configuring IOs as analog inputs with no pull resistors
    // the library handles all internal IOs automatically, but the external IOs are the application's responsibility
    // certain IOs may require internal pullup or pulldown resistors because leaving them floating would cause extra current consumption
    // for xDot: UART_*, I2C_*, SPI_*, GPIO*, WAKE
    // for mDot: XBEE_*, USBTX, USBRX, PB_0, PB_1
    // steps are:
    //   * save IO configuration
    //   * configure IOs to reduce current consumption
    //   * sleep
    //   * restore IO configuration
    if (! deepsleep) {
        // save the GPIO state.
        sleep_save_io();

        // configure GPIOs for lowest current
        sleep_configure_io();
    }

    // go to sleep/deepsleep and wake on rising edge of configured wake pin (only the WAKE pin in deepsleep)
    // since we're not waking on the RTC alarm, the interval is ignored
    dot->sleep(0, mDot::INTERRUPT, deepsleep);

    if (! deepsleep) {
        // restore the GPIO state.
        sleep_restore_io();
    }
}

void sleep_wake_rtc_or_interrupt(bool deepsleep) {
    // in some frequency bands we need to wait until another channel is available before transmitting again
    // wait at least 10s between transmissions
    uint32_t delay_s = dot->getNextTxMs() / 1000;
    if (delay_s < 10) {
        delay_s = 10;
    }

    if (deepsleep) {
        // for xDot, WAKE pin (connected to S2 on xDot-DK) is the only pin that can wake the processor from deepsleep
        // it is automatically configured when INTERRUPT or RTC_ALARM_OR_INTERRUPT is the wakeup source and deepsleep is true in the mDot::sleep call
    } else {
        // configure WAKE pin (connected to S2 on xDot-DK) as the pin that will wake the xDot from low power modes
        //      other pins can be confgured instead: GPIO0-3 or UART_RX
        dot->setWakePin(WAKE);
    }

    logInfo("%ssleeping %lus or until interrupt on %s pin", deepsleep ? "deep" : "", delay_s, deepsleep ? "WAKE" : mDot::pinName2Str(dot->getWakePin()).c_str());

    logInfo("application will %s after waking up", deepsleep ? "execute from beginning" : "resume");

    // lowest current consumption in sleep mode can only be achieved by configuring IOs as analog inputs with no pull resistors
    // the library handles all internal IOs automatically, but the external IOs are the application's responsibility
    // certain IOs may require internal pullup or pulldown resistors because leaving them floating would cause extra current consumption
    // for xDot: UART_*, I2C_*, SPI_*, GPIO*, WAKE
    // for mDot: XBEE_*, USBTX, USBRX, PB_0, PB_1
    // steps are:
    //   * save IO configuration
    //   * configure IOs to reduce current consumption
    //   * sleep
    //   * restore IO configuration
    if (! deepsleep) {
        // save the GPIO state.
        sleep_save_io();

        // configure GPIOs for lowest current
        sleep_configure_io();
    }

    // go to sleep/deepsleep and wake using the RTC alarm after delay_s seconds or rising edge of configured wake pin (only the WAKE pin in deepsleep)
    // whichever comes first will wake the xDot
    dot->sleep(delay_s, mDot::RTC_ALARM_OR_INTERRUPT, deepsleep);

    if (! deepsleep) {
        // restore the GPIO state.
        sleep_restore_io();
    }
}

void sleep_save_io() {
    xdot_save_gpio_state();
}

void sleep_configure_io() {
    // GPIO Ports Clock Enable
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOH_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    // UART1_TX, UART1_RTS & UART1_CTS to analog nopull - RX could be a wakeup source
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // I2C_SDA & I2C_SCL to analog nopull
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // SPI_MOSI, SPI_MISO, SPI_SCK, & SPI_NSS to analog nopull
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // iterate through potential wake pins - leave the configured wake pin alone if one is needed
    if (dot->getWakePin() != WAKE || dot->getWakeMode() == mDot::RTC_ALARM) {
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    if (dot->getWakePin() != GPIO0 || dot->getWakeMode() == mDot::RTC_ALARM) {
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    if (dot->getWakePin() != GPIO1 || dot->getWakeMode() == mDot::RTC_ALARM) {
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    if (dot->getWakePin() != GPIO2 || dot->getWakeMode() == mDot::RTC_ALARM) {
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    if (dot->getWakePin() != GPIO3 || dot->getWakeMode() == mDot::RTC_ALARM) {
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    if (dot->getWakePin() != UART1_RX || dot->getWakeMode() == mDot::RTC_ALARM) {
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void sleep_restore_io() {
    xdot_restore_gpio_state();
}

void send_data(std::vector<uint8_t> data) {
    uint32_t ret;

    ret = dot->send(data);
    if (ret != mDot::MDOT_OK) {
        logError("failed to send data to %s [%d][%s]", dot->getJoinMode() == mDot::PEER_TO_PEER ? "peer" : "gateway", ret, mDot::getReturnCodeString(ret).c_str());
    } else {
        logInfo("successfully sent data to %s", dot->getJoinMode() == mDot::PEER_TO_PEER ? "peer" : "gateway");
    }
}
