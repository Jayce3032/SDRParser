#ifndef IPMI_SDR_H
#define IPMI_SDR_H

#include <inttypes.h>
#include <math.h>
#include "ipmi.h"
#include "ipmi_entity.h"
#include "bswap.h"

#define tos32(val, bits)    ((val & ((1<<((bits)-1)))) ? (-((val) & (1<<((bits)-1))) | (val)) : (val))

#if WORDS_BIGENDIAN
# define __TO_TOL(mtol)     (uint16_t)(mtol & 0x3f)
# define __TO_M(mtol)       (int16_t)(tos32((((mtol & 0xff00) >> 8) | ((mtol & 0xc0) << 2)), 10))
# define __TO_B(bacc)       (int32_t)(tos32((((bacc & 0xff000000) >> 24) | ((bacc & 0xc00000) >> 14)), 10))
# define __TO_ACC(bacc)     (uint32_t)(((bacc & 0x3f0000) >> 16) | ((bacc & 0xf000) >> 6))
# define __TO_ACC_EXP(bacc) (uint32_t)((bacc & 0xc00) >> 10)
# define __TO_R_EXP(bacc)   (int32_t)(tos32(((bacc & 0xf0) >> 4), 4))
# define __TO_B_EXP(bacc)   (int32_t)(tos32((bacc & 0xf), 4))
#else
# define __TO_TOL(mtol)     (uint16_t)(BSWAP_16(mtol) & 0x3f)
# define __TO_M(mtol)       (int16_t)(tos32((((BSWAP_16(mtol) & 0xff00) >> 8) | ((BSWAP_16(mtol) & 0xc0) << 2)), 10))
# define __TO_B(bacc)       (int32_t)(tos32((((BSWAP_32(bacc) & 0xff000000) >> 24) | \
                            ((BSWAP_32(bacc) & 0xc00000) >> 14)), 10))
# define __TO_ACC(bacc)     (uint32_t)(((BSWAP_32(bacc) & 0x3f0000) >> 16) | ((BSWAP_32(bacc) & 0xf000) >> 6))
# define __TO_ACC_EXP(bacc) (uint32_t)((BSWAP_32(bacc) & 0xc00) >> 10)
# define __TO_R_EXP(bacc)   (int32_t)(tos32(((BSWAP_32(bacc) & 0xf0) >> 4), 4))
# define __TO_B_EXP(bacc)   (int32_t)(tos32((BSWAP_32(bacc) & 0xf), 4))
#endif

enum {
	ANALOG_SENSOR,
	DISCRETE_SENSOR,
};

#define READING_UNAVAILABLE	0x20
#define SCANNING_DISABLED	0x40
#define EVENT_MSG_DISABLED	0x80

#define IS_READING_UNAVAILABLE(val)	((val) & READING_UNAVAILABLE)
#define IS_SCANNING_DISABLED(val)	(!((val) & SCANNING_DISABLED))
#define IS_EVENT_MSG_DISABLED(val)	(!((val) & EVENT_MSG_DISABLED))

#define GET_SDR_REPO_INFO	0x20
#define GET_SDR_ALLOC_INFO	0x21

#define SDR_SENSOR_STAT_LO_NC	(1<<0)
#define SDR_SENSOR_STAT_LO_CR	(1<<1)
#define SDR_SENSOR_STAT_LO_NR	(1<<2)
#define SDR_SENSOR_STAT_HI_NC	(1<<3)
#define SDR_SENSOR_STAT_HI_CR	(1<<4)
#define SDR_SENSOR_STAT_HI_NR	(1<<5)

#define GET_DEVICE_SDR_INFO      0x20
#define GET_DEVICE_SDR           0x21
#define GET_SENSOR_FACTORS      0x23
#define GET_SENSOR_FACTORS      0x23
#define SET_SENSOR_HYSTERESIS	0x24
#define GET_SENSOR_HYSTERESIS	0x25
#define SET_SENSOR_THRESHOLDS   0x26
#define GET_SENSOR_THRESHOLDS   0x27
#define SET_SENSOR_EVENT_ENABLE	0x28
#define GET_SENSOR_EVENT_ENABLE 0x29
#define GET_SENSOR_EVENT_STATUS	0x2b
#define GET_SENSOR_READING	0x2d
#define GET_SENSOR_TYPE		0x2f

