export type {
  Charm,
  Item,
  ItemKind,
  ItemRecord,
  Kit,
  KitKind,
  LoadoutItem,
  Paint,
  Sticker,
} from '../../../generated/api';

export const api = OpenChangerAPI.changer;
export const ids = api.ids();
