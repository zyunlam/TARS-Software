/**
 * @file telemetry.cpp
 *
 * @brief This file defines the telemetry class used to facilitate
 * telemetry commands and data transfer between the on-board flight
 * computer and the ground station.
 *
 * Spaceshot Avionics 2021-22
 * Illinois Space Society - Telemetry Team
 * Gautam Dayal
 * Nicholas Phillips
 * Patrick Marschoun
 * Peter Giannetos
 */

#include "mcu_main/telemetry.h"

#include <limits>

#include "RHHardwareSPI1.h"
#include "mcu_main/dataLog.h"
#include "mcu_main/debug.h"

Telemetry tlm;

/**
 * @brief This function maps an input value onto within a particular range into a fixed point value of a certin binary
 * size
 *
 * @param val: number to map into target range, values outside of the range will be clamped
 *
 * @param range: range to map number into. For unsigned output, [0, range). For signed output [-range/2, range)
 *
 * @return fixed point value represing val mapped onto the target range
 */
template <typename T>
T inv_convert_range(float val, float range) {
    size_t numeric_range = (int64_t)std::numeric_limits<T>::max() - (int64_t)std::numeric_limits<T>::min() + 1;
    float converted = val * (float)numeric_range / range;
    return std::max(std::min((float)std::numeric_limits<T>::max(), converted), (float)std::numeric_limits<T>::min());
}

ErrorCode Telemetry::init() {
#ifdef ENABLE_TELEMETRY
    pinMode(RFM96_RST, OUTPUT);
    digitalWrite(RFM96_RST, HIGH);
    delay(10);

    // manual reset

    digitalWrite(RFM96_RST, LOW);
    delay(10);
    digitalWrite(RFM96_RST, HIGH);
    delay(10);

    if (!rf95.init()) {
        return ErrorCode::RADIO_INIT_FAILED;
    }
    Serial.println("[DEBUG]: Radio Initialized");

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf =
    // 128chips/symbol, CRC on

    if (!rf95.setFrequency(RF95_FREQ)) {
        return ErrorCode::RADIO_SET_FREQUENCY_FAILED;
    }

    /*
     * The default transmitter power is 13dBm, using PA_BOOST.
     * If you are using RFM95/96/97/98 modules which uses the PA_BOOST
     * transmitter pin, then you can set transmitter powers from 5 to 23 dBm:
     */
    rf95.setTxPower(6, false);

    sei();
#endif
    return ErrorCode::NO_ERROR;
}

#ifdef ENABLE_TELEMETRY
Telemetry::Telemetry() : rf95(RFM96_CS, RFM96_INT, hardware_spi1) {}
#else
Telemetry::Telemetry() {}
#endif

/**
 * @brief  This function handles commands sent from the ground station
 * to TARS. The effects of this function depend on the command
 * sent.
 *
 * @param cmd: struct containing information necessary to process
 *             ground station command.
 *
 * @return void
 */
void Telemetry::handleCommand(const telemetry_command &cmd) {
    /* Check if the security code is present and matches on ground and on the
     * rocket */
    if (cmd.verify != std::array<char, 6>{'A', 'Y', 'B', 'E', 'R', 'K'}) {
        return;
    }
    /* Check if lasted command ID matched current command ID */
    if (last_command_id == cmd.cmd_id) {
        return;
    }
    last_command_id = (int16_t)cmd.cmd_id;

    /*
     * Write frequency to SD card to save
     * between runs
     */
    if (cmd.command == SET_FREQ) {
        freq_status.should_change = true;
        freq_status.new_freq = cmd.freq;
    }

    if (cmd.command == SET_CALLSIGN) {
        memcpy(callsign, cmd.callsign, sizeof(cmd.callsign));
        Serial.println("[DEBUG]: Got callsign");
    }

    if (cmd.command == ABORT) {
        if (!abort) {
            abort = !abort;
        }
        Serial.println("[DEBUG]: Got abort");
    }
}

//#define TLM_DEBUG

/**
 * @brief This function transmits data from the struct provided as
 * the parameter (data collected from sensor suite) to the
 * ground station. The function also switches to a new commanded
 * frequency based on a previously received command and waits for
 * a response from the ground station.
 *
 * @param sensor_data: struct of data from the sensor suite to be
 *                     transmitted to the ground station.
 *
 * @return void
 */
