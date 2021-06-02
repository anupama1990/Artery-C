/*
 * NodeManager.cc
 *
 *  Created on: Oct 12, 2020
 *      Author: sts
 */

#include "traci/NodeManager.h"


namespace traci {

 void NodeManager::visit(ITraciNodeVisitor *visitor) const{
     for (const auto& entry : m_nodes) {
         visitor->visitNode(entry.first, entry.second);
     }
 }

}
