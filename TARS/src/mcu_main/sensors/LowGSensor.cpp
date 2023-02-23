#include "mcu_main/sensors/LowGSensor.h"

#include "common/packet.h"
#include "mcu_main/dataLog.h"
#include "mcu_main/pins.h"

LowGSensor lowG;

void LowGSensor::update() {
    chSysLock();
    chMtxLock(&mutex);
    LSM.readAccel();
    LSM.readGyro();
    LSM.readMag();

    ax = LSM.calcAccel(LSM.ax);
    ay = LSM.calcAccel(LSM.ay);
    az = LSM.calcAccel(LSM.az);
    gx = LSM.calcGyro(LSM.gx);
    gy = LSM.calcGyro(LSM.gy);
    gz = LSM.calcGyro(LSM.gz);
    mx = LSM.calcMag(LSM.mx);
    my = LSM.calcMag(LSM.my);
    mz = LSM.calcMag(LSM.mz);

    timestamp = chVTGetSystemTime();

    chMtxUnlock(&mutex);
    chSysUnlock();

    dataLogger.pushLowGFifo((LowGData){ax, ay, az, gx, gy, gz, mx, my, mz, timestamp});
}

Acceleration LowGSensor::getAcceleration() { return Acceleration{ax, ay, az}; }

Gyroscope LowGSensor::getGyroscope() { return Gyroscope{gx, gy, gz}; }
Magnetometer LowGSensor::getMagnetometer() { return Magnetometer{mx, my, mz}; }

ErrorCode LowGSensor::init() {
    // note, we need to send this our CS pins (defined above)
    if (!LSM.beginSPI(LSM9DS1_AG_CS, LSM9DS1_M_CS)) {
        return ErrorCode::CANNOT_CONECT_LSM9DS1;
    }
    return ErrorCode::NO_ERROR;
}
