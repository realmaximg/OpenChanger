#include "sdk/client/CModel.h"

#include "sdk/tier1/CBufferString.h"

#include <cstdint>

std::int32_t CModel::getHiddenGlovesChoice(std::int32_t Group,
                                           std::int32_t Choice) {
  if (Choice == FindBodyGroupChoice(this, Group, &ThirdPersonDefault())) {
    return FindBodyGroupChoice(this, Group, &ThirdPersonHidden());
  }
  if (Choice == FindBodyGroupChoice(this, Group, &FirstPersonDefault())) {
    return FindBodyGroupChoice(this, Group, &FirstPersonHidden());
  }
  return Choice;
}

std::int32_t CModel::getLegacyChoice(std::int32_t Group) {
  for (std::int32_t Choice = 0; Choice < GetBodyGroupChoiceCount(this, Group);
       ++Choice) {
    CBufferStringN<0x10> Name;
    GetBodyGroupChoiceName(this, Group, Choice, &Name.m_String);
    if (CBufferString::EndsWith_FastCaseInsensitive(&Name.m_String,
                                                    "_legacy")) {
      return Choice;
    }
  }
  return -1;
}
