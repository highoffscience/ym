// /**
//  * @file    subscriber.h
//  * @version 1.0.0
//  * @author  Forrest Jablonski
//  */

// #pragma once

// #include "ymdefs.h"

// #include <memory>

// namespace ym
// {

// /** Subscriber
//  * 
//  * @brief TODO
//  */
// class Subscriber
// {
//    friend class Publisher;

// public:
//    explicit Subscriber(void);

//    virtual bool receive(void const * const Payload_Ptr) = 0;

// private:
//    std::shared_ptr<class Subscriber> _self_sptr;
// };

// } // ym
