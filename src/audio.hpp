#ifndef AUDIO_HPP
#define AUDIO_HPP

// This implementation is directly translated from my game engine, bonsai audio
// package. (with some minor changes)

#include <array>
#include <cstdint>
#include <mutex>
#include <optional>
#include <vector>

#pragma pack(push, 1)

struct WavHeader {
  char riff_tag[4];
  uint32_t file_size;
  char wave_tag[4];
  char fmt_tag[4];
  uint32_t fmt_size;
  uint16_t audio_format; // 1 - pcm, 3 = ieee
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
};

#pragma pack(pop)

struct ParseResult {
  std::vector<float> samples;
  int channels;
  int sample_rate;
};

struct Sound {
  std::vector<float> samples;
  int channels;
  int sample_rate;
};

struct Voice {
  const Sound *sound = nullptr;
  size_t cursor = 0;
  bool active = false;
  bool looping = false;
};

constexpr size_t MAX_VOICES = 16;
static std::array<Voice, MAX_VOICES> voices;
static std::mutex mixer_lock;

std::optional<ParseResult> ParseFromBytes(const std::vector<uint8_t> &data);
void AudioCallback(float *buffer, int num_frames, int num_channels);

void PlaySound(const Sound *sound, bool looped = false);
void StopSound(const Sound *sound);

extern Sound sfx_ufo;
extern Sound sfx_shoot;
extern Sound sfx_player_die;
extern Sound sfx_invader_die;
extern Sound sfx_fleet_1;
extern Sound sfx_fleet_2;
extern Sound sfx_fleet_3;
extern Sound sfx_fleet_4;
extern Sound sfx_ufo_hit;
extern Sound sfx_extra_ship;

#endif
