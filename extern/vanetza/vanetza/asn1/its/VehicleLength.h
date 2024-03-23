/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "asn1/TS102894-2v131-CDD.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -no-gen-example -R`
 */

#ifndef	_VehicleLength_H_
#define	_VehicleLength_H_


#include "asn_application.h"

/* Including external dependencies */
#include "VehicleLengthValue.h"
#include "VehicleLengthConfidenceIndication.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* VehicleLength */
typedef struct VehicleLength {
	VehicleLengthValue_t	 vehicleLengthValue;
	VehicleLengthConfidenceIndication_t	 vehicleLengthConfidenceIndication;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} VehicleLength_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_VehicleLength;
extern asn_SEQUENCE_specifics_t asn_SPC_VehicleLength_specs_1;
extern asn_TYPE_member_t asn_MBR_VehicleLength_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _VehicleLength_H_ */
#include "asn_internal.h"
