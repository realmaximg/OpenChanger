#pragma once

#include "sdk/client/CEntityInstance.h"
#include "sdk/client/CModel.h"
#include "sdk/macros.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"
#include "sdk/resourcesystem/CStrongHandle.h"
#include "sdk/resourcesystem/ResourceBinding_t.h"
#include "sdk/tier1/CUtlVector.h"

#include <cstdint>

#define PtSetModel                                                             \
  "48 89 6C 24 20 57 48 83 EC 20 33 ED 48 89 5C 24 38 48 8B F9 8B DD 48 85 "   \
  "D2 74 07 48 8B DA F0 FF 42 20 48 8B 91 ? ? ? ? 48 3B D3"
#define PtSkeletonModelChanged "49 8B 00 48 89 81 ? ? ? ? C6 81 ? ? ? ? 01 C3"
#define PtSkeletonMeshGroupMaskChanged                                         \
  "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 49 8B 00 49 8B F8 48 8B F2 "   \
  "48 8B D9 48 39 81 ? ? ? ? 74 69"
#define PtBodyGroupChoiceChanged                                               \
  "48 89 5C 24 08 57 48 83 EC 20 49 63 D8 49 8B F9 45 85 C0 78 20 3B 99 ? ? "  \
  "? ? 7D 18"

class CModelState {
public:
  SDK_CLASS(CModelState);

  SCHEMA_FIELD(CStrongHandle<CModel>, m_hModel);
  SCHEMA_FIELD(std::uint64_t, m_MeshGroupMask);
  SCHEMA_FIELD(CUtlVector<std::int32_t>, m_nBodyGroupChoices);

  PATTERN_FUNCTION(Client, void(CModelState *, ResourceBinding_t *), SetModel,
                   PtSetModel);
  PATTERN_FUNCTION(Client,
                   void(CModelState *, CEntityInstance *, ResourceBinding_t **),
                   SkeletonModelChanged, PtSkeletonModelChanged);
  PATTERN_FUNCTION(Client,
                   void(CModelState *, CEntityInstance *, std::uint64_t *),
                   SkeletonMeshGroupMaskChanged,
                   PtSkeletonMeshGroupMaskChanged);
  PATTERN_FUNCTION(Client,
                   void(CModelState *, CEntityInstance *, std::int32_t,
                        std::int32_t *),
                   BodyGroupChoiceChanged, PtBodyGroupChoiceChanged);
};

class ModelChanges {
public:
  ModelChanges(CNetworkFieldChangedEventQueue *Queue, CEntityInstance &Entity);

  [[nodiscard]] bool sent() const { return State != nullptr; }
  void setModel(const char *Name);
  void hideDefaultGloves();
  void useLegacyModel();

private:
  CModel *model();
  std::uint64_t &mask();
  std::int32_t *choice(std::int32_t Group);

  CNetworkFieldChangedEventQueue *Queue;
  CEntityInstance &Entity;
  CModelState *State = nullptr;
  ResourceBinding_t **SentModel = nullptr;
  std::uint64_t *SentMask = nullptr;
};
