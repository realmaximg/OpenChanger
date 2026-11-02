import { type Accessor, createEffect, createMemo, onCleanup, Show, untrack } from 'solid-js';
import { ActionButton } from '../../../../components/ActionButton/ActionButton';
import { Picture } from '../../../../components/Picture/Picture';
import { icons } from '../../../../design/icons';
import { Presence } from '../../../../design/motion';
import { t } from '../../../../i18n';
import { Layer } from '../../../../renderer/panorama';
import { tabShown } from '../../../../shell/tab';
import type { Entry } from '../../catalog';
import {
  cancelPlacement,
  curtain,
  curtainDone,
  draftId,
  placing,
  revision,
  savePlacement,
  selected,
  slot,
  stickerIn,
  view,
} from '../../editor';
import * as game from '../../game';
import { wide } from '../../layout';
import { background, nextPosition, players, scrape, side, usePanel, weapon, zoomed } from '../../scene';
import { favorites, shared, toggleFavorite } from '../../store';
import css from './Stage.module.css';
import { Toolbar } from './Toolbar';

function cameraOf(entry: Entry) {
  if (entry.kind === 'knife') return 'cam_melee';
  if (entry.kind === 'gloves') return 'cam_gloves';
  return game.weaponCamera(entry.id, untrack(wide) ? 2 : 1);
}

function lookatWeapon(entry: Entry) {
  if (entry.kind !== 'gloves') return draftId;
  const list = game.loadoutWeapons(side(entry));
  return (list.find(([name]) => name === 'rifle1') ?? list[0])?.[1] ?? '';
}

function ItemScene(props: { entry: Entry; first: boolean; placing: boolean }) {
  const { entry } = props;
  const active = entry.kind === 'knife' ? 8 : entry.kind === 'gloves' ? 7 : 0;
  let panel: MapPreviewPanel | undefined;
  let armed = false;
  let bound = false;
  const arm = () => {
    const sticker = stickerIn(slot());
    if (!panel || !sticker) return;
    panel.SetAcceptsFocus(true);
    panel.SetFocus();
    InventoryAPI.PreviewStickerInModelPanel(game.kitId('sticker', sticker.Kit), slot(), panel);
    scrape(sticker.Wear);
    armed = true;
  };
  createEffect(() => {
    if (!panel || revision() === 0) return;
    untrack(() => {
      if (!panel) return;
      if (props.first) {
        panel.SetActiveItem(entry.kind === 'knife' ? 8 : 0);
        panel.SetItemItemId(lookatWeapon(entry), '');
        const settings = players(entry);
        if (settings) CharacterAnims.PlayAnimsOnPanel(settings, false, false);
        panel.StartWeaponLookat();
        return;
      }
      if (bound && props.placing) return;
      if (bound) panel.RemoveItemModel();
      panel.SetActiveItem(active);
      panel.SetItemItemId(draftId, '');
      bound = true;
      if (props.placing && !armed) arm();
    });
  });
  createEffect(() => {
    const sticker = stickerIn(slot());
    if (props.placing && armed && sticker) scrape(sticker.Wear);
  });
  onCleanup(() => usePanel(undefined));
  return (
    <GenericPanel
      type="MapItemPreviewPanel"
      class={css.scene}
      map={game.backgroundMap()}
      pin-fov="vertical"
      require-composition-layer="true"
      transparent-background="false"
      panzoom_enabled="true"
      hide_while_waiting_for_composite_materials="true"
      tabindex="auto"
      selectionpos="auto"
      camera={props.first ? 'cam_default' : cameraOf(entry)}
      initial_entity="item"
      player={props.placing ? 'false' : 'true'}
      mouse_rotate={entry.kind === 'gloves' && !props.first ? 'false' : 'true'}
      disable-depth-of-field={props.first ? 'true' : 'false'}
      sticker_application_mode={props.placing ? 'true' : 'false'}
      ref={(created: Panel) => {
        panel = created as MapPreviewPanel;
        usePanel(panel);
      }}
    />
  );
}

