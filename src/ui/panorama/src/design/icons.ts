const controls = 'file://{images}/control_icons';

export function iconPath(path: string) {
  return `s2r://panorama/images/icons/${path}.vsvg`;
}

export const icons = {
  back: `${controls}/arrow_solid_left.png`,
  up: `${controls}/arrow_solid_up.png`,
  down: `${controls}/arrow_solid_down.png`,
  close: `${controls}/x_close.png`,
  search: iconPath('ui/search'),
  random: iconPath('ui/random'),
  star: iconPath('ui/favorite_star_empty'),
  starFilled: iconPath('ui/favorite_star_filled'),
  missing: iconPath('ui/picture'),
};
