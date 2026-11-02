import { Index, Show } from 'solid-js';
import { ActionButton } from '../../../../components/ActionButton/ActionButton';
import { Toast } from '../../../../components/Toast/Toast';
import { t } from '../../../../i18n';
import type { Entry } from '../../catalog';
import { draft, problem } from '../../editor';
import { install, type Target, targetsOf } from '../../install';
import { shared } from '../../store';
import { message } from '../../toast';
import css from './InstallBar.module.css';

function TargetButton(props: { target: Target; side: boolean }) {
  return (
    <ActionButton
      variant={props.target.installed ? 'installed' : 'primary'}
      class={props.side ? (props.target.teams.length > 1 ? css.both : css.side) : css.single}
      label={props.target.label}
      tooltip={props.target.tooltip}
      onPress={() => install(props.target.teams)}
    />
  );
}

export function InstallBar(props: { entry: Entry }) {
  const targets = () => {
    const current = draft();
    return current ? targetsOf(props.entry, current) : [];
  };
  const side = () => !shared(props.entry);
  return (
    <Panel class={css.bar}>
      <Panel class={css.note}>
        <Show when={problem()} fallback={<Toast message={message()} />}>
          <Label class={css.problem} text={problem()} tooltip={problem()} />
        </Show>
      </Panel>
      <Panel class={css.targets}>
        <Show when={side()}>
          <Label class={css.caption} text={t.installForLabel} />
        </Show>
        <Index each={targets()}>{(target) => <TargetButton target={target()} side={side()} />}</Index>
      </Panel>
    </Panel>
  );
}
