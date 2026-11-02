import { createSignal, For, Show } from 'solid-js';
import { ActionButton } from '../../../../components/ActionButton/ActionButton';
import { Picture } from '../../../../components/Picture/Picture';
import { ViewHeader } from '../../../../components/ViewHeader/ViewHeader';
import { icons } from '../../../../design/icons';
import { tokens } from '../../../../design/tokens';
import { t } from '../../../../i18n';
import { ids, type LoadoutItem } from '../../api';
import { pickCategory } from '../../browser';
import { entryOf } from '../../catalog';
import { draft, open, selected } from '../../editor';
import * as game from '../../game';
import { copySide, recordName, remove, resetAll, teamName } from '../../install';
import { isMine, loadout } from '../../store';
import css from './MineView.module.css';

const groups = [
  { team: ids.Terrorists, color: tokens.color.terrorist, from: ids.CounterTerrorists, copy: t.copyFromCT },
  { team: ids.CounterTerrorists, color: tokens.color.counterTerrorist, from: ids.Terrorists, copy: t.copyFromT },
  { team: ids.AnyTeam, color: tokens.color.textDim, from: -1, copy: '' },
];

function Row(props: { item: LoadoutItem }) {
  const entry = () => entryOf(props.item.Record);
  const current = () => {
    const shown = selected();
    return shown !== undefined && isMine(shown, props.item.Record) && draft()?.Team === props.item.Record.Team;
  };
  return (
    <Panel class={css.row} classList={{ [css.current]: current() }}>
      <Button
        class={css.main}
        hittestchildren={false}
        tabindex="auto"
        selectionpos="auto"
        tooltip={entry() === undefined ? t.notInCatalog : undefined}
        onactivate={() => {
          const found = entry();
          if (found) open(found, props.item.Record.Team);
        }}
      >
        <Panel class={css.rarity} style={{ backgroundColor: game.rarityColor(props.item.Id) }} />
        <Panel class={css.thumb}>
          <Picture id={props.item.Id} />
        </Panel>
        <Panel class={css.text}>
          <Label class={css.name} text={recordName(props.item)} />
          <Show when={!props.item.InSlot}>
            <Label class={css.note} text={t.notInSlot} />
          </Show>
        </Panel>
        <Show when={entry() === undefined}>
          <Panel class={css.missing} />
        </Show>
      </Button>
      <Button
        class={css.remove}
        tabindex="auto"
        selectionpos="auto"
        tooltip={t.remove}
        onactivate={() => remove(props.item)}
      >
        <Image class={css.cross} src={icons.close} />
      </Button>
    </Panel>
  );
}

function Nothing() {
  return (
    <Panel class={css.nothing}>
      <ActionButton class={css.pick} label={t.pickItem} onPress={() => pickCategory('weapon')} />
    </Panel>
  );
}

function Groups() {
  const of = (team: number) => loadout().filter((item) => item.Record.Team === team);
  return (
    <Panel class={css.viewport}>
      <Panel class={css.list}>
        <For each={groups}>
          {(group) => (
            <>
              <Panel class={css.groupHead}>
                <Label class={css.groupTitle} text={teamName(group.team)} style={{ color: group.color }} />
                <Panel class={css.spacer} />
                <Show when={group.from >= 0}>
                  <ActionButton
                    small
                    label={group.copy}
                    enabled={of(group.from).length > 0}
                    onPress={() => copySide(group.from, group.team)}
                  />
                </Show>
              </Panel>
              <For each={of(group.team)}>{(item) => <Row item={item} />}</For>
              <Panel class={css.gap} />
            </>
          )}
        </For>
      </Panel>
      <Panel class={css.rail} hittest={false} />
    </Panel>
  );
}

export function MineView() {
  const [confirming, setConfirming] = createSignal(false);
  return (
    <Panel class={css.view}>
      <ViewHeader title={t.mine}>
        <Show
          when={confirming()}
          fallback={
            <Show when={loadout().length > 0}>
              <ActionButton
                small
                variant="danger"
                class={css.answer}
                label={t.resetAll}
                onPress={() => setConfirming(true)}
              />
            </Show>
          }
        >
          <Label class={css.confirm} text={t.resetConfirm} />
          <ActionButton
            small
            variant="danger"
            class={css.answer}
            label={t.yes}
            focus
            onPress={() => {
              setConfirming(false);
              resetAll();
            }}
          />
          <ActionButton small variant="ghost" class={css.answer} label={t.no} onPress={() => setConfirming(false)} />
        </Show>
      </ViewHeader>
      <Show when={loadout().length > 0} fallback={<Nothing />}>
        <Groups />
      </Show>
    </Panel>
  );
}
