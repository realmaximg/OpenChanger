#pragma once

#include "sdk/macros.h"
#include "sdk/tier1/CUtlVector.h"

#include <array>
#include <cstddef>
#include <cstdint>

template <class K, class V> struct CUtlHashMapLarge {
  struct Node {
    K m_key;
    V m_value;
    std::int32_t m_iNext;
  };

  PAD(0x20);
  CUtlVector<Node> m_memNodes;

  template <class F> void forEach(F Visit) {
    for (const Node &Element : m_memNodes.span()) {
      if (Element.m_iNext >= -1) {
        Visit(Element.m_key, Element.m_value);
      }
    }
  }
};

using CUtlHashMapLargeCheck = CUtlHashMapLarge<std::int32_t, void *>;
static_assert(offsetof(CUtlHashMapLargeCheck, m_memNodes) == 0x20);
static_assert(sizeof(CUtlHashMapLargeCheck::Node) == 0x18);
static_assert(offsetof(CUtlHashMapLargeCheck::Node, m_value) == 0x8);
static_assert(offsetof(CUtlHashMapLargeCheck::Node, m_iNext) == 0x10);