function PlayerScene(props: { entry: Entry }) {
  let panel: MapPreviewPanel | undefined;
  let shown = 0;
  createEffect(() => {
    weapon();
    const current = revision();
    const settings = players(props.entry);
    if (!settings || current === 0) return;
    if (shown !== 0 && shown !== current) panel?.RemoveCharacterModel();
    shown = current;
    CharacterAnims.PlayAnimsOnPanel(settings);
  });
  onCleanup(() => usePanel(undefined));
  return (
    <GenericPanel
      type="MapPlayerPreviewPanel"
      class={css.scene}
      map={game.backgroundMap()}
      pin-fov="vertical"
      require-composition-layer="true"
      camera={untrack(zoomed) ? 'cam_char_inspect_closeup' : 'cam_char_inspect_wide'}
      playername="vanity_character"
      animgraphcharactermode="inventory-inspect"
      animgraphturns="false"
      mouse_rotate="true"
      ref={(created: Panel) => {
        panel = created as MapPreviewPanel;
        panel.SetActiveCharacter(5);
        usePanel(panel);
      }}
    />
  );
}

function MusicScene(props: { entry: Entry }) {
  createEffect(() => {
    if (revision() > 0) InventoryAPI.PlayItemPreviewMusic(draftId, '');
  });
  onCleanup(() => InventoryAPI.StopItemPreviewMusic());
  return <Image class={css.art} scaling="stretch-to-fit-preserve-aspect" src={props.entry.image} />;
}

function Scene() {
  const entry = untrack(selected);
  if (!entry || untrack(placing)) return null;
  if (entry.kind === 'music') return <MusicScene entry={entry} />;
  if (entry.kind === 'medal') return <Picture class={css.medal} id={entry.id} />;
  const shown = untrack(view);
  if (shown === 'agent') return <PlayerScene entry={entry} />;
  return <ItemScene entry={entry} first={shown === 'first'} placing={false} />;
}

function Placement(props: { entry: Entry }) {
  return (
    <Panel class={css.placement}>
      <ItemScene entry={props.entry} first={false} placing />
      <Presence motion="rise">
        <Panel class={css.banner}>
          <Label class={css.bannerText} text={t.placing} />
          <ActionButton class={css.bannerButton} label={t.nextPosition} onPress={nextPosition} />
          <ActionButton class={css.bannerButton} label={t.cancel} onPress={cancelPlacement} />
          <ActionButton variant="primary" class={css.bannerButton} label={t.save} onPress={savePlacement} />
        </Panel>
      </Presence>
    </Panel>
  );
}

export function Stage() {
  const key = createMemo(() => {
    const entry = selected();
    if (!entry || !tabShown()) return '';
    const what = shared(entry) ? entry.key : entry.item.Definition;
    const first = view() === 'first' ? revision() : 0;
    return `${what}:${view()}:${placing()}:${background()}:${first}:${wide()}`;
  });
  const favorite = () => {
    const entry = selected();
    return entry !== undefined && favorites().includes(entry.key);
  };
  return (
    <Panel class={css.stage} classList={{ [css.wide]: wide() }}>
      <Show when={selected()} fallback={<Label class={css.emptyTitle} text={t.selectItem} />}>
        {(entry: Accessor<Entry>) => (
          <>
            <Show when={key()} keyed>
              <Presence motion="appear">
                <Scene />
              </Presence>
            </Show>
            <Button
              class={css.favorite}
              classList={{ [css.on]: favorite() }}
              tabindex="auto"
              selectionpos="auto"
              tooltip={t.favorite}
              onactivate={() => toggleFavorite(entry().key)}
            >
              <Image class={css.star} src={favorite() ? icons.starFilled : icons.star} />
            </Button>
            <Toolbar entry={entry()} />
          </>
        )}
      </Show>
      <Show when={tabShown() && (placing() || curtain() !== '')}>
        <Layer>
          <Show when={placing() ? selected() : undefined} keyed>
            {(entry: Entry) => <Placement entry={entry} />}
          </Show>
          <Show when={curtain() !== ''}>
            <Panel
              class={css.curtain}
              classList={{ [css.opening]: curtain() === 'opening' }}
              hittest={false}
              onAnimationEnd={curtainDone}
            />
          </Show>
        </Layer>
      </Show>
    </Panel>
  );
}
