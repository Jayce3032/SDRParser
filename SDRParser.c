#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ipmi_sdr.h"
#include "ipmi_sel.h"
#include <math.h>

#define VERSION_NUMBER "1.0.0"

typedef unsigned int bool;
#define false 0
#define true  1

#define AMI_SDR_VALID 0x5a

static struct sdr_record_list *sdr_list_head = NULL;
static struct sdr_record_list *sdr_list_tail = NULL;

void PrintCopyright(void)
{
    printf("------------------------------------------------------------------------------\n");
    printf("SDR Binary File Parser Application (Version %s)\n",VERSION_NUMBER);
    printf("------------------------------------------------------------------------------\n");
}

void usage(char *util)
{
    printf("Usage:\n");
    printf("%s -v - utility version\n", util);
    printf("%s -t <1, 2, 3, 8, 16, 17, 18> - SDR Type\n", util);
    printf("SDR Type:\n");
    printf("1    : Full Sensor Record\n");
    printf("2    : Compact Sensor Record\n");
    printf("3    : Event-Only Record\n");
    printf("8    : Entity Association Record\n");
    printf("16   : Generic Device Locator Record\n");
    printf("17   : FRU Device Locator Record\n");
    printf("18   : Management Controller Device locator Record\n");
    printf("%s -f <path/to.sdr.dat>\n", util);
    printf("%s -f <path/to.sdr.dat> > xxx.csv\n", util);
    return;
}

static void sen_specific_base_sensor(struct sdr_record_mask *mask, uint8_t sen_type, int assert)
{
    uint16_t event = 0;
    int len = (int)sizeof(sensor_specific_event_types) / sizeof(sensor_specific_event_types[0]);
    int i = 0, j = 0;
    bool has_event = false;

    if (assert == ASSERT_EVENT) {
        event = mask->type.discrete.assert_event;
    } else {
        event = mask->type.discrete.deassert_event;
    }

    for (i = 0; i < 16; i++) {
        /* If assert/deassert mask has set need find event description. */
        if (event & (1 << i)) {
            for (j = 0; j < len; j++) {
                if (sen_type == sensor_specific_event_types[j].code) {
                    if ((event & (1 << i)) & (1 << sensor_specific_event_types[j].offset)) {
                        printf("[%s]", sensor_specific_event_types[j].desc);
                        has_event = true;
                        break;
                    }
                }
            }
        }
    }

    if (has_event == false) {
        if (assert == ASSERT_EVENT) {
            printf("[no assert event]");
        } else {
            printf("[no de-assert event]");
        }
    }

    return;
}

static void generic_base_sensor(struct sdr_record_mask *mask, uint8_t event_type, int assert)
{
    uint16_t event = 0;
    int len = (int)sizeof(generic_event_types) / sizeof(generic_event_types[0]);
    int i = 0, j = 0;
    bool has_event = false;

    if (assert == ASSERT_EVENT) {
        event = mask->type.discrete.assert_event;
    } else {
        event = mask->type.discrete.deassert_event;
    }

    for (i = 0; i < 16; i++) {
        /* If assert/deassert mask has set need find event description. */
        if (event & (1 << i)) {
            for (j = 0; j < len; j++) {
                if (event_type == generic_event_types[j].code) {
                    if ((event & (1 << i)) & (1 << generic_event_types[j].offset)) {
                        printf("[%s]", generic_event_types[j].desc);
                        has_event = true;
                        break;
                    }
                }
            }
        }
    }

    if (has_event == false) {
        if (assert == ASSERT_EVENT) {
            printf("[no assert event]");
        } else {
            printf("[no de-assert event]");
        }
    }

    return;
}

