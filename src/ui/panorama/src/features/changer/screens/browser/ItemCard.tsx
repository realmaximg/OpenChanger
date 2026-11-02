import { createSignal, For, Show } from 'solid-js';
import { Picture } from '../../../../components/Picture/Picture';
import { iconPath } from '../../../../design/icons';
import { tokens } from '../../../../design/tokens';
import { type ItemRecord, ids } from '../../api';
import { focusGrid, openDrill } from '../../browser';
import type { Card, Entry } from '../../catalog';
import { open, selected } from '../../editor';
import { isMine, loadout } from '../../store';
import css from './ItemCard.module.css';

const teamOrder = [ids.Terrorists, ids.CounterTerrorists, ids.AnyTeam];

function members(card: Card) {
  if (!card.drill) return card.entry.variants;
  return card.entry.kind === 'agent' ? card.drill.entries() : undefined;
}

function holds(card: Card, record: ItemRecord) {
  return members(card)?.some((each) => isMine(each, record)) ?? record.Definition === card.entry.item.Definition;
}

function contains(card: Card, entry: Entry) {
  return members(card)?.some((each) => each.key === entry.key) ?? entry.item === card.entry.item;
}

function TeamMark(props: { team: number }) {
  const teams = () => (props.team === ids.AnyTeam ? [ids.Terrorists, ids.CounterTerrorists] : [props.team]);
  return (
    <Panel class={css.mark}>
      <For each={teams()}>
        {(team) => (
          <Image
            class={css.logo}
            src={iconPath(team === ids.Terrorists ? 'ui/t_logo_1c' : 'ui/ct_logo_1c')}
            style={{ washColor: team === ids.Terrorists ? tokens.color.terrorist : tokens.color.counterTerrorist }}
          />
        )}
      </For>
    </Panel>
  );
}

export function ItemCard(props: { card: Card; first: boolean }) {
  const [distance, setDistance] = createSignal(0);
  let viewport: Panel | undefined;
  let title: Label | undefined;
  const chosen = () => {
    const current = selected();
    return current !== undefined && contains(props.card, current);
  };
  const teams = () => {
    const held = loadout().filter(({ Record }) => holds(props.card, Record));
    return teamOrder.filter((team) => held.some(({ Record }) => Record.Team === team));
  };
  const hover = () => {
    if (!viewport || !title) return;
    const scale = viewport.actualuiscale_x || 1;
    setDistance(Math.max(0, Math.ceil((title.contentwidth - viewport.actuallayoutwidth) / scale) + 20));
  };
  const leave = () => setDistance(0);
  const press = () => {
    const current = selected();
    if (props.card.drill) openDrill(props.card.drill);
    else open(current && chosen() ? current : props.card.entry);
  };
  return (
    <Panel class={css.slot}>
      <Button
        class={css.card}
        classList={{ [css.selected]: chosen() }}
        hittestchildren={false}
        focus={props.first && focusGrid()}
        tabindex="auto"
        selectionpos="auto"
        onactivate={press}
        onmouseover={hover}
        onmouseout={leave}
      >
        <Panel class={css.art}>
          <Picture class={css.picture} id={props.card.entry.id} src={props.card.entry.image || undefined} />
          <Panel class={css.marks}>
            <For each={teams()}>{(team) => <TeamMark team={team} />}</For>
          </Panel>
        </Panel>
        <Panel
          class={css.rarity}
          style={{ backgroundColor: props.card.drill ? tokens.color.pressed : props.card.entry.color }}
        />
        <Panel class={css.titleBox} ref={(panel) => (viewport = panel)}>
          <Label
            class={css.title}
            classList={{ [css.rolling]: distance() > 0 }}
            ref={(panel) => (title = panel)}
            text={props.card.title}
            style={{
              transform: `translateX(-${distance()}px)`,
              transitionDuration: `${distance() > 0 ? Math.max(tokens.duration.rollMinimum, distance() / tokens.speed.roll) : 0}s`,
            }}
          />
          <Show when={props.card.subtitle}>
            <Label class={css.subtitle} text={props.card.subtitle ?? ''} />
          </Show>
        </Panel>
      </Button>
    </Panel>
  );
}
