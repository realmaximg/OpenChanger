import { createSignal } from 'solid-js';
import { type KitKind as AnyKitKind, api, type Kit } from './api';
import { compare, mode, showKits } from './browser';
import { cached } from './catalog';
import * as game from './game';

export type KitKind = Exclude<AnyKitKind, 'music'>;
type KitSort = { field: 'name' | 'rarity'; descending: boolean };

const lists = new Map<KitKind, Kit[]>();
const names = new Map<string, string>();
const rarities = new Map<string, number>();

const [kind, setKind] = createSignal<KitKind>('sticker');
const [query, setQuery] = createSignal('');
const [sort, setSort] = createSignal<KitSort>({ field: 'name', descending: false });
const [fresh, setFresh] = createSignal(false);

export { fresh, kind, query, sort };

export function openKits(next: KitKind) {
  setKind(next);
  setQuery('');
  setFresh(true);
  showKits();
}

export function pickingKits(of: KitKind) {
  return mode() === 'kits' && kind() === of;
}

export function searchKits(text: string) {
  setFresh(false);
  setQuery(text);
}

export function sortKits(field: KitSort['field']) {
  setFresh(false);
  setSort({ field, descending: field === 'rarity' });
}

export function flipKits() {
  setFresh(false);
  setSort({ ...sort(), descending: !sort().descending });
}

export function kitName(of: KitKind, kit: number) {
  return cached(names, `${of}:${kit}`, () => {
    const part = game.itemName(game.kitId(of, kit)).split(' | ')[1] ?? '';
    return part.startsWith('#') ? '' : part;
  });
}

function kitRarity(of: KitKind, kit: number) {
  return cached(rarities, `${of}:${kit}`, () => game.rarity(game.kitId(of, kit)));
}

export function visibleKits() {
  const of = kind();
  const words = query().trim().toLowerCase();
  const list = cached(lists, of, () => api.kits(of)).filter(
    (kit) => kitName(of, kit.Id) !== '' && (!words || kitName(of, kit.Id).toLowerCase().includes(words)),
  );
  const { field, descending } = sort();
  const byName = (a: Kit, b: Kit) => compare(kitName(of, a.Id), kitName(of, b.Id));
  const direction = descending ? -1 : 1;
  const primary = (a: Kit, b: Kit) =>
    direction * (field === 'rarity' ? kitRarity(of, a.Id) - kitRarity(of, b.Id) : byName(a, b));
  return [...list].sort((a, b) => primary(a, b) || byName(a, b));
}