#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
struct sdr_repo_info_rs {
	uint8_t version;	/* SDR version (51h) */
	uint16_t count;		/* number of records */
	uint16_t free;		/* free space in SDR */
	uint32_t add_stamp;	/* last add timestamp */
	uint32_t erase_stamp;	/* last del timestamp */
	uint8_t op_support;	/* supported operations */
} ATTRIBUTE_PACKING;
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif

#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
/* builtin (device) sdrs support */
struct sdr_device_info_rs {
	unsigned char count;	/* number of records */
	unsigned char flags;	/* flags */
	unsigned char popChangeInd[3];	/* free space in SDR */
} ATTRIBUTE_PACKING;
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif

#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
#define GET_SDR_RESERVE_REPO	0x22
struct sdr_reserve_repo_rs {
	uint16_t reserve_id;	/* reservation ID */
} ATTRIBUTE_PACKING;
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif


#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
#define GET_SDR		0x23
struct sdr_get_rq {
	uint16_t reserve_id;	/* reservation ID */
	uint16_t id;		/* record ID */
	uint8_t offset;		/* offset into SDR */
#define GET_SDR_ENTIRE_RECORD	0xff
	uint8_t length;		/* length to read */
} ATTRIBUTE_PACKING;
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif
#define SDR_RECORD_TYPE_FULL_SENSOR             0x01
#define SDR_RECORD_TYPE_COMPACT_SENSOR          0x02
#define SDR_RECORD_TYPE_EVENTONLY_SENSOR        0x03
#define SDR_RECORD_TYPE_ENTITY_ASSOC            0x08
#define SDR_RECORD_TYPE_DEVICE_ENTITY_ASSOC     0x09
#define SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR  0x10
#define SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR      0x11
#define SDR_RECORD_TYPE_MC_DEVICE_LOCATOR       0x12
#define SDR_RECORD_TYPE_MC_CONFIRMATION         0x13
#define SDR_RECORD_TYPE_BMC_MSG_CHANNEL_INFO    0x14
#define SDR_RECORD_TYPE_OEM                     0xc0

#define ASSERT_EVENT   0
#define DEASSERT_EVENT 1

/* Define for display all SDR */
#define SDR_RECORD_TYPE_ALL                     0xff

struct sdr_record_header {
    uint16_t id;
    uint8_t version;
    uint8_t type;
    uint8_t length;
} ATTRIBUTE_PACKING;

struct sdr_record_mask {
    union {
        struct {
            uint16_t assert_event;      /* assertion event mask */
            uint16_t deassert_event;    /* de-assertion event mask */
            uint16_t read;              /* discrete reading mask */
        } ATTRIBUTE_PACKING discrete;

