import { For } from 'solid-js';
import { Presence } from '../../design/motion';
import css from './Toast.module.css';

type Message = {
  text: string;
  tone: 'ok' | 'bad';
};

export function Toast(props: { message: Message | undefined }) {
  return (
    <For each={props.message ? [props.message] : []}>
      {(message) => (
        <Presence motion="rise">
          <Panel class={css.toast} hittest={false}>
            <Panel class={css.body} classList={{ [css.bad]: message.tone === 'bad' }}>
              <Label class={css.text} text={message.text} />
            </Panel>
          </Panel>
        </Presence>
      )}
    </For>
  );
}