void Telemetry::transmit() {
#ifdef ENABLE_TELEMETRY
#ifdef TLM_DEBUG
    const uint8_t data[4] = {0, 1, 2, 3};
    rf95.send(data, 4);
    Serial.println("Sending packet...");
    rf95.waitPacketSent();
    Serial.println("Sent packet");
#else
    static bool blue_state = false;
    digitalWrite(LED_BLUE, blue_state);
    blue_state = !blue_state;

    TelemetryPacket packet = makePacket(dataLogger.read());
    rf95.send((uint8_t *)&packet, sizeof(packet));

    chThdSleepMilliseconds(170);

    rf95.waitPacketSent();

    // change the frequency after we acknowledge
    if (freq_status.should_change) {
        rf95.setFrequency(freq_status.new_freq);
        freq_status.should_change = false;
    }

    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.available() && rf95.recv(buf, &len)) {
        telemetry_command received{};
        memcpy(&received, buf, sizeof(received));

        handleCommand(received);
    }
#endif
#endif
}

void printFloat(float f, int precision = 5) {
    if (isinf(f) || isnan(f)) {
        Serial.print(-1);
    } else {
        Serial.print(f, precision);
    }
}

void printJSONField(const char *name, float val, bool comma = true) {
    Serial.print('\"');
    Serial.print(name);
    Serial.print("\":");
    printFloat(val);
    if (comma) Serial.print(',');
}

void printJSONField(const char *name, int val, bool comma = true) {
    Serial.print('\"');
    Serial.print(name);
    Serial.print("\":");
    Serial.print(val);
    if (comma) Serial.print(',');
}

