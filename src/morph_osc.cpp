#include "userosc.h"

namespace {

constexpr float kDefaultMorph = 0.0f;
constexpr float kDefaultColor = 0.50f;
constexpr float kDefaultWidth = 0.50f;
constexpr float kDefaultFold = 0.18f;
constexpr float kDefaultSub = 0.0f;
constexpr float kDefaultAnim = 0.0f;
constexpr float kDefaultTone = 0.72f;
constexpr float kDefaultLevel = 0.78f;

struct MorphParams {
  float morph;
  float color;
  float width;
  float fold;
  float sub;
  float anim;
  float tone;
  float level;

  float target_morph;
  float target_color;
  float target_width;
  float target_fold;
  float target_sub;
  float target_anim;
  float target_tone;
  float target_level;
};

struct MorphState {
  float phase;
  float sub_phase;
  float w0;
  float target_w0;
  float anim_phase;
  float lp;
  float dc;
  float prev;
};

MorphParams s_params;
MorphState s_state;

inline float absf(float x) {
  return x < 0.0f ? -x : x;
}

inline float clamp(float x, float lo, float hi) {
  return x < lo ? lo : (x > hi ? hi : x);
}

inline float clamp01(float x) {
  return clamp(x, 0.0f, 1.0f);
}

inline float lerp(float a, float b, float t) {
  return a + (b - a) * t;
}

inline float smoothstep(float x) {
  x = clamp01(x);
  return x * x * (3.0f - 2.0f * x);
}

inline float wrap01(float x) {
  while (x >= 1.0f) {
    x -= 1.0f;
  }
  while (x < 0.0f) {
    x += 1.0f;
  }
  return x;
}

inline float sin01(float phase) {
  return osc_sinf(wrap01(phase));
}

inline float soft_limit(float x) {
  return x / (1.0f + absf(x) * 0.34f);
}

inline float tri_wave(float phase) {
  const float p = wrap01(phase);
  return (p < 0.5f) ? (p * 4.0f - 1.0f) : (3.0f - p * 4.0f);
}

inline float folded(float x) {
  x = clamp(x, -3.0f, 3.0f);
  if (x > 1.0f) {
    x = 2.0f - x;
  }
  if (x < -1.0f) {
    x = -2.0f - x;
  }
  return x;
}

inline float safe_bl_idx(float idx) {
  return clamp(idx, 0.0f, 5.999f);
}

inline float note_float(uint16_t pitch) {
  return static_cast<float>(pitch >> 8) + static_cast<float>(pitch & 0xff) * (1.0f / 256.0f);
}

float pulse_wave(float phase, float width, float saw_idx) {
  const float p = wrap01(phase);
  const float p2 = wrap01(p + clamp(width, 0.045f, 0.955f));
  const float a = osc_bl2_sawf(p, saw_idx);
  const float b = osc_bl2_sawf(p2, saw_idx);
  return soft_limit((a - b) * 0.78f);
}

float render_morph_wave(float phase,
                        float saw_idx,
                        float sqr_idx,
                        float morph,
                        float color,
                        float width,
                        float fold) {
  const float p = wrap01(phase);
  const float sine = sin01(p);
  const float tri = tri_wave(p);
  const float saw = osc_bl2_sawf(p, saw_idx);

  const float pulse_width = clamp(0.10f + width * 0.80f + (color - 0.5f) * 0.22f, 0.045f, 0.955f);
  const float pulse = lerp(osc_bl2_sqrf(p, sqr_idx), pulse_wave(p, pulse_width, saw_idx), 0.68f);

  const float second = sin01(p * 2.0f + color * 0.11f);
  const float drive = 1.05f + fold * 2.65f + color * 0.55f;
  const float fold_src = saw * drive + second * (0.10f + fold * 0.30f);
  const float fold_wave = soft_limit(folded(fold_src) * (1.15f + fold * 0.55f));

  const float pos = clamp01(morph) * 4.0f;
  uint32_t segment = static_cast<uint32_t>(pos);
  if (segment > 3u) {
    segment = 3u;
  }
  const float t = smoothstep(pos - static_cast<float>(segment));

  switch (segment) {
  default:
  case 0u:
    return lerp(sine, tri, t);
  case 1u:
    return lerp(tri, saw, t);
  case 2u:
    return lerp(saw, pulse, t);
  case 3u:
    return lerp(pulse, fold_wave, t);
  }
}

void set_defaults() {
  s_params.target_morph = kDefaultMorph;
  s_params.target_color = kDefaultColor;
  s_params.target_width = kDefaultWidth;
  s_params.target_fold = kDefaultFold;
  s_params.target_sub = kDefaultSub;
  s_params.target_anim = kDefaultAnim;
  s_params.target_tone = kDefaultTone;
  s_params.target_level = kDefaultLevel;

  s_params.morph = s_params.target_morph;
  s_params.color = s_params.target_color;
  s_params.width = s_params.target_width;
  s_params.fold = s_params.target_fold;
  s_params.sub = s_params.target_sub;
  s_params.anim = s_params.target_anim;
  s_params.tone = s_params.target_tone;
  s_params.level = s_params.target_level;
}

inline void slew_params() {
  constexpr float fast = 0.0065f;
  constexpr float med = 0.0035f;

  s_params.morph += (s_params.target_morph - s_params.morph) * fast;
  s_params.color += (s_params.target_color - s_params.color) * fast;
  s_params.width += (s_params.target_width - s_params.width) * med;
  s_params.fold += (s_params.target_fold - s_params.fold) * med;
  s_params.sub += (s_params.target_sub - s_params.sub) * med;
  s_params.anim += (s_params.target_anim - s_params.anim) * med;
  s_params.tone += (s_params.target_tone - s_params.tone) * med;
  s_params.level += (s_params.target_level - s_params.level) * med;
}

void reset_audio_state() {
  s_state.phase = 0.0f;
  s_state.sub_phase = 0.0f;
  s_state.anim_phase = 0.0f;
  s_state.lp = 0.0f;
  s_state.dc = 0.0f;
  s_state.prev = 0.0f;
}

} // namespace

