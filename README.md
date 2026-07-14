# MorphOSC for KORG logue SDK synths

[English](#english) | [日本語](#日本語)

## English

### Introduction

MorphOSC is a custom user oscillator for compatible KORG logue SDK synthesizers. It is designed for expressive timbral movement: hold a note and turn the main `SHAPE` knob to continuously morph the oscillator waveform through five distinct characters:

```text
sine -> triangle -> saw -> pulse -> folded saw
```

The lower part of the range moves from soft and rounded to bright and harmonically rich. The upper part travels through pulse tones into an aggressive folded-saw texture. Transitions are smoothed for live knob movement, and the brighter shapes use the logue SDK band-limited wave tables where possible.

Additional parameters provide pulse-width control, wave folding, a sub oscillator, slow animation, tone shaping, and output-level adjustment. Ready-to-load packages are included for the original Nu:Tekt NTS-1 digital kit, minilogue xd, and prologue.

### Downloads

Pick the file for your instrument:

| Instrument | File |
| --- | --- |
| Nu:Tekt NTS-1 digital kit | [`dist/morph_osc.ntkdigunit`](dist/morph_osc.ntkdigunit) |
| minilogue xd | [`dist/morph_osc.mnlgxdunit`](dist/morph_osc.mnlgxdunit) |
| prologue | [`dist/morph_osc.prlgunit`](dist/morph_osc.prlgunit) |

Load the matching unit file with KORG Librarian into a user oscillator slot.

### Controls

| Control | Function |
| --- | --- |
| `SHAPE` | Main waveform morph. Turn this while holding a note. |
| `SHIFT + SHAPE` | Color/bite. Also nudges pulse width and fold brightness. |
| `WIDTH` | Pulse width range for the pulse part of the morph. |
| `FOLD` | Fold-drive amount for the final morph region. |
| `SUB` | One-octave-down sub oscillator mix. |
| `ANIM` | Slow internal motion that gently moves morph/color/phase. |
| `TONE` | Internal tone tilt, also follows the NTS-1 cutoff value. |
| `LEVEL` | Output level. |

### Build

Clone the KORG logue SDK into `vendor/logue-sdk`, or point `LOGUE_SDK_DIR` at an existing checkout.

Build all currently supported packages with:

```sh
make all-platforms
```

The installable files are written to `dist/`.

To build one platform:

```sh
make PLATFORM=nutekt-digital install
make PLATFORM=minilogue-xd install
make PLATFORM=prologue install
```

To use a different SDK checkout:

```sh
make LOGUE_SDK_DIR=/path/to/logue-sdk all-platforms
```

Supported `PLATFORM` values are `nutekt-digital`, `minilogue-xd`, and `prologue`.

NTS-1 mkII and microKORG2 use the newer logue SDK C++ unit API, so they need a separate port rather than just another package manifest.

## 日本語

KORG logue SDK用のユーザーオシレーターです。`SHAPE`つまみを回すと、サイン波から三角波、ノコギリ波、パルス波、折り返しノコギリ波へ連続的にモーフィングします。

現在はNTS-1 digital kit、minilogue xd、prologue向けのビルドを同梱しています。

最初は `FOLD` と `SUB` を低め、`LEVEL` を控えめにして、`SHAPE` をゆっくり回すのがおすすめです。
