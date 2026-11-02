import { ActionButton } from '../../../../components/ActionButton/ActionButton';
import { TextField } from '../../../../components/TextField/TextField';
import { icons } from '../../../../design/icons';
import { t } from '../../../../i18n';
import { draft, edit } from '../../editor';
import css from './NameTab.module.css';

const limit = 20;

export function NameTab() {
  const name = () => draft()?.Name ?? '';
  return (
    <Panel class={css.row}>
      <TextField
        class={css.field}
        value={name()}
        placeholder={t.tagPlaceholder}
        maxchars={limit}
        onChange={(text) => edit({ Name: text.slice(0, limit) })}
      />
      <ActionButton
        class={css.remove}
        variant="danger"
        icon={icons.close}
        tooltip={t.removeTag}
        enabled={name() !== ''}
        onPress={() => edit({ Name: '' })}
      />
    </Panel>
  );
}
