/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "asn1/ISO_TS_19091_CPM.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -no-gen-example -R`
 */

#ifndef	_Node_LLmD_64b_H_
#define	_Node_LLmD_64b_H_


#include "asn_application.h"

/* Including external dependencies */
#include "Longitude.h"
#include "Latitude.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Node-LLmD-64b */
typedef struct Node_LLmD_64b {
	Longitude_t	 lon;
	Latitude_t	 lat;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Node_LLmD_64b_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Node_LLmD_64b;
extern asn_SEQUENCE_specifics_t asn_SPC_Node_LLmD_64b_specs_1;
extern asn_TYPE_member_t asn_MBR_Node_LLmD_64b_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _Node_LLmD_64b_H_ */
#include "asn_internal.h"
