#pragma once

//#define TMCDEBUG

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#endif

#ifdef ARDUINO
	#include <HardwareSerial.h>
	#include <SPI.h>
#endif

#if (__cplusplus == 201703L) && defined(__has_include)
	#define SW_CAPABLE_PLATFORM __has_include(<SoftwareSerial.h>)
#elif defined(__AVR__) || defined(TARGET_LPC1768) || defined(ARDUINO_ARCH_STM32)
	#define SW_CAPABLE_PLATFORM true
#else
	#define SW_CAPABLE_PLATFORM false
#endif

#if SW_CAPABLE_PLATFORM
	#include <SoftwareSerial.h>
#endif

#include "source/SERIAL_SWITCH.h"
#include "source/SW_SPI.h"

#include "source/TMC_HAL.h"

#pragma GCC diagnostic pop

#include "source/TMC2130_bitfields.h"
#include "source/TMC2160_bitfields.h"
#include "source/TMC5130_bitfields.h"
#include "source/TMC5160_bitfields.h"
#include "source/TMC2208_bitfields.h"
#include "source/TMC2209_bitfields.h"
#include "source/TMC2660_bitfields.h"

#include "source/interfaces/TMC2130.hpp"
#include "source/interfaces/TMC2160.hpp"
#include "source/interfaces/TMC2208.hpp"
#include "source/interfaces/TMC2209.hpp"

#define INIT_REGISTER(REG) REG##_t REG##_register = REG##_t
#define INIT2130_REGISTER(REG) TMC2130_n::REG##_t REG##_register = TMC2130_n::REG##_t
#define INIT2160_REGISTER(REG) TMC2160_n::REG##_t REG##_register = TMC2160_n::REG##_t
#define INIT5130_REGISTER(REG) TMC5130_n::REG##_t REG##_register = TMC5130_n::REG##_t
#define INIT5160_REGISTER(REG) TMC5160_n::REG##_t REG##_register = TMC5160_n::REG##_t
#define INIT2660_REGISTER(REG) TMC2660_n::REG##_t REG##_register = TMC2660_n::REG##_t
#define INIT2208_REGISTER(REG) TMC2208_n::REG##_t REG##_register = TMC2208_n::REG##_t
#define INIT2224_REGISTER(REG) TMC2224_n::REG##_t REG##_register = TMC2224_n::REG##_t
#define SET_ALIAS(TYPE, DRIVER, NEW, ARG, OLD) TYPE (DRIVER::*NEW)(ARG) = &DRIVER::OLD

#define TMCSTEPPER_VERSION 0x000701 // v0.7.1

struct TMC_SPI {
protected:

	// Allow register interfaces to call read and write methods
	template<class> friend class TMCStepper;
	template<class> friend class TMC2130_n::GCONF_i;
	template<class> friend class TMC2130_n::GSTAT_i;
	template<class> friend class TMC2130_n::IOIN_i;
	template<class> friend class TMC2130_n::IHOLD_IRUN_i;
	template<class> friend class TMC2130_n::TPOWERDOWN_i;
	template<class> friend class TMC2130_n::TSTEP_i;
	template<class> friend class TMC2130_n::TPWMTHRS_i;
	template<class> friend class TMC2130_n::TCOOLTHRS_i;
	template<class> friend class TMC2130_n::THIGH_i;
	template<class> friend class TMC2130_n::VDCMIN_i;
	template<class> friend class TMC2130_n::MSCNT_i;
	template<class> friend class TMC2130_n::MSCURACT_i;
	template<class> friend class TMC2130_n::CHOPCONF_i;
	template<class> friend class TMC2130_n::COOLCONF_i;
	template<class> friend class TMC2130_n::DCCTRL_i;
	template<class> friend class TMC2130_n::DRV_STATUS_i;
	template<class> friend class TMC2130_n::PWMCONF_i;
	template<class> friend class TMC2130_n::PWM_SCALE_i;
	template<class> friend class TMC2130_n::ENCM_CTRL_i;
	template<class> friend class TMC2130_n::LOST_STEPS_i;

	template<class> friend class TMC2160_n::IOIN_i;
	template<class> friend class TMC2160_n::GLOBAL_SCALER_i;
	template<class> friend class TMC2160_n::OFFSET_READ_i;
	template<class> friend class TMC2160_n::PWM_SCALE_i;
	template<class> friend class TMC2160_n::SHORT_CONF_i;
	template<class> friend class TMC2160_n::DRV_CONF_i;
	template<class> friend class TMC2160_n::PWMCONF_i;