        struct {
#if WORDS_BIGENDIAN
            uint16_t reserved:1;
            uint16_t status_lnr:1;
            uint16_t status_lcr:1;
            uint16_t status_lnc:1;
            uint16_t assert_unr_high:1;
            uint16_t assert_unr_low:1;
            uint16_t assert_ucr_high:1;
            uint16_t assert_ucr_low:1;
            uint16_t assert_unc_high:1;
            uint16_t assert_unc_low:1;
            uint16_t assert_lnr_high:1;
            uint16_t assert_lnr_low:1;
            uint16_t assert_lcr_high:1;
            uint16_t assert_lcr_low:1;
            uint16_t assert_lnc_high:1;
            uint16_t assert_lnc_low:1;
#else
            uint16_t assert_lnc_low:1;
            uint16_t assert_lnc_high:1;
            uint16_t assert_lcr_low:1;
            uint16_t assert_lcr_high:1;
            uint16_t assert_lnr_low:1;
            uint16_t assert_lnr_high:1;
            uint16_t assert_unc_low:1;
            uint16_t assert_unc_high:1;
            uint16_t assert_ucr_low:1;
            uint16_t assert_ucr_high:1;
            uint16_t assert_unr_low:1;
            uint16_t assert_unr_high:1;
            uint16_t status_lnc:1;
            uint16_t status_lcr:1;
            uint16_t status_lnr:1;
            uint16_t reserved:1;
#endif

#if WORDS_BIGENDIAN
            uint16_t reserved_2:1;
            uint16_t status_unr:1;
            uint16_t status_ucr:1;
            uint16_t status_unc:1;
            uint16_t deassert_unr_high:1;
            uint16_t deassert_unr_low:1;
            uint16_t deassert_ucr_high:1;
            uint16_t deassert_ucr_low:1;
            uint16_t deassert_unc_high:1;
            uint16_t deassert_unc_low:1;
            uint16_t deassert_lnr_high:1;
            uint16_t deassert_lnr_low:1;
            uint16_t deassert_lcr_high:1;
            uint16_t deassert_lcr_low:1;
            uint16_t deassert_lnc_high:1;
            uint16_t deassert_lnc_low:1;
#else
            uint16_t deassert_lnc_low:1;
            uint16_t deassert_lnc_high:1;
            uint16_t deassert_lcr_low:1;
            uint16_t deassert_lcr_high:1;
            uint16_t deassert_lnr_low:1;
            uint16_t deassert_lnr_high:1;
            uint16_t deassert_unc_low:1;
            uint16_t deassert_unc_high:1;
            uint16_t deassert_ucr_low:1;
            uint16_t deassert_ucr_high:1;
            uint16_t deassert_unr_low:1;
            uint16_t deassert_unr_high:1;
            uint16_t status_unc:1;
            uint16_t status_ucr:1;
            uint16_t status_unr:1;
            uint16_t reserved_2:1;
#endif
            union {
                struct {
#if WORDS_BIGENDIAN
                    /* settable threshold mask */
                    uint16_t reserved:2;
                    uint16_t unr:1;
                    uint16_t ucr:1;
                    uint16_t unc:1;
                    uint16_t lnr:1;
                    uint16_t lcr:1;
                    uint16_t lnc:1;
                    /* padding lower 8 bits */
                    uint16_t readable:8;
#else
                    uint16_t readable:8;
                    uint16_t lnc:1;
                    uint16_t lcr:1;
                    uint16_t lnr:1;
                    uint16_t unc:1;
                    uint16_t ucr:1;
                    uint16_t unr:1;
                    uint16_t reserved:2;
#endif
                } ATTRIBUTE_PACKING set;

                struct {
#if WORDS_BIGENDIAN
                    /* readable threshold mask */
                    /* padding upper 8 bits */
                    uint16_t settable:8;
                    uint16_t reserved:2;
                    uint16_t unr:1;
                    uint16_t ucr:1;
                    uint16_t unc:1;
                    uint16_t lnr:1;
                    uint16_t lcr:1;
                    uint16_t lnc:1;
#else
                    uint16_t lnc:1;
                    uint16_t lcr:1;
                    uint16_t lnr:1;
                    uint16_t unc:1;
                    uint16_t ucr:1;
                    uint16_t unr:1;
                    uint16_t reserved:2;
                    uint16_t settable:8;
#endif
                } ATTRIBUTE_PACKING read;
            } ATTRIBUTE_PACKING;
        } ATTRIBUTE_PACKING threshold;
    } ATTRIBUTE_PACKING type;
} ATTRIBUTE_PACKING;

struct sdr_record_common_sensor {
    struct {
        uint8_t owner_id;
#if WORDS_BIGENDIAN
        uint8_t channel:4;    /* channel number */
        uint8_t __reserved:2;
        uint8_t lun:2;        /* sensor owner lun */
#else
        uint8_t lun:2;        /* sensor owner lun */
        uint8_t __reserved:2;
        uint8_t channel:4;    /* channel number */
#endif
        uint8_t sensor_num;   /* unique sensor number */
    } ATTRIBUTE_PACKING keys;

    struct entity_id entity;

    struct {
        struct {
#if WORDS_BIGENDIAN
            uint8_t __reserved:1;
            uint8_t scanning:1;
            uint8_t events:1;
            uint8_t thresholds:1;
            uint8_t hysteresis:1;
            uint8_t type:1;
            uint8_t event_gen:1;
            uint8_t sensor_scan:1;
#else
            uint8_t sensor_scan:1;
            uint8_t event_gen:1;
            uint8_t type:1;
            uint8_t hysteresis:1;
            uint8_t thresholds:1;
            uint8_t events:1;
            uint8_t scanning:1;
            uint8_t __reserved:1;
#endif
        } ATTRIBUTE_PACKING init;

        struct {
#if WORDS_BIGENDIAN
            uint8_t ignore:1;
            uint8_t rearm:1;
            uint8_t hysteresis:2;
            uint8_t threshold:2;
            uint8_t event_msg:2;
#else
            uint8_t event_msg:2;
            uint8_t threshold:2;
            uint8_t hysteresis:2;
            uint8_t rearm:1;
            uint8_t ignore:1;
#endif
        } ATTRIBUTE_PACKING capabilities;

