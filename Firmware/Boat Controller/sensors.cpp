#include "sensors.h"

#include <Wire.h>
#include <TinyGPSPlus.h>
#include <Adafruit_BMP3XX.h>
#include <ICM_20948.h>
#include "config.h"

static TwoWire *sWire = &Wire;
static TinyGPSPlus sGpsParser;
static Adafruit_BMP3XX sBmp;
static ICM_20948_I2C sImu;

static SerialUART &gpsSerial = Serial1;

static float gBiasAx = 0.0f;
static float gBiasAy = 0.0f;
static float gBiasAz = 0.0f;
static float gBiasGx = 0.0f;
static float gBiasGy = 0.0f;
static float gBiasGz = 0.0f;

static float lpf(float prev, float sample, float alpha) {
  return prev + alpha * (sample - prev);
}

static bool calibrateImu() {
  constexpr int kSamples = 500;
  float sumAx = 0, sumAy = 0, sumAz = 0;
  float sumGx = 0, sumGy = 0, sumGz = 0;
  int used = 0;

  for (int i = 0; i < kSamples; i++) {
    if (sImu.dataReady()) {
      sImu.getAGMT();
      sumAx += sImu.accX();
      sumAy += sImu.accY();
      sumAz += sImu.accZ();
      sumGx += sImu.gyrX();
      sumGy += sImu.gyrY();
      sumGz += sImu.gyrZ();
      used++;
    }
    delay(2);
  }

  if (used < kSamples / 2) {
    return false;
  }

  // The SparkFun ICM-20948 returns accel in milli-g and gyro in deg/s. Scale the
  // accel means to g so the gravity term below (1 g) is in matching units; the
  // gyro bias stays in deg/s, which is what the estimator integrates.
  gBiasAx = (sumAx / used) * 0.001f;
  gBiasAy = (sumAy / used) * 0.001f;
  gBiasAz = (sumAz / used) * 0.001f - 1.0f;  // remove 1 g of gravity (now in g)
  gBiasGx = sumGx / used;
  gBiasGy = sumGy / used;
  gBiasGz = sumGz / used;
  return true;
}

bool initSensors(BoatState &state) {
  sWire->setSDA(PIN_I2C_SDA);
  sWire->setSCL(PIN_I2C_SCL);
  sWire->begin();
  sWire->setClock(400000);

  bool imuOk = false;
  if (sImu.begin(*sWire, ICM20948_ADDR) == ICM_20948_Stat_Ok) {
    imuOk = calibrateImu();
  }
  state.imuReady = imuOk;

  bool baroOk = true;
  if (USE_BMP388) {
    baroOk = sBmp.begin_I2C(BMP388_ADDR, sWire);
    if (baroOk) {
      sBmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
      sBmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
      sBmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
      sBmp.setOutputDataRate(BMP3_ODR_50_HZ);
    }
  }
  state.baroReady = baroOk;

  bool gpsOk = true;
  if (USE_GPS) {
    gpsSerial.setRX(PIN_GPS_RX);
    gpsSerial.setTX(PIN_GPS_TX);
    gpsSerial.begin(GPS_BAUD);
  }
  state.gpsReady = gpsOk;

  return imuOk && baroOk && gpsOk;
}

void updateImu(SensorFrame &frame, BoatState &state, uint32_t nowUs) {
  if (!state.imuReady) {
    frame.imu.valid = false;
    return;
  }

  if (!sImu.dataReady()) {
    return;
  }

  sImu.getAGMT();

  // Accel: milli-g -> g so it matches the calibrated bias and the 1 g gravity
  // reference used by the tilt estimate. Gyro is already deg/s.
  float ax = sImu.accX() * 0.001f - gBiasAx;
  float ay = sImu.accY() * 0.001f - gBiasAy;
  float az = sImu.accZ() * 0.001f - gBiasAz;
  float gx = sImu.gyrX() - gBiasGx;
  float gy = sImu.gyrY() - gBiasGy;
  float gz = sImu.gyrZ() - gBiasGz;

  frame.imu.ax = lpf(frame.imu.ax, ax, ACCEL_LPF_ALPHA);
  frame.imu.ay = lpf(frame.imu.ay, ay, ACCEL_LPF_ALPHA);
  frame.imu.az = lpf(frame.imu.az, az, ACCEL_LPF_ALPHA);
  frame.imu.gx = lpf(frame.imu.gx, gx, GYRO_LPF_ALPHA);
  frame.imu.gy = lpf(frame.imu.gy, gy, GYRO_LPF_ALPHA);
  frame.imu.gz = lpf(frame.imu.gz, gz, GYRO_LPF_ALPHA);

  frame.imu.mx = sImu.magX();
  frame.imu.my = sImu.magY();
  frame.imu.mz = sImu.magZ();
  frame.imu.valid = true;
  frame.imu.lastUpdateUs = nowUs;
}

void updateBaro(SensorFrame &frame, BoatState &state, uint32_t nowUs) {
  if (!USE_BMP388 || !state.baroReady) {
    frame.baro.valid = false;
    return;
  }

  static float lastPressure = 0.0f;
  static uint32_t lastUs = 0;

  if (!sBmp.performReading()) {
    frame.baro.valid = false;
    return;
  }

  float pressure = sBmp.pressure;
  float tempC = sBmp.temperature;
  float alt = 44330.0f * (1.0f - powf((pressure / 101325.0f), 0.1903f));

  frame.baro.pressurePa = lpf(frame.baro.pressurePa, pressure, BARO_LPF_ALPHA);
  frame.baro.temperatureC = tempC;
  frame.baro.altitudeM = lpf(frame.baro.altitudeM, alt, BARO_LPF_ALPHA);

  if (lastUs > 0) {
    float dt = (nowUs - lastUs) * 1e-6f;
    float trend = (pressure - lastPressure) / max(dt, 1e-3f);
    frame.baro.pressureTrendPaPerSec = lpf(frame.baro.pressureTrendPaPerSec, trend, 0.08f);
  }

  lastPressure = pressure;
  lastUs = nowUs;
  frame.baro.valid = true;
  frame.baro.lastUpdateUs = nowUs;
}

void updateGps(SensorFrame &frame, BoatState &state, uint32_t nowUs) {
  if (!USE_GPS || !state.gpsReady) {
    frame.gps.valid = false;
    return;
  }

  while (gpsSerial.available() > 0) {
    char c = static_cast<char>(gpsSerial.read());
    sGpsParser.encode(c);
  }

  bool hasLocation = sGpsParser.location.isValid();
  bool hasSpeed = sGpsParser.speed.isValid();
  bool hasCourse = sGpsParser.course.isValid();

  frame.gps.hasFix = hasLocation;
  frame.gps.valid = hasLocation || hasSpeed || hasCourse;
  frame.gps.sats = sGpsParser.satellites.isValid() ? sGpsParser.satellites.value() : 0;
  frame.gps.hdop = sGpsParser.hdop.isValid() ? (sGpsParser.hdop.hdop()) : 99.9f;
  frame.gps.ageMs = sGpsParser.location.age();

  if (hasLocation) {
    frame.gps.lat = sGpsParser.location.lat();
    frame.gps.lon = sGpsParser.location.lng();
  }
  if (hasSpeed) {
    frame.gps.speedMps = sGpsParser.speed.mps();
  }
  if (hasCourse) {
    frame.gps.courseDeg = sGpsParser.course.deg();
  }

  if (frame.gps.valid) {
    frame.gps.lastUpdateUs = nowUs;
  }
}