import { createEffect, createSignal } from 'solid-js';
import { api, type ItemRecord, ids, type Sticker } from './api';
import { hideKits } from './browser';
import type { Entry } from './catalog';
import type { KitKind } from './kits';
import { savedFor, shared, teamsOf } from './store';

export type EditorTab = 'finish' | 'stickers' | 'charm' | 'patches' | 'tag';
export type View = 'model' | 'first' | 'agent';

export const seedMax = 1000;
export const draftId = api.draftId();

const [selected, setSelected] = createSignal<Entry>();
const [draft, setDraft] = createSignal<ItemRecord>();
const [tab, setTab] = createSignal<EditorTab>('finish');
const [slot, setSlot] = createSignal(-1);
const [placing, setPlacing] = createSignal(false);
const [curtain, setCurtain] = createSignal<'' | 'closing' | 'opening'>('');
const [view, setView] = createSignal<View>('model');
const [revision, setRevision] = createSignal(0);
const [problem, setProblem] = createSignal('');

export { curtain, draft, placing, problem, revision, selected, slot, tab, view };

let placingNext = false;

export function randomSeed() {
  return Math.floor(Math.random() * (seedMax + 1));
}

export function souvenirAllowed(entry: Entry) {
  return entry.kind === 'weapon' && entry.paint !== undefined;
}

export function tabsOf(entry: Entry): EditorTab[] {
  const { item } = entry;
  const finish = entry.paint !== undefined || item.StatTrak || item.Legacy || entry.variants.length > 1;
  const tabs: [EditorTab, boolean][] = [
    ['finish', finish],
    ['stickers', item.StickerSlots > 0],
    ['charm', item.CharmSlots > 0],
    ['patches', item.PatchSlots > 0],
    ['tag', item.NameTag],
  ];
  return tabs.filter(([, present]) => present).map(([name]) => name);
}

export function newRecord(entry: Entry, team: number): ItemRecord {
  const { paint, item } = entry;
  return {
    Team: team,
    Slot: item.Slot,
    Definition: item.Definition,
    Paint: paint?.Id ?? 0,
    Seed: paint ? randomSeed() : 0,
    Wear: paint?.WearMin ?? 0,
    Legacy: false,
    StatTrak: false,
    StatTrakCount: 0,
    Name: '',
    Stickers: [],
    Patches: [],
    Charm: undefined,
    SouvenirEvent: 0,
    Music: entry.music,
    Shown: false,
  };
}

export function open(entry: Entry, team?: number) {
  const teams = teamsOf(entry);
  const order = team !== undefined ? [team] : shared(entry) ? [ids.AnyTeam] : [ids.Terrorists, ids.CounterTerrorists];
  const saved = order.map((each) => savedFor(entry, each)).find((each) => each !== undefined);
  setSelected(entry);
  setDraft(saved ? { ...saved.Record } : newRecord(entry, teams[0]));
  const tabs = tabsOf(entry);
  if (!tabs.includes(tab())) setTab(tabs[0] ?? 'finish');
  setSlot(-1);
  setPlacing(false);
  setView(entry.kind === 'agent' ? 'agent' : 'model');
  hideKits();
}

export function edit(change: Partial<ItemRecord>) {
  const current = draft();
  if (current) setDraft({ ...current, ...change });
}

export function chooseVariant(entry: Entry) {
  const current = draft();
  if (!current || !entry.paint) return;
  setSelected(entry);
  edit({ Paint: entry.paint.Id, Wear: Math.min(Math.max(current.Wear, entry.paint.WearMin), entry.paint.WearMax) });
}

export function chooseTab(next: EditorTab) {
  setTab(next);
  setSlot(-1);
  setPlacing(false);
  hideKits();
}

export function chooseSlot(next: number) {
  setSlot(next);
  setPlacing(false);
}

export function chooseView(next: View) {
  setView(next);
  setPlacing(false);
}

export function stickerIn(at: number) {
  return draft()?.Stickers.find((each) => each.Slot === at);
}

export function kitIn(kind: KitKind, at: number) {
  if (kind === 'sticker') return stickerIn(at)?.Kit;
  if (kind === 'patch') return draft()?.Patches.find((each) => each.Slot === at)?.Kit;
  return draft()?.Charm?.Kit;
}

function putSticker(at: number, sticker: Sticker | undefined) {
  const rest = (draft()?.Stickers ?? []).filter((each) => each.Slot !== at);
  edit({ Stickers: sticker ? [...rest, sticker] : rest });
}

export function setSticker(at: number, kit: number) {
  const old = stickerIn(at);
  const base = { Wear: 0, Scale: 1, Rotation: 0, OffsetX: 0, OffsetY: 0, Schema: -1 };
  putSticker(at, { ...(old ?? base), Slot: at, Kit: kit });
}

export function changeSticker(at: number, change: Partial<Sticker>) {
  const old = stickerIn(at);
  if (old) putSticker(at, { ...old, ...change });
}

export function removeSticker(at: number) {
  putSticker(at, undefined);
  setPlacing(false);
}

function switchPlacing(next: boolean) {
  if (curtain() !== '' || placing() === next) return;
  placingNext = next;
  setCurtain('closing');
}

export function curtainDone() {
  if (curtain() === 'closing') {
    setPlacing(placingNext);
    setCurtain('opening');
  } else setCurtain('');
}

export function startPlacing() {
  switchPlacing(true);
}

export function savePlacement() {
  const placed = api.stickerPlacement();
  if (placed) changeSticker(slot(), placed);
  switchPlacing(false);
}

export function cancelPlacement() {
  switchPlacing(false);
}

export function resetPlacement() {
  changeSticker(slot(), { Schema: -1, OffsetX: 0, OffsetY: 0, Rotation: 0 });
}

export function setPatch(at: number, kit: number | undefined) {
  const rest = (draft()?.Patches ?? []).filter((each) => each.Slot !== at);
  edit({ Patches: kit === undefined ? rest : [...rest, { Slot: at, Kit: kit }] });
}

export function setCharm(kit: number | undefined) {
  const old = draft()?.Charm;
  const base = { Pattern: 0, Highlight: 0, OffsetX: 0, OffsetY: 0, OffsetZ: 0 };
  edit({ Charm: kit === undefined ? undefined : { ...(old ?? base), Kit: kit } });
}

function previewed() {
  const current = draft();
  if (!current || !placing()) return current;
  return { ...current, Stickers: current.Stickers.filter((each) => each.Slot !== slot()) };
}

export function endPreview() {
  api.endPreview();
}

export function usePreview(active: () => boolean) {
  createEffect(() => {
    const record = active() ? previewed() : undefined;
    if (!record) {
      endPreview();
      setProblem('');
      return;
    }
    const error = api.preview(record);
    setProblem(error);
    if (!error) setRevision((value) => value + 1);
  });
}
