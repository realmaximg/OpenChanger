import { ids } from './api';
import type { KitKind } from './kits';

const cameraByWeapon: Record<string, string> = {
  weapon_awp: '7',
  weapon_aug: '3',
  weapon_sg556: '4',
  weapon_ssg08: '6',
  weapon_ak47: '4',
  weapon_m4a1_silencer: '6',
  weapon_famas: '4',
  weapon_g3sg1: '5',
  weapon_galilar: '3',
  weapon_m4a1: '4',
  weapon_scar20: '5',
  weapon_mp5sd: '3',
  weapon_xm1014: '4',
  weapon_m249: '6',
  weapon_ump45: '3',
  weapon_bizon: '3',
  weapon_mag7: '3',
  weapon_nova: '5',
  weapon_sawedoff: '3',
  weapon_negev: '5',
  weapon_usp_silencer: '2',
  weapon_elite: '2',
  weapon_tec9: '2',
  weapon_revolver: '2',
  weapon_taser: '0',
};

const cameraByCategory: Record<string, string> = { secondary: '0', smg: '2' };

export function localized(token: string, fallback: string) {
  const text = $.Localize(token);
  return text && !text.startsWith('#') ? text : fallback;
}

export function fauxId(definition: number, paint: number) {
  return InventoryAPI.GetFauxItemIDFromDefAndPaintIndex(definition, paint);
}

export function kitId(kind: KitKind, kit: number) {
  return fauxId(InventoryAPI.GetItemDefinitionIndexFromDefinitionName(kind), kit);
}

export function hasImage(id: string) {
  return InventoryAPI.GetItemInventoryImage(id) !== '';
}

export function itemName(id: string) {
  return InventoryAPI.GetItemName(id);
}

export function baseName(id: string) {
  return InventoryAPI.GetItemNameUncustomized(id);
}

export function rarity(id: string) {
  return InventoryAPI.GetItemRarity(id);
}

export function rarityColor(id: string) {
  return InventoryAPI.GetItemRarityColor(id);
}

export function typeOf(id: string) {
  return InventoryAPI.GetTag(id, 'Type');
}

export function typeName(type: string) {
  return type ? localized(`#CSGO_Type_${type}`, type) : '';
}

export function setOf(id: string) {
  return InventoryAPI.GetSet(id);
}

export function setName(set: string) {
  return set ? localized(`#CSGO_${set}`, set) : '';
}

export function musicToken(kit: number) {
  return InventoryAPI.GetMusicNameFromMusicID(kit);
}

export function musicImage(kit: number) {
  return `file://{images}/${InventoryAPI.GetItemInventoryImageFromMusicID(kit)}.png`;
}

export function teamCode(team: number) {
  return team === ids.CounterTerrorists ? 'ct' : 't';
}

export function equippedId(team: number, slot: string) {
  return LoadoutAPI.GetItemID(teamCode(team), slot);
}

export function loadoutWeapons(team: number) {
  return ItemInfo.GetLoadoutWeapons(teamCode(team));
}

export function weaponCamera(id: string, farther: number) {
  const name = InventoryAPI.GetItemDefinitionName(id);
  const camera = Number(cameraByWeapon[name] ?? cameraByCategory[InventoryAPI.GetLoadoutCategory(id)] ?? '3');
  return `cam_${Math.min(camera + farther, 7)}`;
}

export function backgrounds() {
  return GameInterfaceAPI.GetSettingInfo('ui_inspect_bkgnd_map').allowed_values;
}

export function background() {
  return GameInterfaceAPI.GetSettingString('ui_inspect_bkgnd_map');
}

export function setBackground(value: string) {
  GameInterfaceAPI.SetSettingString('ui_inspect_bkgnd_map', value);
}

export function backgroundMap() {
  const value = background();
  return `${value === 'mainmenu' ? GameInterfaceAPI.GetSettingString('ui_mainmenu_bkgnd_movie') : value}_vanity`;
}

export function backgroundName(value: string) {
  return localized(`#SFUI_Map_${value}`, value);
}