void printJSONField(const char *name, const char *val, bool comma = true) {
    Serial.print('\"');
    Serial.print(name);
    Serial.print("\":\"");
    Serial.print(val);
    Serial.print('"');
    if (comma) Serial.print(',');
}
void Telemetry::serialPrint(const sensorDataStruct_t &sensor_data) {
    Serial.print(R"({"type": "data", "value": {)");
    printJSONField("response_ID", -1);
    printJSONField("gps_lat", sensor_data.gps_data.latitude);
    printJSONField("gps_long", sensor_data.gps_data.longitude);
    printJSONField("gps_alt", sensor_data.gps_data.altitude);
    printJSONField("KX_IMU_ax", sensor_data.highG_data.hg_ax);
    printJSONField("KX_IMU_ay", sensor_data.highG_data.hg_ay);
    printJSONField("KX_IMU_az", sensor_data.highG_data.hg_az);
    printJSONField("IMU_gx", sensor_data.lowG_data.gx);
    printJSONField("IMU_gy", sensor_data.lowG_data.gy);
    printJSONField("IMU_gz", sensor_data.lowG_data.az);
    printJSONField("IMU_mx", sensor_data.magnetometer_data.magnetometer.mx);
    printJSONField("IMU_my", sensor_data.magnetometer_data.magnetometer.my);
    printJSONField("IMU_mz", sensor_data.magnetometer_data.magnetometer.mz);
    printJSONField("FSM_state", (int)sensor_data.rocketState_data.rocketStates[0]);
    printJSONField("sign", "NOSIGN");
    printJSONField("RSSI", rf95.lastRssi());
    printJSONField("Voltage", sensor_data.voltage_data.v_battery);
    printJSONField("frequency", -1);
    printJSONField("flap_extension", sensor_data.flap_data.extension);
    printJSONField("STE_ALT", sensor_data.kalman_data.kalman_pos_x);
    printJSONField("STE_VEL", sensor_data.kalman_data.kalman_vel_x);
    printJSONField("STE_ACC", sensor_data.kalman_data.kalman_acc_x);
    printJSONField("STE_APO", sensor_data.kalman_data.kalman_apo);
    printJSONField("BNO_YAW", sensor_data.orientation_data.angle.yaw);
    printJSONField("BNO_PITCH", sensor_data.orientation_data.angle.pitch);
    printJSONField("BNO_ROLL", sensor_data.orientation_data.angle.roll);
    printJSONField("TEMP", sensor_data.barometer_data.temperature);
    printJSONField("pressure", sensor_data.barometer_data.pressure, false);
    Serial.println("}}");
    // Serial.print(R"({"type": "data", "value": {)");
    // Serial.print(R"("response_ID":)");
    // Serial.print(000);
    // Serial.print(',');
    // Serial.print(R"("gps_lat":)");
    // Serial.print(0);
    // Serial.print(",");
    // Serial.print(R"("gps_long":)");
    // Serial.print(0);
    // Serial.print(",");
    // Serial.print(R"("gps_alt":)");
    // Serial.print(0);
    // Serial.print(",");
    // Serial.print(R"("barometer_alt":)");
    // Serial.print(sensor_data.barometer_data.altitude, 5);
    // Serial.print(',');
    // Serial.print(R"("KX_IMU_ax":)");
    // Serial.print(sensor_data.highG_data.hg_ax, 5);
    // Serial.print(',');
    // Serial.print(R"("KX_IMU_ay":)");
    // Serial.print(sensor_data.highG_data.hg_ay, 5);
    // Serial.print(',');
    // Serial.print(R"("KX_IMU_az":)");
    // Serial.print(sensor_data.highG_data.hg_az, 5);
    // Serial.print(',');

    // Serial.print(R"("LSM_IMU_ax":)");
    // Serial.print(sensor_data.lowG_data.ax, 5);
    // Serial.print(',');
    // Serial.print(R"("LSM_IMU_ay":)");
    // Serial.print(sensor_data.lowG_data.ay, 5);
    // Serial.print(',');
    // Serial.print(R"("LSM_IMU_az":)");
    // Serial.print(sensor_data.lowG_data.az, 5);
    // Serial.print(',');
    // Serial.print(R"("LSM_IMU_gx":)");
    // Serial.print(sensor_data.lowG_data.gx, 5);
    // Serial.print(',');
    // Serial.print(R"("LSM_IMU_gy":)");
    // Serial.print(sensor_data.lowG_data.gy, 5);
    // Serial.print(',');
    // Serial.print(R"("LSM_IMU_gz":)");
    // Serial.print(sensor_data.lowG_data.gz, 5);
    // Serial.print(',');

    // Serial.print(R"("FSM_state":)");
    // Serial.print(1);
    // Serial.print(',');
    // Serial.print(R"("sign":")");
    // Serial.print("SIGN");
    // Serial.print("\",");
    // Serial.print(R"("RSSI":)");
    // Serial.print(rf95.lastRssi());
    // Serial.print(',');
    // Serial.print(R"("Voltage":)");
    // Serial.print(sensor_data.voltage_data.v_battery, 5);
    // Serial.print(',');
    // Serial.print(R"("frequency":)");
    // Serial.print(RF95_FREQ);
    // Serial.print(',');
    // Serial.print(R"("flap_extension":)");
    // Serial.print(sensor_data.flap_data.extension, 5);
    // Serial.print(",");
    // Serial.print(R"("STE_ALT":)");
    // Serial.print(sensor_data.kalman_data.kalman_pos_x, 5);
    // Serial.print(",");
    // Serial.print(R"("STE_VEL":)");
    // Serial.print(sensor_data.kalman_data.kalman_vel_x, 5);
    // Serial.print(",");
    // Serial.print(R"("STE_ACC":)");
    // Serial.print(sensor_data.kalman_data.kalman_acc_x, 5);
    // Serial.print(",");
    // Serial.print(R"("TEMP":)");
    // Serial.print(sensor_data.barometer_data.temperature);
    // Serial.print(",");
    // Serial.print(R"("pressure":)");
    // Serial.print(sensor_data.barometer_data.pressure, 5);
    // Serial.print(",");
    // Serial.print(R"("mx":)");
    // Serial.print(sensor_data.magnetometer_data.magnetometer.mx, 5);
    // Serial.print(",");
    // Serial.print(R"("my":)");
    // Serial.print(sensor_data.magnetometer_data.magnetometer.my, 5);
    // Serial.print(",");
    // Serial.print(R"("mz":)");
    // Serial.print(sensor_data.magnetometer_data.magnetometer.mz, 5);
    // Serial.print(",");
    // Serial.print(R"("STE_APO":)");
    // Serial.print(sensor_data.kalman_data.kalman_apo, 5);
    // Serial.print("");

    // Serial.println("}}\n");
}

