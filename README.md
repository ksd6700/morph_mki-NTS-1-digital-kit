# MorphOSC for KORG NTS-1 digital kit

MorphOSC is a custom NTS-1 user oscillator whose main `SHAPE` knob continuously morphs the oscillator waveform:

```text
sine -> triangle -> saw -> pulse -> folded saw
```

The morph is smoothed for live knob movement, and the brighter shapes use the logue SDK band-limited wave tables where possible.

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

This workspace is already pointed at the local logue SDK found on this machine. Build and package with:

```sh
make install
```

The NTS-1 installable file is written to:

```text
dist/morph_osc.ntkdigunit
```

To use a different SDK checkout:

```sh
make LOGUE_SDK_DIR=/path/to/logue-sdk install
```

Load the `.ntkdigunit` with KORG Librarian into a user oscillator slot.

## 日本語メモ

NTS-1用のユーザーオシレーターです。`SHAPE`つまみを回すと、サイン波から三角波、ノコギリ波、パルス波、折り返しノコギリ波へ連続的にモーフィングします。

最初は `FOLD` と `SUB` を低め、`LEVEL` を控えめにして、`SHAPE` をゆっくり回すのがおすすめです。