void OSC_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;

  set_defaults();
  reset_audio_state();
  s_state.w0 = osc_w0f_for_note(60, 0);
  s_state.target_w0 = s_state.w0;
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames) {
  q31_t * __restrict y = reinterpret_cast<q31_t *>(yn);

  const uint16_t pitch = params ? params->pitch : static_cast<uint16_t>(60u << 8);
  const float note = note_float(pitch);
  const float sub_note = clamp(note - 12.0f, 0.0f, 151.0f);
  const float saw_idx = safe_bl_idx(osc_bl_saw_idx(note));
  const float sqr_idx = safe_bl_idx(osc_bl_sqr_idx(note));
  const float sub_sqr_idx = safe_bl_idx(osc_bl_sqr_idx(sub_note));

  s_state.target_w0 = osc_w0f_for_note(pitch >> 8, pitch & 0xff);

  const float shape_lfo = params ? q31_to_f32(params->shape_lfo) : 0.0f;
  const float panel_cutoff = params ? clamp01(static_cast<float>(params->cutoff) * (1.0f / 8191.0f)) : 0.78f;
  const float panel_reso = params ? clamp01(static_cast<float>(params->resonance) * (1.0f / 8191.0f)) : 0.0f;

  for (uint32_t i = 0; i < frames; ++i) {
    slew_params();

    const float glide = 0.0028f + (1.0f - s_params.anim) * 0.0022f;
    s_state.w0 += (s_state.target_w0 - s_state.w0) * glide;
    s_state.phase = wrap01(s_state.phase + s_state.w0);
    s_state.sub_phase = wrap01(s_state.sub_phase + s_state.w0 * 0.5f);

    const float anim_rate = 0.05f + s_params.anim * s_params.anim * 5.30f;
    s_state.anim_phase = wrap01(s_state.anim_phase + anim_rate * k_samplerate_recipf);

    const float morph_wobble = sin01(s_state.anim_phase) * s_params.anim * 0.075f;
    const float lfo_depth = 0.10f + s_params.anim * 0.12f;
    const float morph = clamp01(s_params.morph + shape_lfo * lfo_depth + morph_wobble);

    const float color_motion = sin01(s_state.anim_phase * 0.373f + 0.19f) * s_params.anim * 0.10f;
    const float color = clamp01(s_params.color + color_motion);

    const float phase_warp =
        sin01(s_state.anim_phase * 0.619f + s_state.phase * 0.5f) *
        s_params.anim * (0.0015f + s_params.fold * 0.0025f);
    const float read_phase = wrap01(s_state.phase + phase_warp + s_state.prev * s_params.fold * 0.0030f);

    float out = render_morph_wave(read_phase,
                                  saw_idx,
                                  sqr_idx,
                                  morph,
                                  color,
                                  s_params.width,
                                  s_params.fold);

    const float sub_square = osc_bl2_sqrf(s_state.sub_phase, sub_sqr_idx);
    const float sub_sine = sin01(s_state.sub_phase);
    const float sub_wave = lerp(sub_sine, sub_square, color * 0.70f);
    out = out * (1.0f - s_params.sub * 0.25f) + sub_wave * (s_params.sub * 0.46f);

    const float bite = (render_morph_wave(wrap01(read_phase + 0.0035f + color * 0.010f),
                                          saw_idx,
                                          sqr_idx,
                                          morph,
                                          color,
                                          s_params.width,
                                          s_params.fold) - out);
    out += bite * s_params.fold * (0.08f + color * 0.10f);

    s_state.dc += (out - s_state.dc) * 0.00055f;
    out -= s_state.dc;

    const float tone = clamp01(s_params.tone * 0.68f + panel_cutoff * 0.32f);
    const float lp_coef = clamp(0.018f + tone * tone * 0.48f + panel_reso * 0.035f, 0.018f, 0.62f);
    s_state.lp += (out - s_state.lp) * lp_coef;
    const float high = out - s_state.lp;
    out = s_state.lp + high * (0.25f + tone * 1.18f + panel_reso * 0.10f);

    out = soft_limit(out * (0.92f + s_params.fold * 0.30f)) * s_params.level;
    out = clamp(out, -0.96f, 0.96f);
    s_state.prev = out;

    y[i] = f32_to_q31(out);
  }
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  reset_audio_state();

  if (params) {
    s_state.w0 = osc_w0f_for_note(params->pitch >> 8, params->pitch & 0xff);
    s_state.target_w0 = s_state.w0;
  }
}

void OSC_NOTEOFF(const user_osc_param_t * const params) {
  (void)params;
}

void OSC_MUTE(const user_osc_param_t * const params) {
  (void)params;
  reset_audio_state();
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
  case k_user_osc_param_id1:
    s_params.target_width = clamp01(static_cast<float>(value) * 0.01f);
    break;

  case k_user_osc_param_id2:
    s_params.target_fold = clamp01(static_cast<float>(value) * 0.01f);
    break;

  case k_user_osc_param_id3:
    s_params.target_sub = clamp01(static_cast<float>(value) * 0.01f);
    break;

  case k_user_osc_param_id4:
    s_params.target_anim = clamp01(static_cast<float>(value) * 0.01f);
    break;

  case k_user_osc_param_id5:
    s_params.target_tone = clamp01(static_cast<float>(value) * 0.01f);
    break;

  case k_user_osc_param_id6:
    s_params.target_level = 0.08f + clamp01(static_cast<float>(value) * 0.01f) * 0.90f;
    break;

  case k_user_osc_param_shape:
    s_params.target_morph = clamp01(param_val_to_f32(value));
    break;

  case k_user_osc_param_shiftshape:
    s_params.target_color = clamp01(param_val_to_f32(value));
    break;

  default:
    break;
  }
}
