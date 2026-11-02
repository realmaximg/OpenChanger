import { createMemo, Show } from 'solid-js';
import { ActionButton } from '../../../../components/ActionButton/ActionButton';
import { SortControls } from '../../../../components/SortControls/SortControls';
import { TextField } from '../../../../components/TextField/TextField';
import { ViewHeader } from '../../../../components/ViewHeader/ViewHeader';
import { WindowedGrid } from '../../../../components/WindowedGrid/WindowedGrid';
import { icons } from '../../../../design/icons';
import { Presence } from '../../../../design/motion';
import { t } from '../../../../i18n';
import {
  category,
  favoritesOnly,
  flipSort,
  matchingCards,
  openDrill,
  query,
  type SortField,
  search,
  searching,
  shownDrill,
  sort,
  sortBy,
  sortFields,
  toggleFavoritesOnly,
  visibleCards,
} from '../../browser';
import { categoryName } from '../../catalog';
import { columns } from '../../layout';
import css from './CatalogView.module.css';
import { FilterRow } from './FilterRow';
import { ItemCard } from './ItemCard';

const sortNames: Record<SortField, string> = {
  name: t.sortName,
  rarity: t.sortRarity,
  type: t.sortType,
  collection: t.sortCollection,
};

export function Empty() {
  return (
    <Panel class={css.empty}>
      <Label class={css.emptyTitle} text={t.emptyTitle} />
      <Label class={css.emptyText} text={t.emptyText} />
    </Panel>
  );
}

function FavoritesToggle() {
  return (
    <Button
      class={css.favorites}
      classList={{ [css.on]: favoritesOnly() }}
      enabled={!searching()}
      tabindex="auto"
      selectionpos="auto"
      tooltip={t.favoritesOnly}
      onactivate={toggleFavoritesOnly}
    >
      <Image class={css.star} src={favoritesOnly() ? icons.starFilled : icons.star} />
    </Button>
  );
}

export function CatalogView() {
  const matching = createMemo(matchingCards, undefined, { equals: false });
  const cards = createMemo(() => visibleCards(matching()));
  return (
    <Panel class={css.view}>
      <ViewHeader
        title={shownDrill()?.title ?? categoryName(category())}
        start={
          <Show when={shownDrill() !== undefined}>
            <ActionButton class={css.back} icon={icons.back} tooltip={t.back} onPress={() => openDrill(undefined)} />
          </Show>
        }
      >
        <FavoritesToggle />
      </ViewHeader>
      <Panel class={css.row}>
        <TextField search class={css.search} value={query()} placeholder={t.search} onChange={search} />
        <SortControls
          fields={sortFields()}
          names={sortNames}
          field={sort().field}
          descending={sort().descending}
          onSort={sortBy}
          onFlip={flipSort}
        />
      </Panel>
      <FilterRow cards={matching()} />
      <Show when={cards().length > 0} fallback={<Empty />}>
        <Show when={`${columns()}:${category()}:${shownDrill()?.title ?? ''}`} keyed>
          <Presence motion="appear">
            <WindowedGrid items={cards()} chunk={columns() * 8} chunkHeight={1440} class={css.grid}>
              {(card) => <ItemCard card={card} first={card === cards()[0]} />}
            </WindowedGrid>
          </Presence>
        </Show>
      </Show>
    </Panel>
  );
}
