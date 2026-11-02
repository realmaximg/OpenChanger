import { children, createRenderEffect } from 'solid-js';
import css from './motion.module.css';

type Motion = 'appear' | 'fade' | 'rise' | 'drop';
type Effect = 'press';

type MotionClasses = { enter: string; leave?: string; leaveAnimation?: string };

const motions: Record<Motion, MotionClasses> = {
  appear: { enter: css.enterFade },
  fade: { enter: css.enterFade, leave: css.leaveFade, leaveAnimation: css.fadeOut },
  rise: { enter: css.enterRise, leave: css.leaveRise, leaveAnimation: css.riseOut },
  drop: { enter: css.enterDrop, leave: css.leaveDrop, leaveAnimation: css.dropOut },
};

const effects: Record<Effect, string> = { press: css.press };

const motionAttribute = 'motion';

function shown(panel: GenericPanel) {
  for (let current: GenericPanel | null = panel; current; current = current.GetParent()) {
    if (!current.visible) return false;
  }
  return true;
}

function motionOf(panel: GenericPanel) {
  return motions[panel.GetAttributeString(motionAttribute, '') as Motion] as MotionClasses | undefined;
}

export function isLeaving(panel: GenericPanel) {
  const motion = motionOf(panel);
  return motion?.leave !== undefined && panel.BHasClass(motion.leave);
}

export function leave(panel: GenericPanel) {
  const motion = motionOf(panel);
  if (!motion?.leave || !shown(panel)) return false;
  panel.AddClass(motion.leave);
  return true;
}

export function animationEnded(panel: GenericPanel, animationName: string) {
  const motion = motionOf(panel);
  if (motion?.leave && animationName === motion.leaveAnimation && panel.BHasClass(motion.leave)) panel.DeleteAsync(0);
}

export function triggerClass(panel: GenericPanel, effect: Effect) {
  panel.TriggerClass(effects[effect]);
}

export function Presence(props: { motion: Motion; children: JSX.Element }) {
  const resolved = children(() => props.children);
  createRenderEffect(() => {
    const motion = motions[props.motion];
    for (const item of resolved.toArray()) {
      if (typeof item !== 'object' || item === null) continue;
      const panel = item as unknown as GenericPanel;
      panel.AddClass(motion.enter);
      if (motion.leave) panel.SetAttributeString(motionAttribute, props.motion);
    }
  });
  return resolved;
}
