#pragma once

#include "sdk/tier1/CUtlVector.h"

#include <cstddef>
#include <cstdint>

template <class K, class V> struct CUtlMap {
  struct Node {
    std::int32_t m_Left;
    std::int32_t m_Right;
    std::int32_t m_Parent;
    std::int32_t m_Tag;
    K m_Key;
    V m_Value;
  };

  void *m_LessFunc;
  CUtlVector<Node> m_Elements;

  template <class F> void forEach(F Visit) {
    std::int32_t Index = 0;
    for (const Node &Element : m_Elements.span()) {
      if (Element.m_Left != Index) {
        Visit(Element.m_Key, Element.m_Value);
      }
      ++Index;
    }
  }
};

using CUtlMapCheck = CUtlMap<std::int32_t, void *>;
static_assert(offsetof(CUtlMapCheck, m_Elements) == 0x8);
static_assert(sizeof(CUtlMapCheck::Node) == 0x20);
static_assert(offsetof(CUtlMapCheck::Node, m_Key) == 0x10);
static_assert(offsetof(CUtlMapCheck::Node, m_Value) == 0x18);
