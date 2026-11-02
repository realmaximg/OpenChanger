import { format, t } from '../../i18n';
import { api, type Item, type ItemKind, type ItemRecord, type Paint } from './api';
import * as game from './game';

export type Entry = {
  key: string;
  kind: ItemKind;
  item: Item;
  paint: Paint | undefined;
  music: number;
  id: string;
  image: string;
  name: string;
  title: string;
  rarity: number;
  color: string;
  type: string;
  set: string;
  variants: Entry[];
};

export type Drill = { title: string; count: number; entries: () => Entry[] };

export type Card = { key: string; entry: Entry; title: string; subtitle?: string; drill?: Drill };

export const modelKinds: ItemKind[] = ['weapon', 'knife', 'gloves'];
const browsable: ItemKind[] = [...modelKinds, 'agent', 'music'];
export const kinds: ItemKind[] = [...browsable, 'medal'];

const rarityNames = ['Default', 'Common', 'Uncommon', 'Rare', 'Mythical', 'Legendary', 'Ancient'];
const categoryTokens: Record<ItemKind, [string, string]> = {
  weapon: ['#Inv_Category_weapon', t.weapons],
  knife: ['#SFUI_InvPanel_filter_melee', t.knives],
  gloves: ['#SFUI_InvPanel_filter_gloves', t.gloves],
  agent: ['#LoadoutSlot_customplayer', t.agents],
  music: ['#SFUI_InvPanel_filter_musickit', t.musicKits],
  medal: ['#Inv_Category_flair0', t.medals],
};
const phaseNames = [t.phase1, t.phase2, t.phase3, t.phase4];
const variantNames: Record<string, string> = {
  ruby: t.ruby,
  sapphire: t.sapphire,
  blackpearl: t.blackPearl,
  emerald: t.emerald,
};

const items = new Map<ItemKind, Item[]>();
const cards = new Map<ItemKind, Card[]>();
const drilled = new Map<string, Entry[]>();
let souvenirs: number[] | undefined;
const musicDefinition = () => itemsOf('music')[0];

export function souvenirEvents() {
  souvenirs ??= api.souvenirEvents();
  return souvenirs;
}

export function categoryName(kind: ItemKind) {
  return game.localized(...categoryTokens[kind]);
}

export function rarityName(kind: ItemKind, value: number) {
  if (value >= rarityNames.length) return game.localized('#Rarity_Contraband', String(value));
  const base = `#Rarity_${rarityNames[value]}`;
  const suffix = kind === 'agent' ? '_Character' : kind === 'weapon' || kind === 'knife' ? '_Weapon' : '';
  return game.localized(base + suffix, game.localized(base, String(value)));
}

export function variantLabel(paint: Paint) {
  const phase = /phase(\d)/.exec(paint.Name);
  if (phase) return phaseNames[Number(phase[1]) - 1] ?? paint.Name;
  const gem = /_(ruby|sapphire|blackpearl|emerald)_marbleized/.exec(paint.Name);
  return gem ? variantNames[gem[1]] : paint.Name;
}

export function cached<Key, Value>(cache: Map<Key, Value>, key: Key, make: () => Value) {
  let value = cache.get(key);
  if (value === undefined) {
    value = make();
    cache.set(key, value);
  }
  return value;
}

function groupBy(list: Entry[], key: (entry: Entry) => string) {
  const groups = new Map<string, Entry[]>();
  for (const each of list) groups.set(key(each), [...(groups.get(key(each)) ?? []), each]);
  return groups;
}

function itemsOf(kind: ItemKind) {
  return cached(items, kind, () => api.items(kind));
}

function itemOf(definition: number) {
  for (const kind of kinds) {
    const item = itemsOf(kind).find((each) => each.Definition === definition);
    if (item) return { kind, item };
  }
  return undefined;
}

function paintTitle(name: string) {
  const parts = name.split(' | ');
  return parts.length > 1 ? parts[1] : name;
}

export function cardOf(entry: Entry, drilled = false): Card {
  const [head, tail] = entry.name.split(' | ');
  if (entry.kind === 'agent') return { key: entry.key, entry, title: head, subtitle: tail };
  if (drilled || tail === undefined) return { key: entry.key, entry, title: drilled ? entry.title : entry.name };
  return { key: entry.key, entry, title: tail, subtitle: head };
}

function entry(kind: ItemKind, item: Item, paint: Paint | undefined): Entry {
  const id = game.fauxId(item.Definition, paint?.Id ?? 0);
  const name = game.itemName(id);
  const made: Entry = {
    key: `${item.Definition}:${paint?.Id ?? 0}`,
    kind,
    item,
    paint,
    music: 0,
    id,
    image: '',
    name,
    title: paint ? paintTitle(name) : kind === 'weapon' || kind === 'knife' ? t.vanilla : name,
    rarity: game.rarity(id),
    color: game.rarityColor(id),
    type: game.typeOf(id),
    set: game.setOf(id),
    variants: [],
  };
  made.variants = [made];
  return made;
}

