#pragma once

enum __attribute__((warn_unused)) ErrorCode {
    NO_ERROR,
    CANNOT_CONNECT_KX134_CS,
    CANNOT_INIT_KX134_CS,
    CANNOT_CONNECT_GPS,
    CANNOT_CONNECT_MAGNETOMETER,
    CANNOT_CONNECT_LSM9DS1,
    SD_BEGIN_FAILED,
    RADIO_INIT_FAILED,
    RADIO_SET_FREQUENCY_FAILED
};


void handleError(ErrorCode);