        uint8_t type;
    } ATTRIBUTE_PACKING sensor;

    uint8_t event_type; /* event/reading type code */

    struct sdr_record_mask mask;

    struct {
#if WORDS_BIGENDIAN
        uint8_t analog:2;
        uint8_t rate:3;
        uint8_t modifier:2;
        uint8_t pct:1;
#else
        uint8_t pct:1;
        uint8_t modifier:2;
        uint8_t rate:3;
        uint8_t analog:2;
#endif
        struct {
            uint8_t base;
            uint8_t modifier;
        } ATTRIBUTE_PACKING type;

    } ATTRIBUTE_PACKING unit;
} ATTRIBUTE_PACKING;

struct sdr_record_compact_sensor {
    struct sdr_record_common_sensor cmn;

    struct {
#if WORDS_BIGENDIAN
        uint8_t __reserved:2;
        uint8_t mod_type:2;
        uint8_t count:4;
#else
        uint8_t count:4;
        uint8_t mod_type:2;
        uint8_t __reserved:2;
#endif

#if WORDS_BIGENDIAN
        uint8_t entity_inst:1;
        uint8_t mod_offset:7;
#else
        uint8_t mod_offset:7;
        uint8_t entity_inst:1;
#endif
    } ATTRIBUTE_PACKING share;

    struct {
        struct {
            uint8_t positive;
            uint8_t negative;
        } ATTRIBUTE_PACKING hysteresis;
    } ATTRIBUTE_PACKING threshold;

    uint8_t __reserved[3];
    uint8_t oem;            /* reserved for OEM use */
    uint8_t id_code;        /* sensor ID string type/length code */
    uint8_t id_string[16];  /* sensor ID string bytes, only if id_code != 0 */
} ATTRIBUTE_PACKING;

struct sdr_record_eventonly_sensor {
    struct {
        uint8_t owner_id;
#if WORDS_BIGENDIAN
        uint8_t channel:4;      /* channel number */
        uint8_t fru_owner:2;    /* fru device owner lun */
        uint8_t lun:2;          /* sensor owner lun */
#else
        uint8_t lun:2;          /* sensor owner lun */
        uint8_t fru_owner:2;    /* fru device owner lun */
        uint8_t channel:4;      /* channel number */
#endif
        uint8_t sensor_num;     /* unique sensor number */
    } ATTRIBUTE_PACKING keys;

    struct entity_id entity;

    uint8_t sensor_type;	/* sensor type */
    uint8_t event_type;	/* event/reading type code */

    struct {
#if WORDS_BIGENDIAN
        uint8_t __reserved:2;
        uint8_t mod_type:2;
        uint8_t count:4;
#else
        uint8_t count:4;
        uint8_t mod_type:2;
        uint8_t __reserved:2;
#endif

#if WORDS_BIGENDIAN
        uint8_t entity_inst:1;
        uint8_t mod_offset:7;
#else
        uint8_t mod_offset:7;
        uint8_t entity_inst:1;
#endif
    } ATTRIBUTE_PACKING share;

    uint8_t __reserved;
    uint8_t oem;                /* reserved for OEM use */
    uint8_t id_code;            /* sensor ID string type/length code */
    uint8_t id_string[16];      /* sensor ID string bytes, only if id_code != 0 */
} ATTRIBUTE_PACKING;

struct sdr_record_full_sensor {
	struct sdr_record_common_sensor cmn;

#define SDR_SENSOR_L_LINEAR     0x00
#define SDR_SENSOR_L_LN         0x01
#define SDR_SENSOR_L_LOG10      0x02
#define SDR_SENSOR_L_LOG2       0x03
#define SDR_SENSOR_L_E          0x04
#define SDR_SENSOR_L_EXP10      0x05
#define SDR_SENSOR_L_EXP2       0x06
#define SDR_SENSOR_L_1_X        0x07
#define SDR_SENSOR_L_SQR        0x08
#define SDR_SENSOR_L_CUBE       0x09
#define SDR_SENSOR_L_SQRT       0x0a
#define SDR_SENSOR_L_CUBERT     0x0b
#define SDR_SENSOR_L_NONLINEAR  0x70

    uint8_t linearization;      /* 70h=non linear, 71h-7Fh=non linear, OEM */
    uint16_t mtol;              /* M, tolerance */
    uint32_t bacc;              /* accuracy, B, Bexp, Rexp */

