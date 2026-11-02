import { For } from 'solid-js';
import { t } from '../../../i18n';
import { tabShown } from '../../../shell/tab';
import { category, mode, pickCategory, setMode } from '../browser';
import { categoryName, kinds } from '../catalog';
import css from './Header.module.css';

type HeaderTabProps = { label: string; active: boolean; onPress: () => void };

function HeaderTab(props: HeaderTabProps) {
  return (
    <Button
      class={css.tab}
      classList={{ [css.active]: props.active }}
      focus={tabShown() && props.active}
      tabindex="auto"
      selectionpos="auto"
      onactivate={() => props.onPress()}
    >
      <Label class={css.label} text={props.label} />
    </Button>
  );
}

export function Header() {
  return (
    <Panel class={css.header}>
      <For each={kinds}>
        {(kind) => (
          <HeaderTab
            label={categoryName(kind)}
            active={mode() !== 'mine' && category() === kind}
            onPress={() => pickCategory(kind)}
          />
        )}
      </For>
      <HeaderTab label={t.mine} active={mode() === 'mine'} onPress={() => setMode('mine')} />
    </Panel>
  );
}
