/*
 * ControllableObject.h
 *
 *  Created on: Aug 7, 2020
 *      Author: sts
 */

#pragma once

#include "artery/traci/MovingNodeController.h"


class ControllableObject {
public:
    virtual ~ControllableObject() = default;
    virtual traci::MovingNodeController* getControllerBase() = 0;

    template<typename T>
    T* getController() {
           T* _c = dynamic_cast<T*>(getControllerBase());
           if (!_c)
               throw std::invalid_argument( "cannot cast" );
           return _c;
    }
};