    struct {
#if WORDS_BIGENDIAN
        uint8_t __reserved:5;
        uint8_t normal_min:1;   /* normal min field specified */
        uint8_t normal_max:1;   /* normal max field specified */
        uint8_t nominal_read:1; /* nominal reading field specified */
#else
        uint8_t nominal_read:1; /* nominal reading field specified */
        uint8_t normal_max:1;   /* normal max field specified */
        uint8_t normal_min:1;   /* normal min field specified */
        uint8_t __reserved:5;
#endif
    } ATTRIBUTE_PACKING analog_flag;

    uint8_t nominal_read;       /* nominal reading, raw value */
    uint8_t normal_max;         /* normal maximum, raw value */
    uint8_t normal_min;         /* normal minimum, raw value */
    uint8_t sensor_max;         /* sensor maximum, raw value */
    uint8_t sensor_min;         /* sensor minimum, raw value */

    struct {
        struct {
            uint8_t non_recover;
            uint8_t critical;
            uint8_t non_critical;
        } ATTRIBUTE_PACKING upper;

        struct {
            uint8_t non_recover;
            uint8_t critical;
            uint8_t non_critical;
        } ATTRIBUTE_PACKING lower;

        struct {
            uint8_t positive;
            uint8_t negative;
        } ATTRIBUTE_PACKING hysteresis;

    } ATTRIBUTE_PACKING threshold;

    uint8_t __reserved[2];
    uint8_t oem;                /* reserved for OEM use */
    uint8_t id_code;            /* sensor ID string type/length code */
    uint8_t id_string[16];      /* sensor ID string bytes, only if id_code != 0 */
} ATTRIBUTE_PACKING;

struct sdr_record_mc_locator {
    uint8_t dev_slave_addr;
#if WORDS_BIGENDIAN
    uint8_t __reserved2:4;
    uint8_t channel_num:4;
#else
    uint8_t channel_num:4;
    uint8_t __reserved2:4;
#endif

#if WORDS_BIGENDIAN
    uint8_t pwr_state_notif:3;
    uint8_t __reserved3:1;
    uint8_t global_init:4;
#else
    uint8_t global_init:4;
    uint8_t __reserved3:1;
    uint8_t pwr_state_notif:3;
#endif

    uint8_t dev_support;
    uint8_t __reserved4[3];
    struct entity_id entity;
    uint8_t oem;
    uint8_t id_code;
    uint8_t id_string[16];
} ATTRIBUTE_PACKING;

struct sdr_record_fru_locator {
    uint8_t dev_slave_addr;
    uint8_t device_id;
#if WORDS_BIGENDIAN
    uint8_t logical:1;
    uint8_t __reserved2:2;
    uint8_t lun:2;
    uint8_t bus:3;
#else
    uint8_t bus:3;
    uint8_t lun:2;
    uint8_t __reserved2:2;
    uint8_t logical:1;
#endif

#if WORDS_BIGENDIAN
    uint8_t channel_num:4;
    uint8_t __reserved3:4;
#else
    uint8_t __reserved3:4;
    uint8_t channel_num:4;
#endif

    uint8_t __reserved4;
    uint8_t dev_type;
    uint8_t dev_type_modifier;
    struct entity_id entity;
    uint8_t oem;
    uint8_t id_code;
    uint8_t id_string[16];
} ATTRIBUTE_PACKING;

struct sdr_record_generic_locator {
    uint8_t dev_access_addr;
    uint8_t dev_slave_addr;
#if WORDS_BIGENDIAN
    uint8_t channel_num:3;
    uint8_t lun:2;
    uint8_t bus:3;
#else
    uint8_t bus:3;
    uint8_t lun:2;
    uint8_t channel_num:3;
#endif

#if WORDS_BIGENDIAN
    uint8_t addr_span:3;
    uint8_t __reserved1:5;
#else
    uint8_t __reserved1:5;
    uint8_t addr_span:3;
#endif
    uint8_t __reserved2;
    uint8_t dev_type;
    uint8_t dev_type_modifier;
    struct entity_id entity;
    uint8_t oem;
    uint8_t id_code;
    uint8_t id_string[16];
} ATTRIBUTE_PACKING;

struct sdr_record_entity_assoc {
    struct entity_id entity;    /* container entity ID and instance */

