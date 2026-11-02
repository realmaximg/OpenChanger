import { For, Show } from 'solid-js';
import { DropButton } from '../../../../components/DropButton/DropButton';
import { anchorId, showMenu } from '../../../../components/Menu/menu';
import { iconPath } from '../../../../design/icons';
import { t } from '../../../../i18n';
import type { Entry } from '../../catalog';
import { chooseView, type View, view } from '../../editor';
import * as game from '../../game';
import { anthem, backgroundItems, cheer, chooseWeapon, side, voice, weapon, weapons, zoom, zoomed } from '../../scene';
import css from './Toolbar.module.css';

type Tool = { icon: string; token: string; fallback: string; active?: () => boolean; press: (id: string) => void };

function ToolButton(props: { tool: Tool }) {
  const id = anchorId();
  return (
    <Button
      id={id}
      class={css.tool}
      classList={{ [css.active]: props.tool.active?.() === true }}
      tabindex="auto"
      selectionpos="auto"
      tooltip={game.localized(props.tool.token, props.tool.fallback)}
      onactivate={() => props.tool.press(id)}
    >
      <Image class={css.icon} src={iconPath(props.tool.icon)} />
    </Button>
  );
}

const viewTool = (shown: View, icon: string, token: string): Tool => ({
  icon,
  token,
  fallback: shown,
  active: () => view() === shown,
  press: () => chooseView(shown),
});

const model = viewTool('model', 'equipment/p250', '#tooltip_inspect_weapon_model');
const first = viewTool('first', 'ui/first_person', '#tooltip_lookat_weapon_model');
const agent = viewTool('agent', 'ui/ct_silhouette', '#tooltip_inspect_weapon_model_faction');
const scenery: Tool = {
  icon: 'ui/picture',
  token: '#tooltip_inspect_scenery',
  fallback: 'Background',
  press: (id) => showMenu(id, backgroundItems()),
};

function toolsOf(entry: Entry): Tool[] {
  if (entry.kind === 'weapon' || entry.kind === 'knife') return [model, first, agent, scenery];
  if (entry.kind === 'gloves') return [model, first, scenery];
  if (entry.kind === 'music') {
    return [{ icon: 'ui/unmuted', token: '#tooltip_inspect_play_mvp_anthem', fallback: 'MVP', press: anthem }];
  }
  if (entry.kind !== 'agent') return [];
  return [
    { icon: 'ui/cheer', token: '#tooltip_character_cheer', fallback: 'Cheer', press: () => cheer(entry, false) },
    { icon: 'ui/defeat_anim', token: '#tooltip_character_defeat', fallback: 'Defeat', press: () => cheer(entry, true) },
    { icon: 'ui/sound_off', token: '#tooltip_character_voice', fallback: 'Voice', press: voice },
    {
      icon: zoomed() ? 'ui/zoom_out' : 'ui/zoom_in',
      token: '#tooltip_character_zoom',
      fallback: 'Zoom',
      active: zoomed,
      press: zoom,
    },
    scenery,
  ];
}

export function Toolbar(props: { entry: Entry }) {
  const list = () => weapons(side(props.entry));
  return (
    <Panel class={css.toolbar}>
      <For each={toolsOf(props.entry)}>{(tool) => <ToolButton tool={tool} />}</For>
      <Show when={props.entry.kind === 'agent'}>
        <DropButton
          fill
          class={css.weapon}
          caption={t.inHands}
          value={list()[weapon()]?.name ?? ''}
          items={() =>
            list().map((each, index) => ({
              label: each.name,
              current: index === weapon(),
              on: () => chooseWeapon(index),
            }))
          }
        />
      </Show>
    </Panel>
  );
}
