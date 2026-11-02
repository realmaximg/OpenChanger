import { createEffect, Match, onCleanup, Switch } from 'solid-js';
import { on } from '../../../bridge/events';
import { Presence } from '../../../design/motion';
import { t } from '../../../i18n';
import { tabShown } from '../../../shell/tab';
import { mode } from '../browser';
import { endPreview, usePreview } from '../editor';
import { measureLayout, wide } from '../layout';
import { readLoadout } from '../store';
import { CatalogView } from './browser/CatalogView';
import { KitsView } from './browser/KitsView';
import { MineView } from './browser/MineView';
import css from './Changer.module.css';
import { Header } from './Header';
import { Work } from './workspace/Work';

export function Changer() {
  usePreview(tabShown);
  onCleanup(on('changer.loadoutChanged', readLoadout));
  onCleanup(() => {
    InventoryAPI.StopItemPreviewMusic();
    endPreview();
  });
  createEffect(() => {
    if (!tabShown()) return;
    measureLayout();
    readLoadout();
  });
  return (
    <Panel class={css.page}>
      <Panel class={css.main}>
        <Header />
        <Panel class={css.body}>
          <Panel class={css.browser} classList={{ [css.wide]: wide() }}>
            <Switch>
              <Match when={mode() === 'catalog'}>
                <Presence motion="appear">
                  <CatalogView />
                </Presence>
              </Match>
              <Match when={mode() === 'mine'}>
                <Presence motion="appear">
                  <MineView />
                </Presence>
              </Match>
              <Match when={mode() === 'kits'}>
                <Presence motion="appear">
                  <KitsView />
                </Presence>
              </Match>
            </Switch>
          </Panel>
          <Work />
        </Panel>
      </Panel>
      <Label class={css.credit} text={t.credit} hittest={false} />
    </Panel>
  );
}
