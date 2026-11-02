import { createSignal } from 'solid-js';
import { t } from '../../i18n';
import type { ItemKind } from './api';
import {
  type Card,
  cardOf,
  cardsOf,
  type Drill,
  type Entry,
  entryOfKey,
  everything,
  grouped,
  rarityName,
} from './catalog';
import * as game from './game';
import { favorites } from './store';

type Mode = 'catalog' | 'mine' | 'kits';
export type SortField = 'name' | 'rarity' | 'type' | 'collection';
type Sort = { field: SortField; descending: boolean };
export type Facet<Value> = { value: Value; name: string; count: number };

const [mode, setMode] = createSignal<Mode>('catalog');
const [category, setCategory] = createSignal<ItemKind>('weapon');
const [drill, setDrill] = createSignal<Drill | undefined>();
const [query, setQuery] = createSignal('');
const [modelSort, setModelSort] = createSignal<Sort>({ field: 'name', descending: false });
const [entrySort, setEntrySort] = createSignal<Sort>({ field: 'name', descending: false });
const [type, setType] = createSignal('');
const [rarity, setRarity] = createSignal(-1);
const [set, setSet] = createSignal('');
const [favoritesOnly, setFavoritesOnly] = createSignal(false);
const [focusGrid, setFocusGrid] = createSignal(false);

let previous: Mode = 'catalog';

export { category, drill, favoritesOnly, focusGrid, mode, query, rarity, set, setMode, type };

export function showKits() {
  if (mode() !== 'kits') previous = mode();
  setMode('kits');
}

export function hideKits() {
  if (mode() === 'kits') setMode(previous);
}

function clearFilters() {
  setType('');
  setRarity(-1);
  setSet('');
  setQuery('');
}

export function pickCategory(kind: ItemKind) {
  setFocusGrid(false);
  setCategory(kind);
  setMode('catalog');
  setDrill(undefined);
  clearFilters();
}

export function openDrill(next: Drill | undefined) {
  setFocusGrid(true);
  setDrill(next);
  setRarity(-1);
  setSet('');
  setQuery('');
}

export function search(text: string) {
  setFocusGrid(false);
  setQuery(text);
}

export function filter(field: 'type' | 'set', value: string): void;
export function filter(field: 'rarity', value: number): void;
export function filter(field: 'type' | 'rarity' | 'set', value: string | number) {
  setFocusGrid(false);
  if (field === 'type') setType(value as string);
  if (field === 'rarity') setRarity(value as number);
  if (field === 'set') setSet(value as string);
}

export function toggleFavoritesOnly() {
  setFavoritesOnly(!favoritesOnly());
}

export function resetFilters() {
  setFocusGrid(false);
  clearFilters();
  setFavoritesOnly(false);
}

export const filtered = () => type() !== '' || rarity() >= 0 || set() !== '' || favoritesOnly() || query() !== '';
export const searching = () => query().trim() !== '' && category() !== 'medal';
export const global = () => searching() || (favoritesOnly() && !drill() && category() !== 'medal');
export const models = () => !drill() && !global() && category() === 'weapon';
export const shownDrill = () => (global() ? undefined : drill());
export const sort = () => {
  const current = models() ? modelSort() : entrySort();
  return sortFields().includes(current.field) ? current : { ...current, field: 'name' as const };
};

export function sortBy(field: SortField) {
  (models() ? setModelSort : setEntrySort)({ field, descending: field === 'rarity' });
}

export function flipSort() {
  const current = sort();
  (models() ? setModelSort : setEntrySort)({ ...current, descending: !current.descending });
}

export function sortFields(): SortField[] {
  if (models()) return ['name', 'type'];
  return category() === 'music' && !global() ? ['name'] : ['name', 'rarity', 'collection'];
}

const isFavorite = (card: Card) => card.entry.variants.some((each) => favorites().includes(each.key));
const ofEntries = (list: Entry[]) => list.map((each) => cardOf(each));

export function matchingCards(): Card[] {
  const words = query().trim().toLowerCase();
  if (searching()) return ofEntries(everything().filter((each) => each.name.toLowerCase().includes(words)));
  if (global()) return ofEntries(favorites().flatMap((key) => entryOfKey(key) ?? []));
  const open = drill();
  const list = open ? grouped(open.entries()) : cardsOf(category());
  return words ? list.filter((card) => card.title.toLowerCase().includes(words)) : list;
}

export function facets(list: Card[]) {
  const count = <Value>(value: (card: Card) => Value, name: (value: Value) => string) => {
    const found = new Map<Value, number>();
    for (const card of list) found.set(value(card), (found.get(value(card)) ?? 0) + 1);
    return [...found].map(([each, total]): Facet<Value> => ({ value: each, name: name(each), count: total }));
  };
  const kind = category();
  return {
    types: count((card) => card.entry.type, game.typeName).sort((a, b) => compare(a.name, b.name)),
    rarities: count(
      (card) => card.entry.rarity,
      (value) => rarityName(kind, value),
    ).sort((a, b) => b.value - a.value),
    sets: count((card) => card.entry.set, game.setName)
      .filter((each) => each.value !== '')
      .sort((a, b) => compare(a.name, b.name)),
  };
}

export function compare(a: string, b: string) {
  const left = a.toLowerCase();
  const right = b.toLowerCase();
  return left < right ? -1 : left > right ? 1 : 0;
}

const byName = (a: Card, b: Card) => compare(a.title, b.title);

function sorter(field: SortField) {
  if (field === 'rarity') return (a: Card, b: Card) => a.entry.rarity - b.entry.rarity;
  if (field === 'type') return (a: Card, b: Card) => compare(a.entry.type, b.entry.type);
  if (field === 'collection')
    return (a: Card, b: Card) => compare(game.setName(a.entry.set), game.setName(b.entry.set));
  return byName;
}

export function visibleCards(matching: Card[]) {
  const list = matching.filter(
    (card) =>
      (type() === '' || card.entry.type === type()) &&
      (rarity() < 0 || card.entry.rarity === rarity()) &&
      (set() === '' || card.entry.set === set()) &&
      (!favoritesOnly() || global() || isFavorite(card)),
  );
  const { field, descending } = sort();
  const order = sorter(field);
  const direction = descending ? -1 : 1;
  const sorted = [...list].sort((a, b) => direction * order(a, b) || byName(a, b));
  const vanilla = (card: Card) => shownDrill() !== undefined && card.entry.title === t.vanilla;
  return [...sorted.filter(vanilla), ...sorted.filter((card) => !vanilla(card))];
}
