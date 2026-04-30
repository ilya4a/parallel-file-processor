//
// Created by ilya on 4/30/26.
//

#ifndef FREP_DEBUG_H
#define FREP_DEBUG_H

#ifdef FREP_DEBUG
#include <iostream>
#define DEBUG_LOG(msg) do {std::cerr << "[DEBUG] " << msg << '\n'; } while(false)
#else
#define DEBUG_LOG(msg) do {} while (false)
#endif

#endif //FREP_DEBUG_H
