/// <reference path="./pano-typed/index.d.ts" />

declare interface AbstractPanel<PanelName extends keyof PanelTagNameMap> {
  BHasClass(classname: string): boolean;
  BIsTransparent(): boolean;
  SetSendScrollPositionChangedEvents(send: boolean): void;
}

declare namespace $ {
  function RegisterEventHandler(
    event: 'AnimationStart' | 'AnimationEnd',
    context: GenericPanel,
    callback: (panel: GenericPanel, animationName: string) => void,
  ): void;
  function RegisterEventHandler(
    event: 'PropertyTransitionEnd',
    context: GenericPanel,
    callback: (panel: GenericPanel, propertyName: string) => boolean | undefined,
  ): void;
}

declare namespace MainMenu {
  function NavigateToTab(tab: string, layout: string): void;
  function OnHomeButtonPressed(): void;
}

interface GlobalEventNameMap {
  MainMenuTabShown: (tab: string) => void;
}

declare namespace InventoryAPI {
  function GetFauxItemIDFromDefAndPaintIndex(definition: number, paint: number): string;
  function GetItemDefinitionIndexFromDefinitionName(name: string): number;
  function GetItemName(id: string): string;
  function GetItemNameUncustomized(id: string): string;
  function GetItemDefinitionName(id: string): string;
  function GetLoadoutCategory(id: string): string;
  function GetItemRarity(id: string): number;
  function GetItemRarityColor(id: string): string;
  function GetTag(id: string, tag: string): string;
  function GetSet(id: string): string;
  function GetMusicNameFromMusicID(kit: number): string;
  function GetItemInventoryImageFromMusicID(kit: number): string;
  function GetItemInventoryImage(id: string): string;
  function PlayItemPreviewMusic(id: string, type: string): void;
  function StopItemPreviewMusic(): void;
  function PreviewStickerInModelPanel(sticker: string, slot: number, panel: GenericPanel): void;
  function OnNextStickerButtonPressed(panel: GenericPanel): void;
  function SetStickerScrapeLevel(panel: GenericPanel, level: number): void;
  function GetCharacterDefaultCheerByItemId(id: string): string;
  function GetCharacterDefaultDefeatByItemId(id: string): string;
  function PreviewModelVoice(id: string): void;
}

declare namespace LoadoutAPI {
  function GetItemID(team: string, slot: string): string;
}

declare namespace ItemInfo {
  function GetLoadoutWeapons(team: string): [string, string][];
}

declare namespace CharacterAnims {
  type Settings = {
    panel: GenericPanel;
    team: string;
    charItemId: string;
    weaponItemId: string;
    glovesItemId: string;
    petItemId: string;
    cameraPreset: number;
    cheer?: string;
  };
  function PlayAnimsOnPanel(settings: Settings, dontStompModel?: boolean, makeDeepCopy?: boolean): void;
}

declare namespace GameInterfaceAPI {
  function GetSettingInfo(key: string): { allowed_values: string[] };
}

declare interface MapPreviewPanel extends Panel {
  SetActiveItem(index: number): void;
  SetItemItemId(id: string, extra: string): void;
  SetActiveCharacter(index: number): void;
  TransitionToCamera(camera: string, seconds: number): void;
  StartWeaponLookat(): void;
  RemoveItemModel(): void;
  RemoveCharacterModel(): void;
  SetAcceptsFocus(accepts: boolean): void;
}
