import css from './SliderField.module.css';

type SliderFieldProps = {
  value: number;
  min: number;
  max: number;
  step: number;
  onChange: (value: number) => void;
  onRelease: (value: number) => void;
};

export function SliderField(props: SliderFieldProps) {
  return (
    <Slider
      class={`HorizontalSlider ${css.slider}`}
      direction="horizontal"
      min={props.min}
      max={props.max}
      increment={props.step}
      value={props.value}
      tabindex="auto"
      selectionpos="auto"
      onvaluechanged={(slider) => props.onChange(slider.value)}
      onSliderReleased={(slider) => props.onRelease(slider.value)}
    />
  );
}
