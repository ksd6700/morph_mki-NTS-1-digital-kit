# MorphOSC for KORG logue SDK synths

MorphOSC is a custom KORG logue SDK user oscillator whose main `SHAPE` knob continuously morphs the oscillator waveform:

```text
sine -> triangle -> saw -> pulse -> folded saw
```

The morph is smoothed for live knob movement, and the brighter shapes use the logue SDK band-limited wave tables where possible.

## Downloads

Pick the file for your instrument:

| Instrument | File |
| --- | --- |
| Nu:Tekt NTS-1 digital kit | [`dist/morph_osc.ntkdigunit`](dist/morph_osc.ntkdigunit) |
| minilogue xd | [`dist/morph_osc.mnlgxdunit`](dist/morph_osc.mnlgxdunit) |
| prologue | [`dist/morph_osc.prlgunit`](dist/morph_osc.prlgunit) |

Load the matching unit file with KORG Librarian into a user oscillator slot.

## Controls

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

## Build

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

## 日本語メモ

KORG logue SDK用のユーザーオシレーターです。`SHAPE`つまみを回すと、サイン波から三角波、ノコギリ波、パルス波、折り返しノコギリ波へ連続的にモーフィングします。

現在はNTS-1 digital kit、minilogue xd、prologue向けのビルドを同梱しています。

最初は `FOLD` と `SUB` を低め、`LEVEL` を控えめにして、`SHAPE` をゆっくり回すのがおすすめです。
