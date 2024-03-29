/**
 * @file cpp_unit.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#ifndef CPP_UNIT_HPP
#define CPP_UNIT_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <unordered_map>

#include "mongoose/mongoose.h"

#include "glog/logging.h"       // logging
#include "gflags/gflags.h"      // argument
#include "yaml-cpp/yaml.h"      // yaml parser
#include "nlohmann/json.hpp"    // json parser

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"            // http tool

#include "boost/filesystem.hpp" // boost
#include "boost/beast.hpp"
#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#include "opencv4/opencv2/opencv.hpp"


#endif  // CPP_UNIT_HPP