	TMC_SPI(SPIClass &spi, PinDef cs, int8_t link);
	TMC_SPI(SW_SPIClass &spi, PinDef cs, int8_t link);

	// |         8b        |       32b     |
	// | Address or Status | Register data |
	// |           40b data buffer         |
	union TransferData {
	    struct {
	        union {
	            uint8_t address;
	            uint8_t status;
	        };
	        uint32_t data;
	    };
	    char buffer[5] = {0};
	};

	void setSPISpeed(uint32_t speed);
	void switchCSpin(bool state);

	void beginTransaction();
	void endTransaction();
	void transfer(char *buf, const uint8_t count);
	void write(uint8_t addressByte, uint32_t config);
	uint32_t read(uint8_t addressByte);

	uint8_t status_response;

	static constexpr uint8_t TMC_READ = 0x00,
													TMC_WRITE = 0x80;

	static uint32_t spi_speed; // Default 2MHz
	const TMCStepper_n::PinDef pinCS;
	SPIClass *TMC_HW_SPI = nullptr;
	SW_SPIClass *TMC_SW_SPI = nullptr;

	int8_t link_index;
	static int8_t chain_length;
};

struct TMC_UART {
protected:

	template<class> friend class TMCStepper;
	template<class> friend class TMC2208_n::GCONF_i;
	template<class> friend class TMC2208_n::GSTAT_i;
	template<class> friend class TMC2208_n::IFCNT_i;
	template<class> friend class TMC2208_n::SLAVECONF_i;
	template<class> friend class TMC2208_n::OTP_PROG_i;
	template<class> friend class TMC2208_n::OTP_READ_i;
	template<class> friend class TMC2208_n::IOIN_i;
	template<class> friend class TMC2208_n::FACTORY_CONF_i;
	template<class> friend class TMC2208_n::IHOLD_IRUN_i;
	template<class> friend class TMC2208_n::TPOWERDOWN_i;
	template<class> friend class TMC2208_n::TSTEP_i;
	template<class> friend class TMC2208_n::TPWMTHRS_i;
	template<class> friend class TMC2208_n::VACTUAL_i;
	template<class> friend class TMC2208_n::MSCNT_i;
	template<class> friend class TMC2208_n::MSCURACT_i;
	template<class> friend class TMC2208_n::CHOPCONF_i;
	template<class> friend class TMC2208_n::DRV_STATUS_i;
	template<class> friend class TMC2208_n::PWMCONF_i;
	template<class> friend class TMC2208_n::PWM_SCALE_i;
	template<class> friend class TMC2208_n::PWM_AUTO_i;

	template<class> friend class TMC2209_n::IOIN_i;
	template<class> friend class TMC2209_n::SGTHRS_i;
	template<class> friend class TMC2209_n::SG_RESULT_i;
	template<class> friend class TMC2209_n::COOLCONF_i;

	TMC_UART(HardwareSerial * SerialPort, uint8_t addr);
	TMC_UART(HardwareSerial * SerialPort, uint8_t addr, PinDef mul_pin1, PinDef mul_pin2);
	TMC_UART(PinDef SW_RX_pin, PinDef SW_TX_pin, uint8_t addr);

	static constexpr uint8_t TMC_READ = 0x00,
													TMC_WRITE = 0x80;

    HardwareSerial * HWSerial = nullptr;
	#if SW_CAPABLE_PLATFORM
		SoftwareSerial * SWSerial = nullptr;
		const TMCStepper_n::PinDef RXTX_pin = 0; // Half duplex
	#endif

	SSwitch *sswitch = nullptr;

    static constexpr uint8_t  TMC2208_SYNC = 0x05,
                                                        TMC2208_SLAVE_ADDR = 0x00;
    static constexpr uint8_t replyDelay = 2;
    static constexpr uint8_t abort_window = 5;
    static constexpr uint8_t max_retries = 2;

    struct ReadRequest {
        static constexpr uint8_t length = 3;
        uint8_t sync = TMC2208_SYNC;
        uint8_t driverAddress;
        uint8_t registerAddress;
        uint8_t crc = 0;
    };

    struct WriteDatagram {
        static constexpr uint8_t length = 7;
        uint8_t sync = TMC2208_SYNC;
        uint8_t driverAddress;
        uint8_t registerAddress;
        uint32_t data;
        uint8_t crc = 0;
    };

    typedef WriteDatagram ReadResponse;

