import { createMemo, Show } from 'solid-js';
import { ActionButton } from '../../../../components/ActionButton/ActionButton';
import { DropButton } from '../../../../components/DropButton/DropButton';
import type { MenuItem } from '../../../../components/Menu/menu';
import { icons } from '../../../../design/icons';
import { t } from '../../../../i18n';
import {
  category,
  drill,
  type Facet,
  facets,
  filter,
  filtered,
  global,
  models,
  rarity,
  resetFilters,
  set,
  type,
} from '../../browser';
import { type Card, modelKinds } from '../../catalog';
import * as game from '../../game';
import css from './FilterRow.module.css';

function facetItems<Value>(
  list: Facet<Value>[],
  empty: Value,
  all: string,
  current: Value,
  choose: (v: Value) => void,
) {
  const first: MenuItem = { label: all, current: current === empty, on: () => choose(empty) };
  return [
    first,
    ...list.map((each) => ({
      label: `${each.name}  (${each.count})`,
      current: each.value === current,
      on: () => choose(each.value),
    })),
  ];
}

export function FilterRow(props: { cards: Card[] }) {
  const found = createMemo(() => facets(props.cards));
  const local = () => !global() && (drill() !== undefined || !modelKinds.includes(category()));
  const name = <Value,>(list: Facet<Value>[], value: Value) => list.find((each) => each.value === value)?.name ?? '';
  const types = () => models() && found().types.length > 0;
  const rarities = () => local() && (category() === 'agent' || drill() !== undefined) && found().rarities.length > 0;
  const sets = () => local() && category() !== 'music' && found().sets.length > 0;
  return (
    <Show when={types() || rarities() || sets()}>
      <Panel class={css.row}>
        <Show when={types()}>
          <DropButton
            fill
            class={css.facet}
            caption={t.type}
            value={type() ? game.typeName(type()) : t.all}
            active={type() !== ''}
            items={() => facetItems(found().types, '', t.all, type(), (value) => filter('type', value))}
          />
        </Show>
        <Show when={rarities()}>
          <DropButton
            fill
            class={css.facet}
            caption={t.rarity}
            value={rarity() >= 0 ? name(found().rarities, rarity()) : t.any}
            active={rarity() >= 0}
            items={() => facetItems(found().rarities, -1, t.any, rarity(), (value) => filter('rarity', value))}
          />
        </Show>
        <Show when={sets()}>
          <DropButton
            fill
            class={css.facet}
            caption={t.collection}
            value={set() ? game.setName(set()) : t.all}
            active={set() !== ''}
            items={() => facetItems(found().sets, '', t.all, set(), (value) => filter('set', value))}
          />
        </Show>
        <ActionButton
          class={css.clear}
          icon={icons.close}
          tooltip={t.clearFilters}
          enabled={filtered()}
          onPress={resetFilters}
        />
      </Panel>
    </Show>
  );
}
