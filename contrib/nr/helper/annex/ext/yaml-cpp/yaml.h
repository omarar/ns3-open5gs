#ifndef YAML_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define YAML_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include "ns3/parser.h"
#include "ns3/emitter.h"
#include "ns3/emitterstyle.h"
#include "ns3/stlemitter.h"
#include "ns3/exceptions.h"

#include "ns3/node.h"
#include "ns3/impl.h"
#include "ns3/convert.h"
#include "ns3/iterator.h"
#include "../../../ns-3-dev/contrib/nr/helper/annex/ext/yaml-cpp/node/detail/impl.h"
#include "ns3/parse.h"
#include "ns3/emit.h"

#endif  // YAML_H_62B23520_7C8E_11DE_8A39_0800200C9A66