	int available();
	size_t getTime() const;
	void preWriteCommunication();
	void preReadCommunication();
	void serial_read(uint8_t *data, int8_t length);
	void serial_write(const uint8_t *data, int8_t length);
	void postWriteCommunication();
	void postReadCommunication();
	void write(uint8_t, uint32_t);
	uint32_t read(uint8_t);
	const uint8_t slaveAddress;
	uint8_t calcCRC(uint8_t datagram[], uint8_t len);

	ReadResponse sendReadRequest(ReadRequest &datagram);

	uint16_t bytesWritten = 0;
	bool CRCerror = false;
};

template<typename DERIVED>
class TMCStepper {
	public:
		uint16_t cs2rms(uint8_t CS);
		void rms_current(uint16_t mA);
		void rms_current(uint16_t mA, float mult);
		uint16_t rms_current();
		void hold_multiplier(float val) { holdMultiplier = val; }
		float hold_multiplier() { return holdMultiplier; }
		uint8_t test_connection();

		// Helper functions
		void microsteps(uint16_t ms);
		uint16_t microsteps();
		void blank_time(uint8_t value);
		uint8_t blank_time();
		void hysteresis_end(int8_t value);
		int8_t hysteresis_end();
		void hysteresis_start(uint8_t value);
		uint8_t hysteresis_start();

	protected:
		TMCStepper(float RS) : Rsense(RS) {};

		const float Rsense;
		float holdMultiplier = 0.5;
};

class TMC2130Stepper :
	public TMC_SPI,
	public TMCStepper<TMC2130Stepper>,
	public TMC2130_n::GCONF_i<TMC2130Stepper>,
	public TMC2130_n::GSTAT_i<TMC2130Stepper>,
	public TMC2130_n::IOIN_i<TMC2130Stepper>,
	public TMC2130_n::IHOLD_IRUN_i<TMC2130Stepper>,
	public TMC2130_n::TPOWERDOWN_i<TMC2130Stepper>,
	public TMC2130_n::TSTEP_i<TMC2130Stepper>,
	public TMC2130_n::TPWMTHRS_i<TMC2130Stepper>,
	public TMC2130_n::TCOOLTHRS_i<TMC2130Stepper>,
	public TMC2130_n::THIGH_i<TMC2130Stepper>,
	public TMC2130_n::XDIRECT_i<TMC2130Stepper>,
	public TMC2130_n::VDCMIN_i<TMC2130Stepper>,
	public TMC2130_n::MSCNT_i<TMC2130Stepper>,
	public TMC2130_n::MSCURACT_i<TMC2130Stepper>,
	public TMC2130_n::CHOPCONF_i<TMC2130Stepper>,
	public TMC2130_n::COOLCONF_i<TMC2130Stepper>,
	public TMC2130_n::DCCTRL_i<TMC2130Stepper>,
	public TMC2130_n::DRV_STATUS_i<TMC2130Stepper>,
	public TMC2130_n::PWMCONF_i<TMC2130Stepper>,
	public TMC2130_n::PWM_SCALE_i<TMC2130Stepper>,
	public TMC2130_n::ENCM_CTRL_i<TMC2130Stepper>,
	public TMC2130_n::LOST_STEPS_i<TMC2130Stepper>
	{
	public:
		TMC2130Stepper(SPIClass &spi, TMCStepper_n::PinDef cs, float RS, int8_t link_index = -1);
		TMC2130Stepper(SW_SPIClass &spi, TMCStepper_n::PinDef cs, float RS, int8_t link_index = -1);

		void begin();
		void defaults();
		bool isEnabled();
		void push();

		// Helper functions
		void sg_current_decrease(uint8_t value);
		uint8_t sg_current_decrease();

		// Can be accessed for a common default value
		static constexpr float default_RS = 0.11;

		// Deleted functions
		__attribute__((deprecated("Please provide a sense resistor value")))
		TMC2130Stepper(TMCStepper_n::PinDef, TMCStepper_n::PinDef, TMCStepper_n::PinDef, TMCStepper_n::PinDef, int8_t link_index = -1) = delete;

		__attribute__((deprecated("Please provide a sense resistor value")))
		TMC2130Stepper(TMCStepper_n::PinDef) = delete;
};