static void threshold_base_sensor(struct sdr_record_mask *mask, int assert)
{
    bool has_event = false;

    printf("[");

    if (assert == ASSERT_EVENT) {
        if (mask->type.threshold.assert_unr_high) {
            printf("unr+ ");
            has_event = true;
        }

        if (mask->type.threshold.assert_unr_low) {
            printf("unr- ");
            has_event = true;
        }

        if (mask->type.threshold.assert_ucr_high) {
            printf("ucr+ ");
            has_event = true;
        }

        if (mask->type.threshold.assert_ucr_low) {
            printf("ucr- ");
            has_event = true;
        }

        if (mask->type.threshold.assert_unc_high) {
            printf("unc+ ");
            has_event = true;
        }

        if (mask->type.threshold.assert_unc_low) {
            printf("unc- ");
            has_event = true;
        }

        if (mask->type.threshold.assert_lnr_high) {
            printf("lnr+ ");
            has_event = true;
        }

        if (mask->type.threshold.assert_lnr_low) {
            printf("lnr- ");
            has_event = true;
        }

        if (mask->type.threshold.assert_lcr_high) {
            printf("lcr+ ");
            has_event = true;
        }

        if (mask->type.threshold.assert_lcr_low) {
            printf("lcr- ");
            has_event = true;
        }

        if (mask->type.threshold.assert_lnc_high) {
            printf("lnc+ ");
            has_event = true;
        }

        if (mask->type.threshold.assert_lnc_low) {
            printf("lnc- ");
            has_event = true;
        }

        if (has_event == false) {
            printf("no assert event");
        }
    } else {
        if (mask->type.threshold.deassert_unr_high) {
            printf("unr+ ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_unr_low) {
            printf("unr- ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_ucr_high) {
            printf("ucr+ ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_ucr_low) {
            printf("ucr- ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_unc_high) {
            printf("unc+ ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_unc_low) {
            printf("unc- ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_lnr_high) {
            printf("lnr+ ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_lnr_low) {
            printf("lnr- ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_lcr_high) {
            printf("lcr+ ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_lcr_low) {
            printf("lcr- ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_lnc_high) {
            printf("lnc+ ");
            has_event = true;
        }

        if (mask->type.threshold.deassert_lnc_low) {
            printf("lnc- ");
            has_event = true;
        }

        if (has_event == false) {
            printf("no de-assert event");
        }
    }

    printf("]");
    return;
}

static void base_unit_print(struct sdr_record_list *list)
{
    switch (list->rec_hdr.type) {
        case SDR_RECORD_TYPE_FULL_SENSOR:
            printf("%s", unit_desc[list->record.full->cmn.unit.type.base]);
            break;

        case SDR_RECORD_TYPE_COMPACT_SENSOR:
            printf("%s", unit_desc[list->record.compact->cmn.unit.type.base]);
            break;

        case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
        case SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_MC_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_ENTITY_ASSOC:
            break;
    }

    return;
}

double
sdr_convert_sensor_reading(struct sdr_record_full_sensor *sensor, uint8_t val)
{
	int m, b, k1, k2;
	double result;

	m = __TO_M(sensor->mtol);
	b = __TO_B(sensor->bacc);
	k1 = __TO_B_EXP(sensor->bacc);
	k2 = __TO_R_EXP(sensor->bacc);

	switch (sensor->cmn.unit.analog) {
	case 0:
		result = (double) (((m * val) +
				    (b * pow(10, k1))) * pow(10, k2));
		break;
	case 1:
		if (val & 0x80)
			val++;
		/* Deliberately fall through to case 2. */
	case 2:
		result = (double) (((m * (int8_t) val) +
				    (b * pow(10, k1))) * pow(10, k2));
		break;
	default:
		/* Oops! This isn't an analog sensor. */
		return 0.0;
	}

	switch (sensor->linearization & 0x7f) {
	case SDR_SENSOR_L_LN:
		result = log(result);
		break;
	case SDR_SENSOR_L_LOG10:
		result = log10(result);
		break;
	case SDR_SENSOR_L_LOG2:
		result = (double) (log(result) / log(2.0));
		break;
	case SDR_SENSOR_L_E:
		result = exp(result);
		break;
	case SDR_SENSOR_L_EXP10:
		result = pow(10.0, result);
		break;
	case SDR_SENSOR_L_EXP2:
		result = pow(2.0, result);
		break;
	case SDR_SENSOR_L_1_X:
		result = pow(result, -1.0);	/*1/x w/o exception */
		break;
	case SDR_SENSOR_L_SQR:
		result = pow(result, 2.0);
		break;
	case SDR_SENSOR_L_CUBE:
		result = pow(result, 3.0);
		break;
	case SDR_SENSOR_L_SQRT:
		result = sqrt(result);
		break;
	case SDR_SENSOR_L_CUBERT:
		result = cbrt(result);
		break;
	case SDR_SENSOR_L_LINEAR:
	default:
		break;
	}
	return result;
}

static void threshold_print(struct sdr_record_list *list)
{
    switch (list->rec_hdr.type) {
        case SDR_RECORD_TYPE_FULL_SENSOR:

            /* threshold base */
            if (list->record.full->cmn.event_type == 1) {
                
                printf("%.3f,", sdr_convert_sensor_reading(list->record.full ,list->record.full->threshold.lower.non_recover));
                printf("%.3f,", sdr_convert_sensor_reading(list->record.full ,list->record.full->threshold.lower.critical));
                printf("%.3f,", sdr_convert_sensor_reading(list->record.full ,list->record.full->threshold.lower.non_critical));
                printf("%.3f,", sdr_convert_sensor_reading(list->record.full ,list->record.full->threshold.upper.non_critical));
                printf("%.3f,", sdr_convert_sensor_reading(list->record.full ,list->record.full->threshold.upper.critical));
                printf("%.3f,", sdr_convert_sensor_reading(list->record.full ,list->record.full->threshold.upper.non_recover));
            }
            break;

        case SDR_RECORD_TYPE_COMPACT_SENSOR:
            printf("na,na,na,na,na,na,");
            break;

        case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
        case SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_MC_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_ENTITY_ASSOC:
            break;
    }

    return;
}

static void assert_event_print(struct sdr_record_list *list)
{
    switch (list->rec_hdr.type) {
        case SDR_RECORD_TYPE_FULL_SENSOR:

            /* threshold base */
            if (list->record.full->cmn.event_type == 1) {
                threshold_base_sensor(&list->record.full->cmn.mask, ASSERT_EVENT);

            /* sensor-specific base */
            } else if (list->record.full->cmn.event_type == 0x6f) {
                sen_specific_base_sensor(&list->record.full->cmn.mask, list->record.full->cmn.sensor.type, ASSERT_EVENT);

            /* generic base */
            } else if (list->record.full->cmn.event_type >= 0x02 || list->record.full->cmn.event_type <= 0x0c) {
                generic_base_sensor(&list->record.full->cmn.mask, list->record.full->cmn.event_type, ASSERT_EVENT);
            }

            break;

        case SDR_RECORD_TYPE_COMPACT_SENSOR:

            /* threshold base */
            if (list->record.compact->cmn.event_type == 1) {
                threshold_base_sensor(&list->record.compact->cmn.mask, ASSERT_EVENT);

            /* sensor-specific base */
            } else if (list->record.compact->cmn.event_type == 0x6f) {
                sen_specific_base_sensor(&list->record.compact->cmn.mask, list->record.compact->cmn.sensor.type, ASSERT_EVENT);

            /* generic base */
            } else if (list->record.compact->cmn.event_type >= 0x02 || list->record.compact->cmn.event_type <= 0x0c) {
                generic_base_sensor(&list->record.compact->cmn.mask, list->record.compact->cmn.event_type, ASSERT_EVENT);
            }

            break;

        case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
        case SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_MC_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_ENTITY_ASSOC:
            break;
    }

    return;
}

static void deassert_event_print(struct sdr_record_list *list)
{
    switch (list->rec_hdr.type) {
        case SDR_RECORD_TYPE_FULL_SENSOR:

            /* threshold base */
            if (list->record.full->cmn.event_type == 1) {
                threshold_base_sensor(&list->record.full->cmn.mask, DEASSERT_EVENT);

            /* sensor-specific base */
            } else if (list->record.full->cmn.event_type == 0x6f) {
                sen_specific_base_sensor(&list->record.full->cmn.mask, list->record.full->cmn.sensor.type, DEASSERT_EVENT);

            /* generic base */
            } else if (list->record.full->cmn.event_type >= 0x02 && list->record.full->cmn.event_type <= 0x0c) {
                generic_base_sensor(&list->record.full->cmn.mask, list->record.full->cmn.event_type, DEASSERT_EVENT);
            }

            break;

        case SDR_RECORD_TYPE_COMPACT_SENSOR:

            /* threshold base */
            if (list->record.compact->cmn.event_type == 1) {
                threshold_base_sensor(&list->record.compact->cmn.mask, DEASSERT_EVENT);

            /* sensor-specific base */
            } else if (list->record.compact->cmn.event_type == 0x6f) {
                sen_specific_base_sensor(&list->record.compact->cmn.mask, list->record.compact->cmn.sensor.type, DEASSERT_EVENT);

            /* generic base */
            } else if (list->record.compact->cmn.event_type >= 0x02 || list->record.compact->cmn.event_type <= 0x0c) {
                generic_base_sensor(&list->record.compact->cmn.mask, list->record.compact->cmn.event_type, DEASSERT_EVENT);
            }

            break;

        case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
        case SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_MC_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_ENTITY_ASSOC:
            break;
    }

    return;
}

static int sdr_rec_print(int sdr_type, int total_sdr_rec)
{
    struct sdr_record_list *list, *next;

    printf("RecID,SenNum,SenName,SenType,EventType,AssertEvent,DeassertEvent,LNR,LC,LNC,UNC,UC,UNR,Unit\n");
    for (list = sdr_list_head; list != NULL; list = next) {
        if (sdr_type == list->rec_hdr.type || sdr_type == SDR_RECORD_TYPE_ALL) {
            switch(list->rec_hdr.type) {
                case SDR_RECORD_TYPE_FULL_SENSOR:
                    printf("%d,%d,%s,%s,0x%x,",
                           list->rec_hdr.id,
                           list->record.full->cmn.keys.sensor_num, list->record.full->id_string,
                           sensor_type_desc[list->record.full->cmn.sensor.type],
                           list->record.full->cmn.event_type);

                    /* Assert event mask */
                    assert_event_print(list);
                    printf(",");

                    /* Deassert event mask */
                    deassert_event_print(list);
                    printf(",");

                    /* Threshold */
                    threshold_print(list);

                    /* Base Unit */
                    base_unit_print(list);
                    printf("\n");
                    break;

                case SDR_RECORD_TYPE_COMPACT_SENSOR:
                    printf("%d,%d,%s,%s,0x%x,",
                           list->rec_hdr.id,
                           list->record.compact->cmn.keys.sensor_num, list->record.compact->id_string,
                           sensor_type_desc[list->record.compact->cmn.sensor.type],
                           list->record.compact->cmn.event_type);

                    /* Assert event mask */
                    assert_event_print(list);
                    printf(",");

                    /* Deassert event mask */
                    deassert_event_print(list);
                    printf(",");

                    /* Threshold */
                    threshold_print(list);

                    /* Base Unit */
                    base_unit_print(list);
                    printf("\n");
                    break;

                case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
                    printf("%d,%d,%s,%s,0x%x,na,na,na,na,na,na,na,na,na",
                           list->rec_hdr.id,
                           list->record.eventonly->keys.sensor_num, list->record.eventonly->id_string,
                           sensor_type_desc[list->record.eventonly->sensor_type],
                           list->record.eventonly->event_type);

                    break;

                case SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR:
                    printf("%d,na,%s,na,na,na,na,na,na,na,na,na,na,na\n",
                           list->rec_hdr.id, list->record.genloc->id_string);
                    break;

                case SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR:
                    printf("%d,na,%s,na,na,na,na,na,na,na,na,na,na,na\n",
                           list->rec_hdr.id, list->record.fruloc->id_string);
                    break;

                case SDR_RECORD_TYPE_MC_DEVICE_LOCATOR:
                    printf("%d,na,%s,na,na,na,na,na,na,na,na,na,na,na\n",
                           list->rec_hdr.id, list->record.mcloc->id_string);
                    break;

                case SDR_RECORD_TYPE_ENTITY_ASSOC:
                    printf("%d,na,na,na,na,na,na,na,na,na,na,na,na,na\n", list->rec_hdr.id);
                    break;
            }
        }

        next = list->next;
    }

    return 0;
}

static int sdr_list_clear(void)
{
    struct sdr_record_list *list, *next;

    for (list = sdr_list_head; list != NULL; list = next) {
        switch(list->rec_hdr.type) {
            case SDR_RECORD_TYPE_FULL_SENSOR:
                if (list->record.full) {
                    free(list->record.full);
                    list->record.full = NULL;
                }

                break;

            case SDR_RECORD_TYPE_COMPACT_SENSOR:
                if (list->record.compact) {
                    free(list->record.compact);
                    list->record.compact = NULL;
                }

                break;

            case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
                if (list->record.eventonly) {
                    free(list->record.eventonly);
                    list->record.eventonly = NULL;
                }

                break;

            case SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR:
                if (list->record.genloc) {
                    free(list->record.genloc);
                    list->record.genloc = NULL;
                }

                break;

            case SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR:
                if (list->record.fruloc) {
                    free(list->record.fruloc);
                    list->record.fruloc = NULL;
                }

                break;

            case SDR_RECORD_TYPE_MC_DEVICE_LOCATOR:
                if (list->record.mcloc) {
                    free(list->record.mcloc);
                    list->record.mcloc = NULL;
                }

                break;

            case SDR_RECORD_TYPE_ENTITY_ASSOC:
                if (list->record.entassoc) {
                    free(list->record.entassoc);
                    list->record.entassoc = NULL;
                }

                break;
        }

        if (list->raw) {
            free(list->raw);
            list->raw = NULL;
        }

        next = list->next;
        free(list);
        list = NULL;
    }

    sdr_list_head = NULL;
    sdr_list_tail = NULL;
    return 0;
}

static int sdr_rec_parser(int sdr_type, char *pData, int file_size, int *total_sdr_rec)
{
    struct ami_sdr_hdr {
        char valid;
        char len;
    } ATTRIBUTE_PACKING;

    int i = 0;
    struct ami_sdr_hdr *pSDRHdr = NULL;
    struct sdr_record_list *sdrr = NULL;
    char *rec = NULL;

    while (i + sizeof(struct ami_sdr_hdr) < file_size) {
        pSDRHdr = (struct ami_sdr_hdr *)&pData[i];
        i += sizeof(struct ami_sdr_hdr);

        /* Find AMI SDR Header */
        if (pSDRHdr->valid == AMI_SDR_VALID) {

            /* Copy IPMI SDR data to list */
            sdrr = malloc(sizeof(struct sdr_record_list));
            if (sdrr == NULL) {
                printf("Allocate SDR record fail\n");
                return -1;
            }

            /* Copy SDR header */
            memset(sdrr, 0, sizeof(struct sdr_record_list));
            memcpy(&sdrr->rec_hdr, &pData[i], sizeof(struct sdr_record_header));

            /* Copy SDR raw data except SDR header. */
            sdrr->raw = malloc(sdrr->rec_hdr.length);
            memcpy(sdrr->raw, &pData[i + sizeof(struct sdr_record_header)], sdrr->rec_hdr.length);

            rec = malloc(sdrr->rec_hdr.length + 1);
            memset(rec, 0, sdrr->rec_hdr.length + 1);
            memcpy(rec, &sdrr->raw[0], sdrr->rec_hdr.length);

            switch(sdrr->rec_hdr.type) {
                case SDR_RECORD_TYPE_FULL_SENSOR:
                    sdrr->record.full = (struct sdr_record_full_sensor *)rec;
                    break;

                case SDR_RECORD_TYPE_COMPACT_SENSOR:
                    sdrr->record.compact = (struct sdr_record_compact_sensor *)rec;
                    break;

                case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
                    sdrr->record.eventonly = (struct sdr_record_eventonly_sensor *)rec;
                    break;

                case SDR_RECORD_TYPE_ENTITY_ASSOC:
                    sdrr->record.entassoc = (struct sdr_record_entity_assoc *)rec;
                    break;

                case SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR:
                    sdrr->record.genloc = (struct sdr_record_generic_locator *)rec;
                    break;

                case SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR:
                    sdrr->record.fruloc = (struct sdr_record_fru_locator *)rec;
                    break;

                case SDR_RECORD_TYPE_MC_DEVICE_LOCATOR:
                    sdrr->record.mcloc = (struct sdr_record_mc_locator *)rec;
                    break;
            }

            if (sdr_list_head == NULL) {
                sdr_list_head = sdrr;
            } else {
                sdr_list_tail->next = sdrr;
            }

            sdr_list_tail = sdrr;

            *total_sdr_rec = *total_sdr_rec + 1;
        }
    }

    return 0;
}

static int sdr_type_id_get(char *type_str)
{
    int sdr_type = 0;

    sdr_type = strtol(type_str, (char **)NULL, 10);

    switch (sdr_type) {
        case SDR_RECORD_TYPE_FULL_SENSOR:
        case SDR_RECORD_TYPE_COMPACT_SENSOR:
        case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
        case SDR_RECORD_TYPE_ENTITY_ASSOC:
        case SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR:
        case SDR_RECORD_TYPE_MC_DEVICE_LOCATOR:
            return sdr_type;

        default:
            return -1;
    }

    return -1;
}

int main(int argc, char *argv[])
{
    int fd = 0;
    int cmd_opt = 0;
    int file_size = 0;
    char type[8];
    char file[256];
    char *pData = NULL;
    bool TypeFlag = false;
    int sdr_type_id = 0;
    int total_sdr_rec = 0;

    if (argc < 2) {
        usage(argv[0]);
        return -1;
    }

    memset(type, '\0', sizeof(type));
    memset(file, '\0', sizeof(file));

    while (1) {
        cmd_opt = getopt(argc, argv, "t:f:v");

        if (cmd_opt == -1) {
            break;
        }

        switch(cmd_opt) {
            case 't':
                snprintf(type, sizeof(type), "%s", optarg);
                TypeFlag = true;
                break;
            case 'f':
                snprintf(file, sizeof(file), "%s", optarg);
                break;
            case 'v':
                PrintCopyright();
                return 0;
            default:
                usage(argv[0]);
                return -1;
        }
    }

    /* Check SDR Type */
    if (TypeFlag == true) {
        if (strlen(type) <= 0) {
            usage(argv[0]);
            return -1;
        }

        sdr_type_id = sdr_type_id_get(type);
        if (sdr_type_id == -1) {
            usage(argv[0]);
            return -1;
        }
    } else {
        sdr_type_id = SDR_RECORD_TYPE_ALL;
    }

    /* Check SDR file */
    if (strlen(file) <= 0) {
        usage(argv[0]);
        return -1;
    } else {
        if (access(file, F_OK) != 0) {
            printf("Please check file is exist file %s.\n",file);
            return -1;
        }
    }

    /* Open SDR binary file to parsing. */
    fd = open(file, O_RDONLY);
    if (fd < 0) {
        printf("Can't open file.\n");
        goto SDR_END;
    }

    /* Check file size. */
    file_size = lseek(fd, 0, SEEK_END);
    //printf("File size = %d\n", file_size);

    /* Allocate array of file size. */
    pData = malloc(file_size);
    if (pData == NULL) {
        printf("Allocate array fail\n");
        goto SDR_END;
    }

    /* Read all data to arrary buffer */
    lseek(fd, 0, SEEK_SET);
    if (read(fd, pData, file_size) != file_size) {
        printf("Read fail\n");
        goto SDR_END;
    }

    /* Start to parsing SDR table follow SDR type. */
    if (sdr_rec_parser(sdr_type_id, pData, file_size, &total_sdr_rec) < 0) {
        printf("Parsing SDR data fail\n");
        goto SDR_END;
    }

    sdr_rec_print(sdr_type_id, total_sdr_rec);
    //printf("Total SDR Rec = %d\n", total_sdr_rec);

SDR_END:

    sdr_list_clear();

    if (pData != NULL) {
        free(pData);
        pData = NULL;
    }

    if (fd != 0) {
        close(fd);
        fd = 0;
        //printf("Close file\n");
    }

    return 0;
}

