import { type Accessor, Show } from 'solid-js';
import { Toast } from '../../../../components/Toast/Toast';
import { Presence } from '../../../../design/motion';
import type { Entry } from '../../catalog';
import { selected } from '../../editor';
import { message } from '../../toast';
import { Editor } from './Editor';
import { Stage } from './Stage';
import css from './Work.module.css';

export function Work() {
  return (
    <Panel class={css.work}>
      <Panel class={css.content}>
        <Stage />
        <Show when={selected()}>
          {(entry: Accessor<Entry>) => (
            <Presence motion="rise">
              <Editor entry={entry()} />
            </Presence>
          )}
        </Show>
      </Panel>
      <Show when={selected() === undefined}>
        <Panel class={css.toasts} hittest={false}>
          <Toast message={message()} />
        </Panel>
      </Show>
    </Panel>
  );
}