class TMC2160Stepper :
	public TMC_SPI,
	public TMCStepper<TMC2160Stepper>,
	public TMC2160_n::GCONF_i<TMC2160Stepper>,
	public TMC2160_n::GSTAT_i<TMC2160Stepper>,
	public TMC2160_n::IOIN_i<TMC2160Stepper>,
	public TMC2160_n::IHOLD_IRUN_i<TMC2160Stepper>,
	public TMC2160_n::TPOWERDOWN_i<TMC2160Stepper>,
	public TMC2160_n::TSTEP_i<TMC2160Stepper>,
	public TMC2160_n::TPWMTHRS_i<TMC2160Stepper>,
	public TMC2160_n::TCOOLTHRS_i<TMC2160Stepper>,
	public TMC2160_n::THIGH_i<TMC2160Stepper>,
	public TMC2160_n::XDIRECT_i<TMC2160Stepper>,
	public TMC2160_n::VDCMIN_i<TMC2160Stepper>,
	public TMC2160_n::MSCNT_i<TMC2160Stepper>,
	public TMC2160_n::MSCURACT_i<TMC2160Stepper>,
	public TMC2160_n::CHOPCONF_i<TMC2160Stepper>,
	public TMC2160_n::COOLCONF_i<TMC2160Stepper>,
	public TMC2160_n::DCCTRL_i<TMC2160Stepper>,
	public TMC2160_n::DRV_STATUS_i<TMC2160Stepper>,
	public TMC2160_n::PWMCONF_i<TMC2160Stepper>,
	public TMC2160_n::ENCM_CTRL_i<TMC2160Stepper>,
	public TMC2160_n::LOST_STEPS_i<TMC2130Stepper>,
	public TMC2160_n::SHORT_CONF_i<TMC2160Stepper>,
	public TMC2160_n::DRV_CONF_i<TMC2160Stepper>,
	public TMC2160_n::GLOBAL_SCALER_i<TMC2160Stepper>,
	public TMC2160_n::OFFSET_READ_i<TMC2160Stepper>,
	public TMC2160_n::PWM_SCALE_i<TMC2160Stepper>
	{
	public:
		TMC2160Stepper(SPIClass &spi, TMCStepper_n::PinDef pinCS, float RS, int8_t link_index = -1);
		TMC2160Stepper(SW_SPIClass &spi, TMCStepper_n::PinDef pinCS, float RS, int8_t link_index = -1);
		void begin();
		void defaults();
		void push();

		uint16_t cs2rms(uint8_t CS);
		void rms_current(uint16_t mA);
		void rms_current(uint16_t mA, float mult);
		uint16_t rms_current();

		// Deleted functions
		__attribute__((deprecated("Please provide a sense resistor value")))
		TMC2160Stepper(TMCStepper_n::PinDef, TMCStepper_n::PinDef, TMCStepper_n::PinDef, TMCStepper_n::PinDef, int8_t link_index = -1) = delete;

		static constexpr float default_RS = 0.075;
};
#if 0
class TMC5130Stepper {
	public:
		TMC5130Stepper(SPIClass &spi, TMCStepper_n::PinDef pinCS, float RS, int8_t link_index = -1);
		TMC5130Stepper(SW_SPIClass &spi, TMCStepper_n::PinDef pinCS, float RS, int8_t link_index = -1);

		void begin();
		void defaults();
		void push();

		void rms_current(uint16_t mA) { TMC2130Stepper::rms_current(mA); }
		void rms_current(uint16_t mA, float mult) { TMC2130Stepper::rms_current(mA, mult); }
		uint16_t rms_current() { return TMC2130Stepper::rms_current(); }

		// R: IFCNT
		uint8_t IFCNT();
		// W: SLAVECONF
		uint16_t SLAVECONF();
		void SLAVECONF(uint16_t input);
		// R: IOIN
		uint32_t 	IOIN();
		bool 			refl_step();
		bool 			refr_dir();
		bool 			encb_dcen_cfg4();
		bool 			enca_dcin_cfg5();
		bool 			drv_enn_cfg6();
		bool 			enc_n_dco();
		bool 			sd_mode();
		bool 			swcomp_in();
		uint8_t 	version();

		// RW: GCONF
		void diag1_poscomp_pushpull(bool B) { diag1_pushpull(B); }
		bool diag1_poscomp_pushpull() { return diag1_pushpull(); }

		// RW: SW_MODE
		uint32_t SW_MODE();
		void SW_MODE(uint32_t input);

		void stop_l_enable(bool B);
		void stop_r_enable(bool B);
		void pol_stop_l(bool B);
		void pol_stop_r(bool B);
		void swap_lr(bool B);
		void latch_l_active(bool B);
		void latch_l_inactive(bool B);
		void latch_r_active(bool B);
		void latch_r_inactive(bool B);
		void en_latch_encoder(bool B);
		void sg_stop(bool B);
		void en_softstop(bool B);

