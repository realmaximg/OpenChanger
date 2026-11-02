import { icons } from '../../design/icons';
import { t } from '../../i18n';
import { ActionButton } from '../ActionButton/ActionButton';
import { DropButton } from '../DropButton/DropButton';
import css from './SortControls.module.css';

type SortControlsProps<Field extends string> = {
  fields: Field[];
  names: Record<Field, string>;
  field: Field;
  descending: boolean;
  onSort: (field: Field) => void;
  onFlip: () => void;
};

export function SortControls<Field extends string>(props: SortControlsProps<Field>) {
  return (
    <>
      <DropButton
        value={props.names[props.field]}
        items={() =>
          props.fields.map((field) => ({
            label: props.names[field],
            current: props.field === field,
            on: () => props.onSort(field),
          }))
        }
      />
      <ActionButton
        class={css.direction}
        icon={props.descending ? icons.down : icons.up}
        tooltip={props.descending ? t.descending : t.ascending}
        onPress={props.onFlip}
      />
    </>
  );
}
