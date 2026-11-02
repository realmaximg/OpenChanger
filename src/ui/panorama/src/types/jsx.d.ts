declare namespace JSX {
  type Element = GenericPanel | Element[] | (() => Element) | string | number | boolean | null | undefined;

  interface ElementChildrenAttribute {
    children: unknown;
  }
}

type PanelEventName =
  | 'onactivate'
  | 'onmouseover'
  | 'onmouseout'
  | 'ontextentrychange'
  | 'onvaluechanged'
  | 'onblur'
  | 'oninputsubmit';

type PanelProps<T extends GenericPanel> = {
  id?: string;
  class?: string;
  classList?: Record<string, boolean>;
  style?: Partial<Record<keyof Style, string>>;
  hittest?: boolean;
  hittestchildren?: boolean;
  visible?: boolean;
  enabled?: boolean;
  tooltip?: string;
  focus?: boolean;
  tabindex?: string;
  selectionpos?: string;
  ref?: T | ((panel: T) => void);
  children?: JSX.Element;
  onAnimationEnd?: (panel: T, animation: string) => void;
  onImageLoaded?: (panel: T) => void;
  onImageFailedLoad?: (panel: T) => void;
  onScrollPositionChanged?: (panel: T) => void;
  onSliderReleased?: (panel: T, value: number) => void;
} & { [Name in PanelEventName]?: (panel: T) => void };

type LabelProps = PanelProps<Label> & { text: string | number };

type TextEntryProps = PanelProps<TextEntry> & {
  text?: string;
  maxchars?: number;
  textmode?: 'normal' | 'numeric';
};

type SliderProps = PanelProps<Slider> & {
  direction: 'horizontal' | 'vertical';
  min: number;
  max: number;
  value: number;
  increment?: number;
};

type ImageProps = PanelProps<Image> & { src: string; scaling?: string };

type GameProps = PanelProps<Panel> & {
  type: 'ItemImage' | 'MapItemPreviewPanel' | 'MapPlayerPreviewPanel';
  [attribute: string]: unknown;
};

declare const Panel: (props: PanelProps<Panel>) => JSX.Element;
declare const Button: (props: PanelProps<Button>) => JSX.Element;
declare const Label: (props: LabelProps) => JSX.Element;
declare const TextEntry: (props: TextEntryProps) => JSX.Element;
declare const Slider: (props: SliderProps) => JSX.Element;
declare const Image: (props: ImageProps) => JSX.Element;
declare const GenericPanel: (props: GameProps) => JSX.Element;
