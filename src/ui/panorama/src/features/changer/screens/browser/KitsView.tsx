import { createMemo, Show } from 'solid-js';
import { ActionButton } from '../../../../components/ActionButton/ActionButton';
import { Picture } from '../../../../components/Picture/Picture';
import { SortControls } from '../../../../components/SortControls/SortControls';
import { TextField } from '../../../../components/TextField/TextField';
import { ViewHeader } from '../../../../components/ViewHeader/ViewHeader';
import { WindowedGrid } from '../../../../components/WindowedGrid/WindowedGrid';
import { t } from '../../../../i18n';
import type { Kit } from '../../api';
import { hideKits } from '../../browser';
import { kitIn, setCharm, setPatch, setSticker, slot } from '../../editor';
import * as game from '../../game';
import {
  flipKits,
  fresh,
  type KitKind,
  kind,
  kitName,
  query,
  searchKits,
  sort,
  sortKits,
  visibleKits,
} from '../../kits';
import { kitColumns } from '../../layout';
import { Empty } from './CatalogView';
import css from './KitsView.module.css';

const titles: Record<KitKind, string> = { sticker: t.tabStickers, patch: t.tabPatches, keychain: t.tabCharm };
const sortNames = { name: t.sortName, rarity: t.sortRarity };

function pick(kit: Kit) {
  const of = kind();
  if (of === 'sticker') setSticker(slot(), kit.Id);
  else if (of === 'patch') setPatch(slot(), kit.Id);
  else setCharm(kit.Id);
}

function KitTile(props: { kit: Kit; first: boolean }) {
  const id = () => game.kitId(kind(), props.kit.Id);
  const name = () => kitName(kind(), props.kit.Id);
  return (
    <Button
      class={css.tile}
      classList={{ [css.current]: kitIn(kind(), slot()) === props.kit.Id }}
      hittestchildren={false}
      focus={props.first && fresh()}
      tabindex="auto"
      selectionpos="auto"
      onactivate={() => pick(props.kit)}
    >
      <Panel class={css.art}>
        <Picture class={css.image} id={id()} />
      </Panel>
      <Panel class={css.rarity} style={{ backgroundColor: game.rarityColor(id()) }} />
      <Label class={css.name} text={name()} />
    </Button>
  );
}

export function KitsView() {
  const kits = createMemo(visibleKits);
  return (
    <Panel class={css.view}>
      <ViewHeader title={titles[kind()]}>
        <ActionButton label={t.done} onPress={hideKits} />
      </ViewHeader>
      <Panel class={css.row}>
        <TextField search class={css.search} value={query()} placeholder={t.searchKits} onChange={searchKits} />
        <SortControls
          fields={['name', 'rarity']}
          names={sortNames}
          field={sort().field}
          descending={sort().descending}
          onSort={sortKits}
          onFlip={flipKits}
        />
      </Panel>
      <Show when={kits().length > 0} fallback={<Empty />}>
        <Show when={kitColumns()} keyed>
          {(count: number) => (
            <WindowedGrid items={kits()} chunk={count * 6} chunkHeight={912} class={css.grid}>
              {(kit) => <KitTile kit={kit} first={kit === kits()[0]} />}
            </WindowedGrid>
          )}
        </Show>
      </Show>
    </Panel>
  );
}