TelemetryPacket Telemetry::makePacket(const sensorDataStruct_t &data_struct) {
    TelemetryPacket packet{};
    packet.gps_lat = data_struct.gps_data.latitude;
    packet.gps_long = data_struct.gps_data.longitude;
    packet.gps_alt = data_struct.gps_data.altitude;

    packet.gnc_state_ax = data_struct.kalman_data.kalman_acc_x;
    packet.gnc_state_vx = data_struct.kalman_data.kalman_vel_x;
    packet.gnc_state_x = data_struct.kalman_data.kalman_pos_x;
    packet.gnc_state_ay = data_struct.kalman_data.kalman_acc_y;
    packet.gnc_state_vy = data_struct.kalman_data.kalman_vel_y;
    packet.gnc_state_y = data_struct.kalman_data.kalman_pos_y;
    packet.gnc_state_az = data_struct.kalman_data.kalman_acc_z;
    packet.gnc_state_vz = data_struct.kalman_data.kalman_vel_z;
    packet.gnc_state_z = data_struct.kalman_data.kalman_pos_z;
    packet.gns_state_apo = data_struct.kalman_data.kalman_apo;

    packet.mag_x = inv_convert_range<int16_t>(data_struct.magnetometer_data.magnetometer.mx, 8);
    packet.mag_y = inv_convert_range<int16_t>(data_struct.magnetometer_data.magnetometer.my, 8);
    packet.mag_z = inv_convert_range<int16_t>(data_struct.magnetometer_data.magnetometer.mz, 8);

    packet.gyro_x = inv_convert_range<int16_t>(data_struct.lowG_data.gx, 8192);
    packet.gyro_y = inv_convert_range<int16_t>(data_struct.lowG_data.gy, 8192);
    packet.gyro_z = inv_convert_range<int16_t>(data_struct.lowG_data.gz, 8192);

    packet.response_ID = last_command_id;
    packet.rssi = rf95.lastRssi();
    packet.voltage_battery = inv_convert_range<uint8_t>(data_struct.voltage_data.v_battery, 16);
    packet.FSM_State = (uint8_t)data_struct.rocketState_data.rocketStates[0];
    packet.barometer_temp = inv_convert_range<int16_t>(data_struct.barometer_data.temperature, 256);

    TelemetryDataLite data{};
    packet.datapoint_count = 0;
    for (int8_t i = 0; i < 4 && buffered_data.pop(data); i++) {
        packet.datapoints[i] = data;
        packet.datapoint_count = i + (int8_t)1;
    }
    return packet;
}

void Telemetry::bufferData() {
#ifdef ENABLE_TELEMETRY
#ifndef TLM_DEBUG
    sensorDataStruct_t sensor_data = dataLogger.read();
    TelemetryDataLite data{};
    data.timestamp = TIME_I2MS(chVTGetSystemTime());
    data.barometer_pressure = inv_convert_range<uint16_t>(sensor_data.barometer_data.pressure, 4096);

    data.highG_ax = inv_convert_range<int16_t>(sensor_data.highG_data.hg_ax, 256);
    data.highG_ay = inv_convert_range<int16_t>(sensor_data.highG_data.hg_ay, 256);
    data.highG_az = inv_convert_range<int16_t>(sensor_data.highG_data.hg_az, 256);

    data.bno_pitch = inv_convert_range<int16_t>(sensor_data.orientation_data.angle.pitch, 8);
    data.bno_yaw = inv_convert_range<int16_t>(sensor_data.orientation_data.angle.yaw, 8);
    data.bno_roll = inv_convert_range<int16_t>(sensor_data.orientation_data.angle.roll, 8);

    data.flap_extension = sensor_data.flap_data.extension;
    buffered_data.push(data);

#ifdef SERIAL_PLOTTING
    serialPrint(sensor_data);
#endif
#endif
#endif
}
