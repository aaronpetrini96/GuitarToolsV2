---
name: Iron & Voltage
colors:
  surface: '#141313'
  surface-dim: '#141313'
  surface-bright: '#3a3939'
  surface-container-lowest: '#0e0e0e'
  surface-container-low: '#1c1b1b'
  surface-container: '#201f1f'
  surface-container-high: '#2a2a2a'
  surface-container-highest: '#353435'
  on-surface: '#e5e2e1'
  on-surface-variant: '#c5c6ca'
  inverse-surface: '#e5e2e1'
  inverse-on-surface: '#313030'
  outline: '#8f9194'
  outline-variant: '#44474a'
  surface-tint: '#c6c6c9'
  primary: '#c6c6c9'
  on-primary: '#2f3133'
  primary-container: '#1a1c1e'
  on-primary-container: '#838486'
  inverse-primary: '#5d5e61'
  secondary: '#c3c7cd'
  on-secondary: '#2c3136'
  secondary-container: '#454a4f'
  on-secondary-container: '#b4b9bf'
  tertiary: '#cfc5be'
  on-tertiary: '#352f2b'
  tertiary-container: '#201b17'
  on-tertiary-container: '#8b837d'
  error: '#ffb4ab'
  on-error: '#690005'
  error-container: '#93000a'
  on-error-container: '#ffdad6'
  primary-fixed: '#e2e2e5'
  primary-fixed-dim: '#c6c6c9'
  on-primary-fixed: '#1a1c1e'
  on-primary-fixed-variant: '#454749'
  secondary-fixed: '#dfe3e9'
  secondary-fixed-dim: '#c3c7cd'
  on-secondary-fixed: '#171c20'
  on-secondary-fixed-variant: '#43474c'
  tertiary-fixed: '#ebe0da'
  tertiary-fixed-dim: '#cfc5be'
  on-tertiary-fixed: '#201b17'
  on-tertiary-fixed-variant: '#4c4641'
  background: '#141313'
  on-background: '#e5e2e1'
  surface-variant: '#353435'
  chassis-steel: '#121417'
  panel-recess: '#0A0B0D'
  led-amber: '#FFBF00'
  led-red: '#EF4444'
  led-green: '#22C55E'
  silkscreen-ink: '#A0A0A0'
  bakelite-black: '#080808'
typography:
  headline-lg:
    fontFamily: Geist
    fontSize: 20px
    fontWeight: '700'
    lineHeight: 28px
    letterSpacing: 0.1em
  panel-header:
    fontFamily: Geist
    fontSize: 14px
    fontWeight: '800'
    lineHeight: 20px
    letterSpacing: 0.15em
  label-md:
    fontFamily: JetBrains Mono
    fontSize: 12px
    fontWeight: '500'
    lineHeight: 16px
    letterSpacing: 0.02em
  value-display:
    fontFamily: JetBrains Mono
    fontSize: 13px
    fontWeight: '700'
    lineHeight: 16px
    letterSpacing: -0.02em
  brand-stamp:
    fontFamily: Geist
    fontSize: 16px
    fontWeight: '900'
    lineHeight: 24px
    letterSpacing: 0.2em
rounded:
  sm: 0.125rem
  DEFAULT: 0.25rem
  md: 0.375rem
  lg: 0.5rem
  xl: 0.75rem
  full: 9999px
spacing:
  panel-margin: 24px
  gutter: 16px
  component-gap: 12px
  internal-padding: 20px
---

## Brand & Style

The design system evokes the rugged, uncompromising aesthetic of mid-century rack-mounted studio gear and boutique guitar amplifiers. It targets professional audio engineers and guitarists who value the tactile, "heavy-duty" feel of physical hardware. 

The style is **Analog Skeuomorphism**. It prioritizes physical realism through simulated materials: powder-coated steel chassis, machined aluminum knobs, and silkscreened technical labels. The emotional response is one of reliability, precision, and "musical" warmth. Every element must feel like it has weight, resistance, and a physical state, moving away from flat digital abstractions toward a functional, industrial object.

## Colors

The palette is anchored in a monochromatic "hardware" foundation with high-visibility functional accents. 

- **Chassis & Panels:** Use `chassis-steel` as the primary background. Apply a subtle grain or "brushed" texture overlay to simulate metal.
- **Recesses:** Use `panel-recess` for etched areas, slider tracks, and shadows around knobs to create depth.
- **Typography:** Labels are rendered in `silkscreen-ink`, simulating white or light gray paint applied to a dark surface.
- **Status Indicators:** Use the LED colors for active states. These should have a "glow" effect (box-shadow) when active, and a dull, dark-transparent appearance when inactive.

## Typography

Typography is treated as physical marking on hardware. Use **Geist** for structural labels and branding, and **JetBrains Mono** for technical readouts and variable values.

- **Silkscreen Effect:** All text labels should use a slightly reduced opacity (85-90%) or a subtle "knockout" effect to look like ink on metal. 
- **Readouts:** Values (like "40 Hz") should appear as if they are on a small recessed LCD or LED screen, using `value-display` tokens with the `led-green` or `led-amber` color.
- **Hierarchy:** Headers use uppercase and wide letter-spacing to mimic industrial engraving.

## Layout & Spacing

The layout follows a **Fixed Grid** model, mimicking a 19-inch rack unit. The interface is divided into three distinct vertical modules ("Cut Filters", "Magic Buttons", "Expression").

- **Module Definition:** Each module is separated by a visible "seam" or a physical gap in the metal chassis.
- **Alignment:** Components within modules are centered along a vertical axis, creating a symmetrical, rack-mount appearance.
- **Margins:** Consistent `panel-margin` ensures the controls don't feel crowded near the edges of the "casing."

## Elevation & Depth

Hierarchy is established through skeuomorphic depth rather than layers.

- **Knobs:** Use a 45-degree light source. Add a sharp highlight on the top-left and a deep drop shadow on the bottom-right. Machined aluminum knobs should have a radial "anodized" gradient.
- **Recessed Areas:** Slider tracks and value readouts must have an `inner-shadow` to appear sunken into the front panel.
- **Toggle Switches:** Physical switches should have a "baseplate" shadow and a highlight on the "on" side of the rocker.
- **Screws:** Place small, hex-head or Phillips-head screw graphics at the four corners of each panel module to reinforce the hardware metaphor.

## Shapes

The design uses a "Soft" industrial logic. While the main chassis might have sharp corners, individual panel inserts and buttons have slight rounding (`0.25rem`) to reflect manufactured parts.

- **Knobs:** Perfectly circular with a 1px "rim" highlight.
- **Buttons:** Use `roundedness: 1` for a "chicklet" switch feel. 
- **Indicators:** Small circular LEDs for "on" states.

## Components

- **Rotary Knobs:** Designed as "Machined Aluminum." A dark circular body with a notched pointer. The pointer line should be `led-amber` or `led-red` to match the panel's accent.
- **Toggle Switches:** Used for "Magic Buttons." These are 3D rocker switches. "Up" is off (dull); "Down" is on (revealing an LED-colored highlight).
- **Faders (Sliders):** Use a horizontal recessed "trench" (`panel-recess`). The handle (thumb) is a rectangular block with a single vertical "grip" line in the center.
- **LCD Readouts:** Numeric values appear in a recessed black box with a subtle inner glow. Text is `led-green` or `led-amber`.
- **Power Button:** A large, physical "Killer Switch" or a heavy-duty circular push-button with a tactile ring of light when engaged.
- **LED Indicators:** Small, dome-shaped lights that "bloom" slightly when active. Use for the CLIP indicator and button states.