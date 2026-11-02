import { createSignal } from 'solid-js';
import { api, type ItemRecord, ids, type LoadoutItem } from './api';
import type { Entry } from './catalog';

const [loadout, setLoadout] = createSignal<LoadoutItem[]>(api.loadout());
const [favorites, setFavorites] = createSignal<string[]>(api.favorites());

export { favorites, loadout };

export function readLoadout() {
  setLoadout(api.loadout());
}

export function toggleFavorite(key: string) {
  api.favorite(key, !favorites().includes(key));
  setFavorites(api.favorites());
}

export function shared(entry: Entry) {
  return entry.kind === 'music' || entry.kind === 'medal';
}

export function teamsOf(entry: Entry) {
  if (shared(entry)) return [ids.AnyTeam];
  return [ids.Terrorists, ids.CounterTerrorists].filter((team) => (entry.item.Teams & (1 << team)) !== 0);
}

export function isMine(entry: Entry, record: ItemRecord) {
  if (record.Definition !== entry.item.Definition) return false;
  if (entry.kind === 'music') return record.Music === entry.music;
  return entry.kind === 'medal' || record.Paint === (entry.paint?.Id ?? 0);
}

export function savedFor(entry: Entry, team: number) {
  return loadout().find(
    ({ Record }) => Record.Team === team && Record.Slot === entry.item.Slot && isMine(entry, Record),
  );
}

function canonical(value: unknown): unknown {
  if (Array.isArray(value)) return value.map(canonical);
  if (typeof value === 'number') return Math.round(value * 1e5) / 1e5;
  if (typeof value !== 'object' || value === null) return value;
  const fields = value as Record<string, unknown>;
  return Object.fromEntries(
    Object.keys(fields)
      .sort()
      .map((key) => [key, canonical(fields[key])]),
  );
}

function normal(record: ItemRecord) {
  const bySlot = (a: { Slot: number }, b: { Slot: number }) => a.Slot - b.Slot;
  const sorted = {
    ...record,
    Team: 0,
    Stickers: [...record.Stickers].sort(bySlot),
    Patches: [...record.Patches].sort(bySlot),
  };
  return JSON.stringify(canonical(sorted));
}

export function sameRecord(a: ItemRecord, b: ItemRecord) {
  return normal(a) === normal(b);
}