		bool stop_r_enable();
		bool pol_stop_l();
		bool pol_stop_r();
		bool swap_lr();
		bool latch_l_active();
		bool latch_l_inactive();
		bool latch_r_active();
		bool latch_r_inactive();
		bool en_latch_encoder();
		bool sg_stop();
		bool en_softstop();

		// R+C: RAMP_STAT
		uint16_t RAMP_STAT();
		bool status_stop_l();
		bool status_stop_r();
		bool status_latch_l();
		bool status_latch_r();
		bool event_stop_l();
		bool event_stop_r();
		bool event_stop_sg();
		bool event_pos_reached();
		bool velocity_reached();
		bool position_reached();
		bool vzero();
		bool t_zerowait_active();
		bool second_move();
		bool status_sg();

		// RW: ENCMODE
		uint16_t ENCMODE();
		void ENCMODE(uint16_t input);
		void pol_a(bool B);
		void pol_b(bool B);
		void pol_n(bool B);
		void ignore_ab(bool B);
		void clr_cont(bool B);
		void clr_once(bool B);
		void pos_edge(bool B);
		void neg_edge(bool B);
		void clr_enc_x(bool B);
		void latch_x_act(bool B);
		void enc_sel_decimal(bool B);
		bool pol_a();
		bool pol_b();
		bool pol_n();
		bool ignore_ab();
		bool clr_cont();
		bool clr_once();
		bool pos_edge();
		bool neg_edge();
		bool clr_enc_x();
		bool latch_x_act();
		bool enc_sel_decimal();

		// W: OUTPUT
		bool TMC_OUTPUT();
		void TMC_OUTPUT(bool input);
		// W: X_COMPARE
		uint32_t X_COMPARE();
		void X_COMPARE(uint32_t input);
		// RW: RAMPMODE
		uint8_t RAMPMODE();
		void RAMPMODE(uint8_t input);
		// RW: XACTUAL
		int32_t XACTUAL();
		void XACTUAL(int32_t input);
		// R: VACTUAL
		int32_t VACTUAL();
		// W: VSTART
		uint32_t VSTART();
		void VSTART(uint32_t input);
		// W: A1
		uint16_t A1();
		void A1(uint16_t input);
		// W: V1
		uint32_t V1();
		void V1(uint32_t input);
		// W: AMAX
		uint16_t AMAX();
		void AMAX(uint16_t input);
		// W: VMAX
		uint32_t VMAX();
		void VMAX(uint32_t input);
		// W: DMAX
		uint16_t DMAX();
		void DMAX(uint16_t input);
		// W: D1
		uint16_t D1();
		void D1(uint16_t input);
		// W: VSTOP
		uint32_t VSTOP();
		void VSTOP(uint32_t input);
		// W: TZEROWAIT
		uint16_t TZEROWAIT();
		void TZEROWAIT(uint16_t input);
		// RW: XTARGET
		int32_t XTARGET();
		void XTARGET(int32_t input);
		// R: XLATCH
		uint32_t XLATCH();
		// RW: X_ENC
		int32_t X_ENC();
		void X_ENC(int32_t input);
		// W: ENC_CONST
		uint32_t ENC_CONST();
		void ENC_CONST(uint32_t input);
		// R: ENC_STATUS
		bool ENC_STATUS();
		// R: ENC_LATCH
		uint32_t ENC_LATCH();

		__attribute__((deprecated("Please provide a sense resistor value")))
		TMC5130Stepper(TMCStepper_n::PinDef, TMCStepper_n::PinDef, TMCStepper_n::PinDef, TMCStepper_n::PinDef, int8_t link_index = -1) = delete;

	protected:
		struct IFCNT_t 		{ constexpr static uint8_t address = 0x02; }; // R
		struct VACTUAL_t 	{ constexpr static uint8_t address = 0x22; }; // R
		struct XTARGET_t 	{ constexpr static uint8_t address = 0x2D; }; // RW
		struct XLATCH_t 	{ constexpr static uint8_t address = 0x36; }; // R
		struct X_ENC_t 		{ constexpr static uint8_t address = 0x39; }; // RW
		struct ENC_STATUS_t { constexpr static uint8_t address = 0x3B; }; // R+C
		struct ENC_LATCH_t 	{ constexpr static uint8_t address = 0x3C; }; // R

		static constexpr float default_RS = 0.15;

};

class TMC5160Stepper : {
	public:
		TMC5160Stepper(SPIClass &spi, TMCStepper_n::PinDef pinCS, float RS, int8_t link_index = -1);
		TMC5160Stepper(SW_SPIClass &spi, TMCStepper_n::PinDef pinCS, float RS, int8_t link_index = -1);

