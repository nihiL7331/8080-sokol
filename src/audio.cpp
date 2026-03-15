#include "audio.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <vector>

Sound sfx_ufo;
Sound sfx_shoot;
Sound sfx_player_die;
Sound sfx_invader_die;
Sound sfx_fleet_1;
Sound sfx_fleet_2;
Sound sfx_fleet_3;
Sound sfx_fleet_4;
Sound sfx_ufo_hit;
Sound sfx_extra_ship;

std::optional<ParseResult> ParseFromBytes(const std::vector<uint8_t> &data) {
  if (data.size() < sizeof(WavHeader)) {
    std::cerr << "Failed to parse WAV. Data is too small." << std::endl;
    return std::nullopt;
  }

  const WavHeader *header = reinterpret_cast<const WavHeader *>(data.data());

  if (std::memcmp(header->riff_tag, "RIFF", 4) != 0 ||
      std::memcmp(header->wave_tag, "WAVE", 4) != 0) {
    std::cerr << "Invalid WAV file signature." << std::endl;
    return std::nullopt;
  }

  if (header->audio_format != 1 && header->audio_format != 3) {
    std::cerr << "Unsupported WAV format: " << header->audio_format
              << " (Only PCM or IEEE Float supported)" << std::endl;
    return std::nullopt;
  }

  if (header->bits_per_sample != 8 && header->bits_per_sample != 16 &&
      header->bits_per_sample != 32) {
    std::cerr << "Unsupported WAV bit depth: " << header->bits_per_sample
              << " (Only 8, 16, or 32 supported)" << std::endl;
    return std::nullopt;
  }

  size_t cursor = 12;

  if (std::memcmp(&data[cursor], "fmt ", 4) != 0) {
    return std::nullopt;
  }

  cursor += 8 + header->fmt_size;

  bool data_found = false;
  uint32_t data_size = 0;

  while (cursor < data.size()) {
    if (cursor + 8 > data.size())
      break;

    const char *chunk_tag = reinterpret_cast<const char *>(&data[cursor]);
    uint32_t chunk_size =
        *reinterpret_cast<const uint32_t *>(&data[cursor + 4]);

    if (std::memcmp(chunk_tag, "data", 4) == 0) {
      data_found = true;
      data_size = chunk_size;
      cursor += 8;
      break;
    }

    uint32_t padding = chunk_size % 2;
    cursor += 8 + chunk_size + padding;
  }

  if (!data_found) {
    std::cerr << "Couldn't find data tag in the WAV file." << std::endl;
    return std::nullopt;
  }

  if (cursor + data_size > data.size()) {
    std::cerr << "WAV data chunk size exceeds file size." << std::endl;
    return std::nullopt;
  }

  ParseResult result;
  result.channels = header->num_channels;
  result.sample_rate = header->sample_rate;

  int total_samples = data_size / (header->bits_per_sample / 8);
  result.samples.resize(total_samples);

  const uint8_t *pcm_data = &data[cursor];

  if (header->bits_per_sample == 16) {
    const int16_t *source = reinterpret_cast<const int16_t *>(pcm_data);
    for (int i = 0; i < total_samples; ++i) {
      result.samples[i] = static_cast<float>(source[i]) / 32768.0f;
    }
  } else if (header->bits_per_sample == 8) {
    for (int i = 0; i < total_samples; ++i) {
      result.samples[i] = (static_cast<float>(pcm_data[i]) - 128.0f) / 128.0f;
    }
  } else if (header->bits_per_sample == 32) {
    const float *source = reinterpret_cast<const float *>(pcm_data);
    std::copy(source, source + total_samples, result.samples.begin());
  }

  return result;
}

void PlaySound(const Sound *sound, bool looped) {
  if (!sound || sound->samples.empty())
    return;

  std::lock_guard<std::mutex> lock(mixer_lock);

  for (auto &voice : voices) {
    if (!voice.active) {
      voice.sound = sound;
      voice.cursor = 0;
      voice.active = true;
      voice.looping = looped;
      return;
    }
  }
}

void StopSound(const Sound *sound) {
  if (!sound)
    return;

  std::lock_guard<std::mutex> lock(mixer_lock);

  for (auto &voice : voices) {
    if (voice.active && voice.sound == sound)
      voice.active = false;
  }
}

void AudioCallback(float *buffer, int num_frames, int num_channels) {
  int total_samples = num_frames * num_channels;
  std::fill_n(buffer, total_samples, 0.0f);

  std::lock_guard<std::mutex> lock(mixer_lock);

  for (auto &voice : voices) {
    if (!voice.active || voice.sound == nullptr)
      continue;

    const std::vector<float> &samples = voice.sound->samples;

    for (int frame = 0; frame < num_frames; ++frame) {
      if (voice.cursor >= samples.size()) {
        if (voice.looping) {
          voice.cursor = 0;
        } else {
          voice.active = false;
          break;
        }
      }

      float sample = samples[voice.cursor++];

      for (int c = 0; c < num_channels; ++c) {
        buffer[frame * num_channels + c] += sample;
      }
    }
  }
}