    struct {
#if WORDS_BIGENDIAN
        uint8_t isrange:1;
        uint8_t islinked:1;
        uint8_t isaccessable:1;
        uint8_t __reserved:5;
#else
        uint8_t __reserved:5;
        uint8_t isaccessable:1;
        uint8_t islinked:1;
        uint8_t isrange:1;
#endif
    } flags;

    uint8_t entity_id_1;        /* entity ID 1    |  range 1 entity */
    uint8_t entity_inst_1;      /* entity inst 1  |  range 1 first instance */
    uint8_t entity_id_2;        /* entity ID 2    |  range 1 entity */
    uint8_t entity_inst_2;      /* entity inst 2  |  range 1 last instance */
    uint8_t entity_id_3;        /* entity ID 3    |  range 2 entity */
    uint8_t entity_inst_3;      /* entity inst 3  |  range 2 first instance */
    uint8_t entity_id_4;        /* entity ID 4    |  range 2 entity */
    uint8_t entity_inst_4;      /* entity inst 4  |  range 2 last instance */
} ATTRIBUTE_PACKING;

struct sdr_record_oem {
    uint8_t *data;
    int data_len;
};

struct sdr_record_list {
    struct sdr_record_header rec_hdr;

    uint8_t *raw;
    struct sdr_record_list *next;
    union {
        struct sdr_record_common_sensor *common;
        struct sdr_record_full_sensor *full;
        struct sdr_record_compact_sensor *compact;
        struct sdr_record_eventonly_sensor *eventonly;
        struct sdr_record_generic_locator *genloc;
        struct sdr_record_fru_locator *fruloc;
        struct sdr_record_mc_locator *mcloc;
        struct sdr_record_entity_assoc *entassoc;
        struct sdr_record_oem *oem;
    } ATTRIBUTE_PACKING  record;
} ATTRIBUTE_PACKING;


/* unit description codes (IPMI v1.5 section 37.16) */
static const char *unit_desc[] __attribute__ ((unused)) = {
    "unspecified", "degrees C", "degrees F", "degrees K",
    "Volts", "Amps", "Watts", "Joules",
    "Coulombs", "VA", "Nits",
    "lumen", "lux", "Candela",
    "kPa", "PSI", "Newton",
    "CFM", "RPM", "Hz",
    "microsecond", "millisecond", "second", "minute", "hour",
    "day", "week", "mil", "inches", "feet", "cu in", "cu feet",
    "mm", "cm", "m", "cu cm", "cu m", "liters", "fluid ounce",
    "radians", "steradians", "revolutions", "cycles",
    "gravities", "ounce", "pound", "ft-lb", "oz-in", "gauss",
    "gilberts", "henry", "millihenry", "farad", "microfarad",
    "ohms", "siemens", "mole", "becquerel", "PPM", "reserved",
    "Decibels", "DbA", "DbC", "gray", "sievert",
    "color temp deg K", "bit", "kilobit", "megabit", "gigabit",
    "byte", "kilobyte", "megabyte", "gigabyte", "word", "dword",
    "qword", "line", "hit", "miss", "retry", "reset",
    "overflow", "underrun", "collision", "packets", "messages",
    "characters", "error", "correctable error", "uncorrectable error",
};

/* sensor type codes (IPMI v1.5 table 36.3)
  / Updated to v2.0 Table 42-3, Sensor Type Codes */
static const char *sensor_type_desc[] __attribute__ ((unused)) = {
    "reserved", "Temperature", "Voltage", "Current", "Fan",
    "Physical Security", "Platform Security", "Processor",
    "Power Supply", "Power Unit", "Cooling Device", "Other",
    "Memory", "Drive Slot / Bay", "POST Memory Resize",
    "System Firmwares", "Event Logging Disabled", "Watchdog1",
    "System Event", "Critical Interrupt", "Button",
    "Module / Board", "Microcontroller", "Add-in Card",
    "Chassis", "Chip Set", "Other FRU", "Cable / Interconnect",
    "Terminator", "System Boot Initiated", "Boot Error",
    "OS Boot", "OS Critical Stop", "Slot / Connector",
    "System ACPI Power State", "Watchdog2", "Platform Alert",
    "Entity Presence", "Monitor ASIC", "LAN",
    "Management Subsys Health", "Battery", "Session Audit",
    "Version Change", "FRU State"
};

#endif
/* IPMI_SDR_H */
