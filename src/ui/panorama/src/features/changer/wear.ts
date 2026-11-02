import { t } from '../../i18n';
import * as game from './game';

export type Band = { from: number; to: number; label: string; name: string };

const limits = [0, 0.07, 0.15, 0.38, 0.45, 1];
const short = [t.wearShort0, t.wearShort1, t.wearShort2, t.wearShort3, t.wearShort4];

export const bands: Band[] = short.map((label, index) => ({
  from: limits[index],
  to: limits[index + 1],
  label,
  name: game.localized(`#SFUI_InvTooltip_Wear_Amount_${index}`, label),
}));

export function bandOf(wear: number) {
  return bands.find((band) => wear < band.to) ?? bands[bands.length - 1];
}
