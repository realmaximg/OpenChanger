import { createSignal } from 'solid-js';
import type { MenuItem } from '../../components/Menu/menu';
import { t } from '../../i18n';
import type { Entry } from './catalog';
import { draft, draftId } from './editor';
import * as game from './game';
import { teamsOf } from './store';

type Weapon = { name: string; id: string };

const [zoomed, setZoomed] = createSignal(false);
const [weapon, chooseWeapon] = createSignal(1);
const [background, setBackground] = createSignal(game.background());
let panel: MapPreviewPanel | undefined;

export { background, chooseWeapon, weapon, zoomed };

export function usePanel(next: MapPreviewPanel | undefined) {
  panel = next;
}

export function side(entry: Entry) {
  return entry.kind === 'agent' ? teamsOf(entry)[0] : (draft()?.Team ?? teamsOf(entry)[0]);
}

export function weapons(team: number): Weapon[] {
  const list = game
    .loadoutWeapons(team)
    .filter(([slot]) => slot !== 'melee' && slot !== 'c4' && !slot.startsWith('equipment'))
    .map(([, id]) => ({ name: game.itemName(id), id }));
  return [{ name: t.noWeapon, id: '' }, ...list];
}

export function players(entry: Entry, cheer?: string): CharacterAnims.Settings | undefined {
  if (!panel) return undefined;
  const team = side(entry);
  const agent = entry.kind === 'agent';
  return {
    panel,
    team: game.teamCode(team),
    charItemId: agent ? draftId : game.equippedId(team, 'customplayer'),
    weaponItemId: agent ? (weapons(team)[weapon()]?.id ?? '') : draftId,
    glovesItemId: entry.kind === 'gloves' ? draftId : game.equippedId(team, 'clothing_hands'),
    petItemId: '',
    cameraPreset: zoomed() ? 27 : 26,
    cheer,
  };
}

export function cheer(entry: Entry, defeat: boolean) {
  const sequence = defeat
    ? InventoryAPI.GetCharacterDefaultDefeatByItemId(draftId)
    : InventoryAPI.GetCharacterDefaultCheerByItemId(draftId);
  setZoomed(false);
  const settings = players(entry, sequence);
  if (sequence && settings) CharacterAnims.PlayAnimsOnPanel(settings);
}

export function voice() {
  InventoryAPI.PreviewModelVoice(draftId);
}

export function zoom() {
  setZoomed(!zoomed());
  panel?.TransitionToCamera(zoomed() ? 'cam_char_inspect_closeup' : 'cam_char_inspect_wide', 0.5);
}

export function anthem() {
  InventoryAPI.StopItemPreviewMusic();
  InventoryAPI.PlayItemPreviewMusic(draftId, 'MVPPreview');
}

export function backgroundItems(): MenuItem[] {
  return game.backgrounds().map((value) => ({
    label: game.backgroundName(value),
    current: value === background(),
    on: () => {
      game.setBackground(value);
      setBackground(value);
    },
  }));
}

export function nextPosition() {
  if (panel) InventoryAPI.OnNextStickerButtonPressed(panel);
}

export function scrape(wear: number) {
  if (panel) InventoryAPI.SetStickerScrapeLevel(panel, Math.round(wear * 100));
}