		void rms_current(uint16_t mA) { TMC2160Stepper::rms_current(mA); }
		void rms_current(uint16_t mA, float mult) { TMC2160Stepper::rms_current(mA, mult); }
		uint16_t rms_current() { return TMC2160Stepper::rms_current(); }
		void defaults();
		void push();

		// RW: GCONF
		void recalibrate(bool);
		void faststandstill(bool);
		void multistep_filt(bool);
		bool recalibrate();
		bool faststandstill();
		bool multistep_filt();

		// R: IOIN
		bool drv_enn() { return drv_enn_cfg6(); }
		bool enc_n_dco_cfg6() { return enc_n_dco(); }

		// W: SHORT_CONF
		using TMC2160Stepper::SHORT_CONF;
		using TMC2160Stepper::s2vs_level;
		using TMC2160Stepper::s2g_level;
		using TMC2160Stepper::shortfilter;
		using TMC2160Stepper::shortdelay;

		// W: DRV_CONF
		using TMC2160Stepper::DRV_CONF;
		using TMC2160Stepper::bbmtime;
		using TMC2160Stepper::bbmclks;
		using TMC2160Stepper::otselect;
		using TMC2160Stepper::drvstrength;
		using TMC2160Stepper::filt_isense;

		// W: GLOBAL_SCALER
		using TMC2160Stepper::GLOBAL_SCALER;

		// R: OFFSET_READ
		using TMC2160Stepper::OFFSET_READ;

		// R+WC: ENC_STATUS
		void ENC_STATUS(uint8_t);
		uint8_t ENC_STATUS();

		// W: ENC_DEVIATION
		void ENC_DEVIATION(uint32_t);
		uint32_t ENC_DEVIATION();

		// R: PWM_AUTO
		uint32_t PWM_AUTO();
		uint8_t pwm_ofs_auto();
		uint8_t pwm_grad_auto();

		// RW: CHOPCONF
		void diss2vs(bool);
		void tpfd(uint8_t);
		bool diss2vs();
		uint8_t tpfd();

		// W: PWM_CONF

		__attribute__((deprecated("Please provide a sense resistor value")))
		TMC5160Stepper(TMCStepper_n::PinDef, TMCStepper_n::PinDef, TMCStepper_n::PinDef, TMCStepper_n::PinDef, int8_t link_index = -1) = delete;

		static constexpr float default_RS = 0.075;
};

typedef TMC5160Stepper TMC5161Stepper;
#endif
class TMC2208Stepper :
	public TMC_UART,
	public TMCStepper<TMC2208Stepper>,
	public TMC2208_n::GCONF_i<TMC2208Stepper>,
	public TMC2208_n::GSTAT_i<TMC2208Stepper>,
	public TMC2208_n::IFCNT_i<TMC2208Stepper>,
	public TMC2208_n::SLAVECONF_i<TMC2208Stepper>,
	public TMC2208_n::OTP_PROG_i<TMC2208Stepper>,
	public TMC2208_n::OTP_READ_i<TMC2208Stepper>,
	public TMC2208_n::IOIN_i<TMC2208Stepper>,
	public TMC2208_n::FACTORY_CONF_i<TMC2208Stepper>,
	public TMC2208_n::IHOLD_IRUN_i<TMC2208Stepper>,
	public TMC2208_n::TPOWERDOWN_i<TMC2208Stepper>,
	public TMC2208_n::TSTEP_i<TMC2208Stepper>,
	public TMC2208_n::TPWMTHRS_i<TMC2208Stepper>,
	public TMC2208_n::VACTUAL_i<TMC2208Stepper>,
	public TMC2208_n::MSCNT_i<TMC2208Stepper>,
	public TMC2208_n::MSCURACT_i<TMC2208Stepper>,
	public TMC2208_n::CHOPCONF_i<TMC2208Stepper>,
	public TMC2208_n::DRV_STATUS_i<TMC2208Stepper>,
	public TMC2208_n::PWMCONF_i<TMC2208Stepper>,
	public TMC2208_n::PWM_SCALE_i<TMC2208Stepper>,
	public TMC2208_n::PWM_AUTO_i<TMC2208Stepper>
	{
	public:
	    TMC2208Stepper(HardwareSerial * SerialPort, float RS, uint8_t addr, TMCStepper_n::PinDef mul_pin1, TMCStepper_n::PinDef mul_pin2);
		TMC2208Stepper(HardwareSerial * SerialPort, float RS) :
			TMC2208Stepper(SerialPort, RS, TMC2208_SLAVE_ADDR)
			{}
		#if SW_CAPABLE_PLATFORM
			TMC2208Stepper(TMCStepper_n::PinDef SW_RX_pin, TMCStepper_n::PinDef SW_TX_pin, float RS) :
				TMC2208Stepper(SW_RX_pin, SW_TX_pin, RS, TMC2208_SLAVE_ADDR)
				{}

			__attribute__((deprecated("Boolean argument has been deprecated and does nothing")))
			TMC2208Stepper(TMCStepper_n::PinDef SW_RX_pin, TMCStepper_n::PinDef SW_TX_pin, float RS, bool) :
				TMC2208Stepper(SW_RX_pin, SW_TX_pin, RS, TMC2208_SLAVE_ADDR)
				{};
		#else
			TMC2208Stepper(TMCStepper_n::PinDef, TMCStepper_n::PinDef, float) = delete; // Your platform does not currently support Software Serial
		#endif
		void defaults();
		void push();
		void begin();
		#if SW_CAPABLE_PLATFORM
			void beginSerial(uint32_t baudrate);
		#else
			void beginSerial(uint32_t) = delete; // Your platform does not currently support Software Serial
		#endif
		bool isEnabled();

	protected:

		TMC2208Stepper(HardwareSerial * SerialPort, float RS, uint8_t addr);
		#if SW_CAPABLE_PLATFORM
			TMC2208Stepper(TMCStepper_n::PinDef SW_RX_pin, TMCStepper_n::PinDef SW_TX_pin, float RS, uint8_t addr);
		#endif
};

