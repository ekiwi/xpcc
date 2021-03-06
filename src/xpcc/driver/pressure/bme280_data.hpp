// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <cmath>

#include <xpcc/debug/logger/logger.hpp>

#ifndef XPCC_BME280_DATA_HPP
#define XPCC_BME280_DATA_HPP

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DISABLED

// Forward declaration the test class
class Bme280Test;

namespace xpcc
{

template < typename I2cMaster >
class Bme280;

namespace bme280data
{

/**
 * Holds the calibration data from the sensor.
 * Values are used for calculation of calibrated
 * sensor values from raw sensor data
 */
struct xpcc_packed
Calibration
{
	uint16_t T1; // 88 89
	int16_t  T2; // 8A 8B
	int16_t  T3; // 8C 8D
	uint16_t P1; // 8e 8f
	int16_t  P2; // 90 91
	int16_t  P3; // 92 93
	int16_t  P4; // 94 95
	int16_t  P5; // 96 97
	int16_t  P6; // 98 99
	int16_t  P7; // 9a 9b
	int16_t  P8; // 9c 9d
	int16_t  P9; // 9e 9f
	uint8_t  unused; // a0 unused
	uint8_t  H1; // a1

	int16_t  H2; // e1 e2
	uint8_t  H3; // e3
	int16_t  H4; // e4 e5[3:0]
	int16_t  H5; // e5[7:4] e6
	int8_t   H6; // e7
};

struct xpcc_packed
DataBase
{
	template < typename I2cMaster >
	friend class ::xpcc::Bme280;

	// Grant unit test full access to private members.
	friend class ::Bme280Test;

public:
	// DATA ACCESS
	inline Calibration &
	getCalibration() {
		return calibration;
	}

public:
	/// Notify data class about changed buffer of raw temperature data.
	void inline
	rawTemperatureTouched() { meta &= ~TEMPERATURE_CALCULATED; }

	/// Notify data class about changed buffer of raw pressure data.
	void inline
	rawPressureTouched() { meta &= ~PRESSURE_CALCULATED; }

	/// Notify data class about changed buffer of raw pressure data.
	void inline
	rawHumidityTouched() { meta &= ~HUMIDITY_CALCULATED; }

protected:
	Calibration calibration;

protected:
	/// The raw data that was read from the sensor
	/// 0 .. 2 pressure data
	/// 3 .. 5 temperatur data
	/// 6 .. 7 humidity data
	uint8_t raw[8];

	// bit 2: humidityCalculated
	// bit 1: temperatureCalculated
	// bit 0: pressureCalculated
	uint8_t meta;

	enum
	{
		/// Remember if the raw data was already converted to calibrated humidity
		HUMIDITY_CALCULATED = Bit2,
		/// Remember if the raw data was already converted to calibrated temperature
		TEMPERATURE_CALCULATED = Bit1,
		/// Remember if the raw data was already converted to calibrated pressure
		PRESSURE_CALCULATED = Bit0,
	};
};

class xpcc_packed
Data : public DataBase
{
public:
	/**
	 * Get the calibrated temperature for the device in 0.01 degree Celsius
	 *
	 * If recalculation is necessary it is done on the fly.
	 * No I2C transaction.
	 */
	int32_t inline
	getTemperature();

	void inline
	getTemperature(int32_t &temp) { temp = getTemperature(); };

	void inline
	getTemperature(float &temp) { temp = float(getTemperature() / 100.0f); };

	void inline
	getTemperature(double &temp) { temp = double(getTemperature()) / double(100.0); };

	/**
	 * Get the calibrated pressure from the device in Pascal.
	 *
	 * If recalculation is necessary it is done on the fly.
	 * No I2C transaction.
	 */
	int32_t inline
	getPressure();

	int32_t inline
	getHumidity();

protected:
	/**
	 * Use the calibration data read from the sensor to
	 * calculate the calibrated temperature from the
	 * raw data.
	 * The result is stored in this struct for further
	 * access.
	 * Implemented differently for Fixed Point, Single and
	 * Double Precision by template specialsation.
	 */
	void inline
	calculateCalibratedTemperature();

	/**
	 * See calculateCalibratedTemperature()
	 */
	void inline
	calculateCalibratedPressure();

	/**
	 * See calculateCalibratedTemperature()
	 */
	void inline
	calculateCalibratedHumidity();


private:
	int32_t calibratedTemperature; // in 0.01 degree Celsius
	int32_t calibratedPressure;    // in Pa
	int32_t calibratedHumidity;    // in ???

	int32_t t_fine; // calculated in calculateCalibratedTemperature, needed for calculateCalibratedPressure and calculateCalibratedHumidity
};

class xpcc_packed
DataDouble : public DataBase
{
public:
	int32_t inline
	getTemperature();

	void inline
	getTemperature(int32_t &temp);

	void inline
	getTemperature(float &temp);

	void inline
	getTemperature(double &temp);

	int32_t inline
	getPressure();

	int32_t inline
	getHumidity();

protected:
	void inline
	calculateCalibratedTemperature();

	void inline
	calculateCalibratedPressure();

	void inline
	calculateCalibratedHumidity();

private:
	// Stored temperature, needed for pressure calculation
	double calibratedTemperatureDouble;

	// Stored pressure in double precision in hPa
	double calibratedPressureDouble;

	// Stored pressure in double precision in ???
	double calibratedHumidityDouble;

	// calculated in calculateCalibratedTemperature, needed for calculateCalibratedPressure and calculateCalibratedHumidity	
	double t_fine; 
};

} // bme280data namespace

} // xpcc namespace

#include "bme280_data_impl_fp.hpp"
#include "bme280_data_impl_double.hpp"

#endif // XPCC_BME280_DATA_HPP