function musicEntry(kit: number): Entry | undefined {
  const token = game.musicToken(kit);
  const name = game.localized(token, '');
  if (!name) return undefined;
  const made: Entry = {
    key: `music:${kit}`,
    kind: 'music',
    item: musicDefinition(),
    paint: undefined,
    music: kit,
    id: '',
    image: game.musicImage(kit),
    name,
    title: name,
    rarity: 3,
    color: game.rarityColor(game.fauxId(musicDefinition().Definition, 0)),
    type: '',
    set: '',
    variants: [],
  };
  made.variants = [made];
  return made;
}

function withVariants(list: Entry[]) {
  for (const group of groupBy(list, (each) => each.name).values()) {
    if (group.length < 2 || !group[0].paint) continue;
    for (const each of group) each.variants = group;
  }
  return list;
}

function paintsOf(kind: ItemKind, item: Item) {
  return cached(drilled, `${kind}:${item.Definition}`, () => {
    const vanilla = kind === 'gloves' ? [] : [entry(kind, item, undefined)];
    return withVariants([...vanilla, ...api.paints(item.Definition).map((paint) => entry(kind, item, paint))]);
  });
}

function modelCards(kind: ItemKind) {
  return itemsOf(kind).flatMap((item): Card[] => {
    const count = api.paints(item.Definition).length + (kind === 'gloves' ? 0 : 1);
    if (kind !== 'weapon' && count < 2) return [];
    const model = entry(kind, item, undefined);
    model.title = model.name;
    if (!game.hasImage(model.id)) {
      const finishes = paintsOf(kind, item).filter((each) => each.paint);
      model.id = finishes.sort((a, b) => (a.title.toLowerCase() < b.title.toLowerCase() ? -1 : 1))[0]?.id ?? model.id;
    }
    return [
      {
        key: model.key,
        entry: model,
        title: model.name,
        drill: { title: model.name, count, entries: () => paintsOf(kind, item) },
      },
    ];
  });
}

function agentCards() {
  const agents = itemsOf('agent').map((item) => entry('agent', item, undefined));
  return [...groupBy(agents, (agent) => paintTitle(agent.name))].map(([title, members]): Card => {
    if (members.length === 1) return cardOf(members[0]);
    const drill = { title, count: members.length, entries: () => members };
    return { key: `group:${title}`, entry: members[0], title, drill };
  });
}

function build(kind: ItemKind): Card[] {
  if (modelKinds.includes(kind)) return modelCards(kind);
  if (kind === 'agent') return agentCards();
  if (kind === 'music') {
    const list = api.kits('music').flatMap((kit) => musicEntry(kit.Id) ?? []);
    return list.map((each) => ({ key: each.key, entry: each, title: each.title }));
  }
  return medalCards(
    itemsOf(kind)
      .map((item) => entry(kind, item, undefined))
      .filter((medal) => medal.name !== ''),
  );
}

function medalCards(medals: Entry[]): Card[] {
  const byName = groupBy(medals, (medal) => medal.name);
  return medals.map((medal): Card => {
    const same = byName.get(medal.name) ?? [];
    if (same.length < 2) return { key: medal.key, entry: medal, title: medal.name };
    const level = [...same].sort((a, b) => a.item.Definition - b.item.Definition).indexOf(medal) + 1;
    return { key: medal.key, entry: medal, title: medal.name, subtitle: format(t.level, { n: level }) };
  });
}

export function cardsOf(kind: ItemKind) {
  return cached(cards, kind, () => build(kind));
}

export function grouped(list: Entry[]): Card[] {
  const seen = new Set<Entry[]>();
  const result: Card[] = [];
  for (const each of list) {
    if (each.variants.length > 1) {
      if (seen.has(each.variants)) continue;
      seen.add(each.variants);
    }
    result.push(cardOf(each, true));
  }
  return result;
}

function entriesOf(kind: ItemKind) {
  return cardsOf(kind).flatMap((card) => card.drill?.entries() ?? [card.entry]);
}

function musicOf(kit: number) {
  return entriesOf('music').find((each) => each.music === kit);
}

export function everything() {
  return browsable.flatMap((kind) => entriesOf(kind));
}

export function entryOf(record: ItemRecord): Entry | undefined {
  const found = itemOf(record.Definition);
  if (!found) return undefined;
  const { kind, item } = found;
  if (kind === 'music') return musicOf(record.Music);
  if (kind === 'agent' || kind === 'medal') return entriesOf(kind).find((each) => each.item === item);
  return paintsOf(kind, item).find((each) => (each.paint?.Id ?? 0) === record.Paint);
}

export function entryOfKey(key: string) {
  const [first, second] = key.split(':');
  if (first === 'music') return musicOf(Number(second));
  return entryOf({ Definition: Number(first), Paint: Number(second), Music: 0 } as ItemRecord);
}