class TMC2209Stepper :
	public TMC2208Stepper,
	public TMC2209_n::IOIN_i<TMC2209Stepper>,
	public TMC2209_n::TCOOLTHRS_i<TMC2209Stepper>,
	public TMC2209_n::SGTHRS_i<TMC2209Stepper>,
	public TMC2209_n::SG_RESULT_i<TMC2209Stepper>,
	public TMC2209_n::COOLCONF_i<TMC2209Stepper>
	{
	public:
		TMC2209Stepper(HardwareSerial * SerialPort, float RS, uint8_t addr) :
			TMC2208Stepper(SerialPort, RS, addr) {}

		#if SW_CAPABLE_PLATFORM
			TMC2209Stepper(TMCStepper_n::PinDef SW_RX_pin, TMCStepper_n::PinDef SW_TX_pin, float RS, uint8_t addr) :
				TMC2208Stepper(SW_RX_pin, SW_TX_pin, RS, addr) {}
		#else
			TMC2209Stepper(TMCStepper_n::PinDef, TMCStepper_n::PinDef, float, uint8_t) = delete; // Your platform does not currently support Software Serial
		#endif
		void push();
};

class TMC2224Stepper : public TMC2208Stepper {
	public:
		uint32_t IOIN();
		bool enn();
		bool ms1();
		bool ms2();
		bool pdn_uart();
		bool spread();
		bool step();
		bool sel_a();
		bool dir();
		uint8_t version();
};

class TMC2660Stepper {
	public:
		TMC2660Stepper(SPIClass &spi, TMCStepper_n::PinDef pinCS, float RS);
		TMC2660Stepper(SW_SPIClass &spi, TMCStepper_n::PinDef pinCS, float RS);
		void begin();
		bool isEnabled();
		uint8_t test_connection();
		uint16_t cs2rms(uint8_t CS);
		uint16_t rms_current();
		void rms_current(uint16_t mA);
		//uint16_t getMilliamps() {return val_mA;}
		void push();
		uint8_t savedToff() { return _savedToff; }

		// Helper functions
		void microsteps(uint16_t ms);
		uint16_t microsteps();
		void blank_time(uint8_t value);
		uint8_t blank_time();
		void hysteresis_end(int8_t value);
		int8_t hysteresis_end();
		void hysteresis_start(uint8_t value);
		uint8_t hysteresis_start();

		// W: DRVCONF
		void DRVCONF(uint32_t);
		void tst(bool);
		void slph(uint8_t);
		void slpl(uint8_t);
		void diss2g(bool);
		void ts2g(uint8_t);
		void sdoff(bool);
		void vsense(bool);
		void rdsel(uint8_t);
		uint32_t DRVCONF();
		bool tst();
		uint8_t slph();
		uint8_t slpl();
		bool diss2g();
		uint8_t ts2g();
		bool sdoff();
		bool vsense();
		uint8_t rdsel();

