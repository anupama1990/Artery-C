/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "asn1/TS102894-2v131-CDD.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -no-gen-example -R`
 */

#ifndef	_VehicleIdentification_H_
#define	_VehicleIdentification_H_


#include "asn_application.h"

/* Including external dependencies */
#include "WMInumber.h"
#include "VDS.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* VehicleIdentification */
typedef struct VehicleIdentification {
	WMInumber_t	*wMInumber	/* OPTIONAL */;
	VDS_t	*vDS	/* OPTIONAL */;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} VehicleIdentification_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_VehicleIdentification;

#ifdef __cplusplus
}
#endif

#endif	/* _VehicleIdentification_H_ */
#include "asn_internal.h"
