import { format, t } from '../../i18n';
import { api, type ItemRecord, ids, type LoadoutItem } from './api';
import type { Entry } from './catalog';
import { draft, newRecord, selected } from './editor';
import * as game from './game';
import { loadout, sameRecord, savedFor, shared, teamsOf } from './store';
import { toast } from './toast';

export function teamName(team: number) {
  return team === ids.CounterTerrorists ? 'CT' : team === ids.Terrorists ? 'T' : t.shared;
}

function sidesName(teams: number[]) {
  return teams.length > 1 ? t.both : teamName(teams[0]);
}

function installed(entry: Entry, record: ItemRecord, teams: number[]) {
  return teams.every((team) => {
    const saved = savedFor(entry, team);
    if (!saved) return false;
    return entry.kind === 'medal' ? saved.Record.Shown : sameRecord(saved.Record, record);
  });
}

function vanillaFirst(entry: Entry) {
  return entry.paint !== undefined && entry.kind === 'knife';
}

export type Target = { teams: number[]; label: string; installed: boolean; tooltip?: string };

export function targetsOf(entry: Entry, record: ItemRecord): Target[] {
  const teams = teamsOf(entry);
  if (shared(entry)) {
    const done = installed(entry, record, teams);
    return [{ teams, label: done ? t.uninstall : t.install, installed: done }];
  }
  const groups = teams.length > 1 ? [...teams.map((team) => [team]), teams] : [teams];
  return groups.map((group): Target => {
    const done = installed(entry, record, group);
    const tooltip = vanillaFirst(entry) ? t.restoreVanilla : t.uninstall;
    return {
      teams: group,
      label: group.length > 1 ? t.bothShort : teamName(group[0]),
      installed: done,
      tooltip: done ? tooltip : undefined,
    };
  });
}

export function recordName(item: LoadoutItem) {
  const base = game.baseName(item.Id);
  return item.Record.Name ? `«${item.Record.Name}» · ${base}` : base;
}

function vanilla(entry: Entry, team: number) {
  return newRecord({ ...entry, paint: undefined }, team);
}

function restore(entry: Entry, record: ItemRecord, teams: number[]) {
  for (const team of teams) {
    if (vanillaFirst(entry)) {
      const error = api.equip(vanilla(entry, team));
      if (error) return toast(error, 'bad');
    } else if (entry.kind === 'medal') api.removeMedal(entry.item.Definition);
    else api.unequip({ ...record, Team: team });
  }
  if (vanillaFirst(entry)) return toast(format(t.toastRestored, { team: sidesName(teams) }));
  toast(shared(entry) ? t.toastUninstalled : format(t.toastUninstalledFrom, { team: sidesName(teams) }));
}

export function install(teams: number[]) {
  const entry = selected();
  const record = draft();
  if (!entry || !record) return;
  if (installed(entry, record, teams)) return restore(entry, record, teams);
  for (const team of teams) {
    const error = entry.kind === 'medal' ? api.showMedal(entry.item.Definition) : api.equip({ ...record, Team: team });
    if (error) return toast(error, 'bad');
  }
  if (shared(entry)) return toast(t.toastInstalledShared);
  const away = teams.filter((team) => !savedFor(entry, team)?.InSlot);
  if (away.length === 0) return toast(format(t.toastInstalled, { team: sidesName(teams) }));
  toast(format(t.toastInInventory, { team: sidesName(away) }));
}

function take(item: LoadoutItem) {
  const { Record } = item;
  if (Record.Slot === ids.Medal) api.removeMedal(Record.Definition);
  else api.unequip(Record);
}

export function remove(item: LoadoutItem) {
  const name = recordName(item);
  take(item);
  toast(format(t.toastRemoved, { name }));
}

export function resetAll() {
  for (const item of loadout()) take(item);
  toast(t.toastReset);
}

export function copySide(from: number, to: number) {
  let copied = 0;
  let skipped = 0;
  for (const { Record } of loadout().filter((item) => item.Record.Team === from)) {
    if (api.equip({ ...Record, Team: to })) skipped++;
    else copied++;
  }
  const team = teamName(to);
  const text = format(t.toastCopied, { team, n: copied });
  toast(skipped ? `${text} · ${format(t.toastSkipped, { team, n: skipped })}` : text);
}
