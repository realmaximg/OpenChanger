import { drill, hideKits, mode, openDrill } from './browser';
import { cancelPlacement, placing } from './editor';

export function back() {
  if (placing()) cancelPlacement();
  else if (mode() === 'kits') hideKits();
  else if (mode() === 'catalog' && drill()) openDrill(undefined);
  else return false;
  return true;
}
