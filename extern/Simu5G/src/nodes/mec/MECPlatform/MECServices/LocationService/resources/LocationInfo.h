//
//                  Simu5G
//
// Authors: Giovanni Nardini, Giovanni Stea, Antonio Virdis (University of Pisa)
//
// This file is part of a software released under the license included in file
// "license.pdf". Please read LICENSE and README files before using it.
// The above files and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef CORENETWORK_NODES_MEC_MECPLATFORM_MESERVICES_LOCATIONSERVICE_RESOURCES_LOCATIONINFO_H_
#define CORENETWORK_NODES_MEC_MECPLATFORM_MESERVICES_LOCATIONSERVICE_RESOURCES_LOCATIONINFO_H_

#include "nodes/mec/MECPlatform/MECServices/Resources/AttributeBase.h"
#include "inet/common/geometry/common/Coord.h"

class LocationInfo : public AttributeBase
{
    public:
        LocationInfo();
        LocationInfo(const inet::Coord& coordinates, const inet::Coord& speed);
        LocationInfo(const inet::Coord& coordinates);
        virtual ~LocationInfo();
        nlohmann::ordered_json toJson() const override;

    private:
        inet::Coord coordinates_;
        inet::Coord speed_;
};


#endif /* CORENETWORK_NODES_MEC_MECPLATFORM_MESERVICES_LOCATIONSERVICE_RESOURCES_LOCATIONINFO_H_ */
