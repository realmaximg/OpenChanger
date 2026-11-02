import { For, Match, Switch } from 'solid-js';
import { t } from '../../../../i18n';
import type { Entry } from '../../catalog';
import { chooseTab, type EditorTab, tab, tabsOf } from '../../editor';
import { wide } from '../../layout';
import { CharmTab } from './CharmTab';
import css from './Editor.module.css';
import { FinishTab } from './FinishTab';
import { InstallBar } from './InstallBar';
import { KitSlotsTab } from './KitSlotsTab';
import { NameTab } from './NameTab';

function titleOf(name: EditorTab, entry: Entry) {
  if (name === 'finish') return entry.kind === 'music' ? t.tabQuality : t.tabFinish;
  return { stickers: t.tabStickers, charm: t.tabCharm, patches: t.tabPatches, tag: t.tabTag }[name];
}

export function Editor(props: { entry: Entry }) {
  const tabs = () => tabsOf(props.entry);
  return (
    <Panel class={css.editor} classList={{ [css.wide]: wide() }}>
      <Panel class={css.tabs} visible={tabs().length > 1}>
        <For each={tabs()}>
          {(name, index) => (
            <Button
              class={css.tab}
              classList={{ [css.active]: tab() === name }}
              focus={index() === 0}
              tabindex="auto"
              selectionpos="auto"
              onactivate={() => chooseTab(name)}
            >
              <Label class={css.tabTitle} text={titleOf(name, props.entry)} />
            </Button>
          )}
        </For>
      </Panel>
      <Panel class={css.body}>
        <For each={tabs()}>
          {(name) => (
            <Panel class={css.page} classList={{ [css.shown]: tab() === name }} hittestchildren={tab() === name}>
              <Body entry={props.entry} name={name} />
            </Panel>
          )}
        </For>
      </Panel>
      <InstallBar entry={props.entry} />
    </Panel>
  );
}

function Body(props: { entry: Entry; name: EditorTab }) {
  return (
    <Switch>
      <Match when={props.name === 'finish'}>
        <FinishTab entry={props.entry} />
      </Match>
      <Match when={props.name === 'stickers'}>
        <KitSlotsTab kind="sticker" slots={props.entry.item.StickerSlots} />
      </Match>
      <Match when={props.name === 'patches'}>
        <KitSlotsTab kind="patch" slots={props.entry.item.PatchSlots} />
      </Match>
      <Match when={props.name === 'charm'}>
        <CharmTab />
      </Match>
      <Match when={props.name === 'tag'}>
        <NameTab />
      </Match>
    </Switch>
  );
}