		// W: DRVCTRL
		void DRVCTRL(uint32_t);
		void pha(bool B);
		void ca(uint8_t B);
		void phb(bool B);
		void cb(uint8_t B);
		bool pha();
		uint8_t ca();
		bool phb();
		uint8_t cb();
		void intpol(bool);
		void dedge(bool);
		void mres(uint8_t);
		uint32_t DRVCTRL();
		bool intpol();
		bool dedge();
		uint8_t mres();

		// W: CHOPCONF
		void CHOPCONF(uint32_t);
		void tbl(uint8_t);
		void chm(bool);
		void rndtf(bool);
		void hdec(uint8_t);
		void hend(uint8_t);
		void hstrt(uint8_t);
		void toff(uint8_t);
		uint32_t CHOPCONF();
		uint8_t tbl();
		bool chm();
		bool rndtf();
		uint8_t hdec();
		uint8_t hend();
		uint8_t hstrt();
		uint8_t toff();

		// R: DRVSTATUS
		uint32_t DRV_STATUS() { return DRVSTATUS(); }
		uint32_t DRVSTATUS();
		uint16_t mstep();
		uint8_t se();
		bool stst();
		bool olb();
		bool ola();
		bool s2gb();
		bool s2ga();
		bool otpw();
		bool ot();
		bool sg();
		uint16_t sg_result();

		// W: SGCSCONF
		uint32_t SGCSCONF();
		void sfilt(bool);
		void sgt(uint8_t);
		void cs(uint8_t);
		void SGCSCONF(uint32_t);
		bool sfilt();
		uint8_t sgt();
		uint8_t cs();

		// W: SMARTEN
		void SMARTEN(uint32_t);
		void seimin(bool B);
		void sedn(uint8_t B);
		void semax(uint8_t B);
		void seup(uint8_t B);
		void semin(uint8_t B);
		uint32_t SMARTEN();
		bool seimin();
		uint8_t sedn();
		uint8_t semax();
		uint8_t seup();
		uint8_t semin();
		/*
		// Alias
		SET_ALIAS(void, polarity_A, bool, pha);
		SET_ALIAS(void, current_A, uint8_t, ca);
		SET_ALIAS(void, polarity_B, bool, phb);
		SET_ALIAS(void, current_b, uint8_t, cb);
		SET_ALIAS(void, interpolate, bool, intpol);
		SET_ALIAS(void, double_edge_step, bool, dedge);
		SET_ALIAS(void, microsteps, uint8_t, mres);
		SET_ALIAS(void, blank_time, uint8_t, tbl);
		SET_ALIAS(void, chopper_mode, bool, chm);
		SET_ALIAS(void, random_off_time, bool, rndtf);
		SET_ALIAS(void, hysteresis_decrement, uint8_t, hdec);
		SET_ALIAS(void, hysteresis_low, uint8_t, hend);
		SET_ALIAS(void, hysteresis_start, uint8_t, hstrt);
		SET_ALIAS(void, off_time, uint8_t, toff);
		*/

		uint8_t status_response;

		// Deleted functions
		__attribute__((deprecated("Please provide a sense resistor value")))
		TMC2660Stepper(TMCStepper_n::PinDef pinCS, TMCStepper_n::PinDef pinMOSI, TMCStepper_n::PinDef pinMISO, TMCStepper_n::PinDef pinSCK, int8_t link_index = -1) = delete;

	private:
		INIT_REGISTER(DRVCTRL_1){{.sr=0}};
		INIT_REGISTER(DRVCTRL_0){{.sr=0}};
		INIT2660_REGISTER(CHOPCONF){{.sr=0}};
		INIT_REGISTER(SMARTEN){{.sr=0}};
		INIT_REGISTER(SGCSCONF){{.sr=0}};
		INIT_REGISTER(DRVCONF){{.sr=0}};
		INIT_REGISTER(READ_RDSEL00){{.sr=0}};
		INIT_REGISTER(READ_RDSEL01){{.sr=0}};
		INIT_REGISTER(READ_RDSEL10){{.sr=0}};

		const TMCStepper_n::PinDef pinCS;
		const float Rsense;
		static constexpr float default_RS = 0.1;
		float holdMultiplier = 0.5;
		uint32_t spi_speed = 16000000/8; // Default 2MHz
		uint8_t _savedToff = 0;
		SPIClass *TMC_HW_SPI = nullptr;
		SW_SPIClass *TMC_SW_SPI = nullptr;

        void write(uint8_t addressByte, uint32_t config);
        uint32_t read();
        void beginTransaction();
        void endTransaction();
        void transfer(char *buf, const uint8_t count);
};
