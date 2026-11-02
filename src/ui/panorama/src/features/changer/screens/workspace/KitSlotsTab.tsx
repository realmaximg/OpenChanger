import { For, Show } from 'solid-js';
import { ActionButton } from '../../../../components/ActionButton/ActionButton';
import { icons } from '../../../../design/icons';
import { t } from '../../../../i18n';
import {
  changeSticker,
  chooseSlot,
  kitIn,
  removeSticker,
  resetPlacement,
  setPatch,
  slot,
  startPlacing,
  stickerIn,
} from '../../editor';
import { type KitKind, openKits, pickingKits } from '../../kits';
import { wide } from '../../layout';
import { KitSlot } from './KitSlot';
import css from './KitSlotsTab.module.css';
import { WearGauge } from './WearGauge';

type Props = { kind: 'sticker' | 'patch'; slots: number };

export function KitHead(props: { kind: KitKind; picking: boolean; onRemove: () => void }) {
  return (
    <Panel class={css.head}>
      <Show when={!props.picking}>
        <ActionButton small class={css.replace} label={t.replace} onPress={() => openKits(props.kind)} />
      </Show>
      <ActionButton small variant="danger" icon={icons.close} tooltip={t.remove} onPress={props.onRemove} />
    </Panel>
  );
}

function StickerDetail() {
  const sticker = () => stickerIn(slot());
  return (
    <>
      <WearGauge
        value={sticker()?.Wear ?? 0}
        min={0}
        max={1}
        decimals={2}
        onCommit={(value) => changeSticker(slot(), { Wear: value })}
      />
      <Panel class={css.buttons}>
        <ActionButton small label={t.editPosition} onPress={startPlacing} />
        <Show when={(sticker()?.Schema ?? -1) >= 0}>
          <ActionButton small variant="ghost" class={css.extra} label={t.resetPosition} onPress={resetPlacement} />
        </Show>
      </Panel>
    </>
  );
}

export function KitSlotsTab(props: Props) {
  const picking = () => pickingKits(props.kind);
  const kitAt = (at: number) => kitIn(props.kind, at);
  const press = (at: number) => {
    chooseSlot(at);
    if (!picking() && kitAt(at) === undefined) openKits(props.kind);
  };
  const remove = () => (props.kind === 'sticker' ? removeSticker(slot()) : setPatch(slot(), undefined));
  return (
    <Panel class={css.layout} classList={{ [css.stacked]: wide(), [css.sticker]: props.kind === 'sticker' }}>
      <Panel class={css.slots}>
        <For each={Array.from({ length: props.slots }, (_, index) => index)}>
          {(at) => (
            <KitSlot
              kind={props.kind}
              kit={kitAt(at)}
              active={slot() === at}
              picking={picking() && slot() === at}
              onPress={() => press(at)}
            />
          )}
        </For>
      </Panel>
      <Panel class={css.detail}>
        <Show when={kitAt(slot())}>
          {() => (
            <>
              <KitHead kind={props.kind} picking={picking()} onRemove={remove} />
              <Show when={props.kind === 'sticker' && !picking()}>
                <StickerDetail />
              </Show>
            </>
          )}
        </Show>
      </Panel>
    </Panel>
  );
}
