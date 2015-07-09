#include "sdl2-mixer.h"
#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "sdl2_rwops.h"


static struct RClass *class_Music    = NULL;
static struct RClass *class_Chunk    = NULL;


typedef struct mrb_sdl2_mixer_chunk_data_t {
  Mix_Chunk * chunk;
} mrb_sdl2_mixer_chunk_data_t;

static void
mrb_sdl2_mixer_chunk_data_free(mrb_state *mrb, void *p)
{
  mrb_sdl2_mixer_chunk_data_t *data =
    (mrb_sdl2_mixer_chunk_data_t*)p;
  if (NULL != data) {
    if (NULL != data->chunk) {
      mrb_free(mrb, data->chunk);
    }
    mrb_free(mrb, p);
  }
}


struct mrb_data_type const mrb_sdl2_mixer_chunk_data_type = {
  "Chunk", mrb_sdl2_mixer_chunk_data_free
};

Mix_Chunk *
mrb_sdl2_chunk_get_ptr(mrb_state *mrb, mrb_value value)
{
  mrb_sdl2_mixer_chunk_data_t *data;
  if (mrb_nil_p(value)) {
    return NULL;
  }

  data =
    (mrb_sdl2_mixer_chunk_data_t*)mrb_data_get_ptr(mrb, value, &mrb_sdl2_mixer_chunk_data_type);
  return data->chunk;
}

mrb_value
mrb_sdl2_chunk(mrb_state *mrb, Mix_Chunk *value)
{
  mrb_sdl2_mixer_chunk_data_t *data =
    (mrb_sdl2_mixer_chunk_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_mixer_chunk_data_t));
  if (NULL == data) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
  }
  data->chunk = value;
  return mrb_obj_value(Data_Wrap_Struct(mrb, class_Chunk, &mrb_sdl2_mixer_chunk_data_type, data));
}


static mrb_value
mrb_sdl2_mixer_chunk_initialize(mrb_state *mrb, mrb_value self)
{
  Mix_Chunk *chunk = NULL;
  mrb_sdl2_mixer_chunk_data_t *data =
    (mrb_sdl2_mixer_chunk_data_t*)DATA_PTR(self);

  if (NULL == data) {
    data = (mrb_sdl2_mixer_chunk_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_mixer_chunk_data_t));
    if (NULL == data) {
      mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
    }
    data->chunk = NULL;
  }

  if (1 == mrb->c->ci->argc) {
  	SDL_RWops *rwops;
    mrb_value rw;
    mrb_get_args(mrb, "o", &rw);
	rwops = mrb_sdl2_rwops_get_ptr(mrb, rw);
    chunk = Mix_LoadWAV_RW(rwops, 0);
  } else {
    mrb_free(mrb, data);
    mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments.");
  }
  if (NULL == chunk) {
    mrb_free(mrb, data);
    mruby_sdl2_raise_error(mrb);
  }

  data->chunk = chunk;

  DATA_PTR(self) = data;
  DATA_TYPE(self) = &mrb_sdl2_mixer_chunk_data_type;
  return self;
}

static mrb_value
mrb_sdl2_mixer_chunk_free(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_mixer_chunk_data_t *data =
    (mrb_sdl2_mixer_chunk_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_mixer_chunk_data_type);
  if (NULL != data->chunk) {
    Mix_FreeChunk(data->chunk);
    data->chunk = NULL;
  }
  return self;
}


typedef struct mrb_sdl2_mixer_music_data_t {
  Mix_Music * music;
} mrb_sdl2_mixer_music_data_t;

static void
mrb_sdl2_mixer_music_data_free(mrb_state *mrb, void *p)
{
  mrb_sdl2_mixer_music_data_t *data =
    (mrb_sdl2_mixer_music_data_t*)p;
  if (NULL != data) {
    if (NULL != data->music) {
      mrb_free(mrb, data->music);
    }
    mrb_free(mrb, p);
  }
}


struct mrb_data_type const mrb_sdl2_mixer_music_data_type = {
  "Music", mrb_sdl2_mixer_music_data_free
};

Mix_Music *
mrb_sdl2_music_get_ptr(mrb_state *mrb, mrb_value value)
{
  mrb_sdl2_mixer_music_data_t *data;
  if (mrb_nil_p(value)) {
    return NULL;
  }

  data =
    (mrb_sdl2_mixer_music_data_t*)mrb_data_get_ptr(mrb, value, &mrb_sdl2_mixer_music_data_type);
  return data->music;
}

mrb_value
mrb_sdl2_music(mrb_state *mrb, Mix_Music *value)
{
  mrb_sdl2_mixer_music_data_t *data =
    (mrb_sdl2_mixer_music_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_mixer_music_data_t));
  if (NULL == data) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
  }
  data->music = value;
  return mrb_obj_value(Data_Wrap_Struct(mrb, class_Music, &mrb_sdl2_mixer_music_data_type, data));
}


static mrb_value
mrb_sdl2_mixer_music_initialize(mrb_state *mrb, mrb_value self)
{
  Mix_Music *music = NULL;
  mrb_sdl2_mixer_music_data_t *data =
    (mrb_sdl2_mixer_music_data_t*)DATA_PTR(self);

  if (NULL == data) {
    data = (mrb_sdl2_mixer_music_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_mixer_music_data_t));
    if (NULL == data) {
      mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
    }
    data->music = NULL;
  }

  if (1 == mrb->c->ci->argc) {
    mrb_value obj;
    SDL_RWops *rwops;
    mrb_get_args(mrb, "o", &obj);
    if (mrb_obj_is_kind_of(mrb, obj, mrb_class_get_under(mrb, mrb->object_class, "String"))) {
      music = Mix_LoadMUS(RSTRING_PTR(obj));
    } else if (mrb_obj_is_kind_of(mrb, obj, mrb_class_get_under(mrb, mod_SDL2, "RWops"))) {
      rwops = mrb_sdl2_rwops_get_ptr(mrb, obj);
      music = Mix_LoadMUS_RW(rwops, 0);
    } else {
      mrb_free(mrb, data);
      mrb_raise(mrb, E_RUNTIME_ERROR, "unexpected type.");
    }
  } else if (2 == mrb->c->ci->argc) {
    mrb_value rw;
    mrb_int type;
    SDL_RWops *rwops;
    mrb_get_args(mrb, "oi", &rw, &type);
    rwops = mrb_sdl2_rwops_get_ptr(mrb, rw);  
    music = Mix_LoadMUSType_RW(rwops, (Mix_MusicType) type, 0);
  } else {
    mrb_free(mrb, data);
    mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments.");
  }
  if (NULL == music) {
    mrb_free(mrb, data);
    mruby_sdl2_raise_error(mrb);
  }

  data->music = music;

  DATA_PTR(self) = data;
  DATA_TYPE(self) = &mrb_sdl2_mixer_music_data_type;
  return self;
}

static mrb_value
mrb_sdl2_mixer_music_free(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_mixer_music_data_t *data =
    (mrb_sdl2_mixer_music_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_mixer_music_data_type);
  if (NULL != data->music) {
    Mix_FreeMusic(data->music);
    data->music = NULL;
  }
  return self;
}
static mrb_value
mrb_sdl2_mixer_open(mrb_state *mrb, mrb_value self)
{
	int result;
	mrb_int freq, format, channels, chunksize;
	mrb_get_args(mrb, "iiii", &freq, &format, &channels, &chunksize);

	result = Mix_OpenAudio((int) freq, (Uint16) format, (int) channels, (int) chunksize);
	return (result == -1) ? mrb_false_value() : mrb_true_value();
}

static mrb_value
mrb_sdl2_mixer_allocate_channels(mrb_state *mrb, mrb_value self)
{
	int result;
	mrb_int numchans;
	mrb_get_args(mrb, "iiii", &numchans);

	result = Mix_AllocateChannels((int) numchans);
	return mrb_fixnum_value(result);
}

static mrb_value
mrb_sdl2_mixer_query_spec(mrb_state *mrb, mrb_value self)
{
	mrb_value array;
	int frequency, channels;
	Uint16 format;

	array = mrb_ary_new_capa(mrb, 3);
	Mix_QuerySpec(&frequency, &format, &channels);
	mrb_ary_push(mrb, array, mrb_fixnum_value(frequency));
	mrb_ary_push(mrb, array, mrb_fixnum_value(format));
	mrb_ary_push(mrb, array, mrb_fixnum_value(channels));

	return array;
}	

static mrb_value
mrb_sdl2_mixer_GetNumChunkDecoders(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(Mix_GetNumChunkDecoders());
}

static mrb_value
mrb_sdl2_mixer_GetChunkDecoder(mrb_state *mrb, mrb_value self)
{
  mrb_int index;
  mrb_get_args(mrb, "i", &index);

  return mrb_str_new_cstr(mrb, Mix_GetChunkDecoder((int) index));
}

static mrb_value
mrb_sdl2_mixer_GetNumMusicDecoders(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(Mix_GetNumMusicDecoders());
}

static mrb_value
mrb_sdl2_mixer_GetMusicDecoder(mrb_state *mrb, mrb_value self)
{
  mrb_int index;
  mrb_get_args(mrb, "i", &index);

  return mrb_str_new_cstr(mrb, Mix_GetMusicDecoder((int) index));
}

static mrb_value
mrb_sdl2_mixer_music_get_type(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(Mix_GetMusicType(mrb_sdl2_music_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_mixer_set_panning(mrb_state *mrb, mrb_value self)
{
  mrb_int channel, left, right;
  mrb_get_args(mrb, "iii", &channel, &left, &right);
  return mrb_fixnum_value(Mix_SetPanning((int)channel, (Uint8)left, (Uint8)right));
}

static mrb_value
mrb_sdl2_mixer_set_position(mrb_state *mrb, mrb_value self)
{
  mrb_int channel, angle, distance;
  mrb_get_args(mrb, "iii", &channel, &angle, &distance);
  return mrb_fixnum_value(Mix_SetPosition((int)channel, (Sint16)angle, (Uint8)distance));
}

static mrb_value
mrb_sdl2_mixer_set_distance(mrb_state *mrb, mrb_value self)
{
  mrb_int channel, distance;
  mrb_get_args(mrb, "ii", &channel, &distance);
  return mrb_fixnum_value(Mix_SetDistance((int)channel, (Uint8)distance));
}

static mrb_value
mrb_sdl2_mixer_set_reverse_stereo(mrb_state *mrb, mrb_value self)
{
  mrb_int channel, flip;
  mrb_get_args(mrb, "ii", &channel, &flip);
  return mrb_fixnum_value(Mix_SetReverseStereo(channel, flip));
}

static mrb_value
mrb_sdl2_mixer_reverse_channel(mrb_state *mrb, mrb_value self)
{
  mrb_int num;
  mrb_get_args(mrb, "i", &num);
  return mrb_fixnum_value(Mix_ReserveChannels(num));
}

static mrb_value
mrb_sdl2_mixer_group_channel(mrb_state *mrb, mrb_value self)
{
  mrb_int from, tag;
  mrb_get_args(mrb, "ii", &from, &tag);
  return mrb_fixnum_value(Mix_GroupChannel(from, tag));
}

static mrb_value
mrb_sdl2_mixer_group_channels(mrb_state *mrb, mrb_value self)
{
  mrb_int from, to, tag;
  mrb_get_args(mrb, "iii", &from, &to, &tag);
  return mrb_fixnum_value(Mix_GroupChannels(from, to, tag));
}

static mrb_value
mrb_sdl2_mixer_group_count(mrb_state *mrb, mrb_value self)
{
  mrb_int tag;
  mrb_get_args(mrb, "i", &tag);
  return mrb_fixnum_value(Mix_GroupCount(tag));
}

static mrb_value
mrb_sdl2_mixer_group_oldest(mrb_state *mrb, mrb_value self)
{
  mrb_int tag;
  mrb_get_args(mrb, "i", &tag);
  return mrb_fixnum_value(Mix_GroupOldest(tag));
}

static mrb_value
mrb_sdl2_mixer_group_newer(mrb_state *mrb, mrb_value self)
{
  mrb_int tag;
  mrb_get_args(mrb, "i", &tag);
  return mrb_fixnum_value(Mix_GroupNewer(tag));
}

static mrb_value
mrb_sdl2_mixer_chunk_play(mrb_state *mrb, mrb_value self)
{
  Mix_Chunk *c;
  mrb_int channel, loops, ticks;
  int result;
  int argc = mrb_get_args(mrb, "ii|i", &channel, &loops, &ticks);
  c = mrb_sdl2_chunk_get_ptr(mrb, self);
  if (argc == 2) {
    result = Mix_PlayChannelTimed(channel, c, loops, -1);
  } else {
    result = Mix_PlayChannelTimed(channel, c, loops, ticks);
  }
  return mrb_fixnum_value(result);
}

static mrb_value
mrb_sdl2_mixer_music_play(mrb_state *mrb, mrb_value self)
{
  Mix_Music *c;
  mrb_int loops;
  mrb_get_args(mrb, "i", &loops);
  c = mrb_sdl2_music_get_ptr(mrb, self);
  return mrb_fixnum_value(Mix_PlayMusic(c, loops));
}

static mrb_value
mrb_sdl2_mixer_music_fade_in(mrb_state *mrb, mrb_value self)
{
  Mix_Music *c;
  mrb_int loops, ms;
  mrb_get_args(mrb, "ii", &loops, &ms);
  c = mrb_sdl2_music_get_ptr(mrb, self);
  return mrb_fixnum_value(Mix_FadeInMusic(c, loops, ms));
}

static mrb_value
mrb_sdl2_mixer_music_fade_in_pos(mrb_state *mrb, mrb_value self)
{
  Mix_Music *c;
  mrb_int loops, ms, pos;
  mrb_get_args(mrb, "iii", &loops, &ms, &pos);
  c = mrb_sdl2_music_get_ptr(mrb, self);
  return mrb_fixnum_value(Mix_FadeInMusic(c, loops, ms));
}

static mrb_value
mrb_sdl2_mixer_music_fade_in_timed(mrb_state *mrb, mrb_value self)
{
  Mix_Chunk *c;
  mrb_int channel, loops, ms, ticks;
  mrb_get_args(mrb, "iiii", &channel, &loops, &ms, &ticks);
  c = mrb_sdl2_chunk_get_ptr(mrb, self);
  return mrb_fixnum_value(Mix_FadeInChannelTimed(channel, c, loops, ms, ticks));
}

static mrb_value
mrb_sdl2_mixer_volume(mrb_state *mrb, mrb_value self)
{
  mrb_int channel, volume;
  mrb_get_args(mrb, "ii", &channel, &volume);
  return mrb_fixnum_value(Mix_Volume(channel, volume));
}

static mrb_value
mrb_sdl2_mixer_chunk_volume(mrb_state *mrb, mrb_value self)
{
  Mix_Chunk *c;
  mrb_int volume;
  mrb_get_args(mrb, "i", &volume);
  c = mrb_sdl2_chunk_get_ptr(mrb, self);
  return mrb_fixnum_value(Mix_VolumeChunk(c, volume));
}

static mrb_value
mrb_sdl2_mixer_music_volume(mrb_state *mrb, mrb_value self)
{
  mrb_int volume;
  mrb_get_args(mrb, "i", &volume);
  return mrb_fixnum_value(Mix_VolumeMusic(volume));
}

static mrb_value
mrb_sdl2_mixer_halt_channel(mrb_state *mrb, mrb_value self)
{
  mrb_int channel;
  mrb_get_args(mrb, "i", &channel);
  return mrb_fixnum_value(Mix_HaltChannel(channel));
}

static mrb_value
mrb_sdl2_mixer_halt_group(mrb_state *mrb, mrb_value self)
{
  mrb_int tag;
  mrb_get_args(mrb, "i", &tag);
  return mrb_fixnum_value(Mix_HaltGroup(tag));
}

static mrb_value
mrb_sdl2_mixer_halt_music(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(Mix_HaltMusic());
}

static mrb_value
mrb_sdl2_mixer_expire_channel(mrb_state *mrb, mrb_value self)
{
  mrb_int channel, ticks;
  mrb_get_args(mrb, "ii", &channel, &ticks);
  return mrb_fixnum_value(Mix_ExpireChannel(channel, ticks));
}

static mrb_value
mrb_sdl2_mixer_fade_out_channel(mrb_state *mrb, mrb_value self)
{
  mrb_int which, ms;
  mrb_get_args(mrb, "ii", &which, &ms);
  return mrb_fixnum_value(Mix_FadeOutChannel(which, ms));
}

static mrb_value
mrb_sdl2_mixer_fade_out_group(mrb_state *mrb, mrb_value self)
{
  mrb_int tag, ms;
  mrb_get_args(mrb, "ii", &tag, &ms);
  return mrb_fixnum_value(Mix_FadeOutGroup(tag, ms));
}

static mrb_value
mrb_sdl2_mixer_fade_out_music(mrb_state *mrb, mrb_value self)
{
  mrb_int ms;
  mrb_get_args(mrb, "i", &ms);
  return mrb_fixnum_value(Mix_FadeOutMusic(ms));
}

static mrb_value
mrb_sdl2_mixer_fading_music(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(Mix_FadingMusic());
}

static mrb_value
mrb_sdl2_mixer_fading_channel(mrb_state *mrb, mrb_value self)
{
  mrb_int which;
  mrb_get_args(mrb, "i", &which);
  return mrb_fixnum_value(Mix_FadingChannel(which));
}

static mrb_value
mrb_sdl2_mixer_pause_channel(mrb_state *mrb, mrb_value self)
{
  mrb_int which;
  mrb_get_args(mrb, "i", &which);
  Mix_Pause(which);
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_mixer_resume_channel(mrb_state *mrb, mrb_value self)
{
  mrb_int which;
  mrb_get_args(mrb, "i", &which);
  Mix_Resume(which);
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_mixer_paused_channel(mrb_state *mrb, mrb_value self)
{
  mrb_int which;
  mrb_get_args(mrb, "i", &which);
  return mrb_fixnum_value(Mix_Paused(which));
}

static mrb_value
mrb_sdl2_mixer_pause_music(mrb_state *mrb, mrb_value self)
{
  Mix_PauseMusic();
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_mixer_resume_music(mrb_state *mrb, mrb_value self)
{
  Mix_ResumeMusic();
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_mixer_rewind_music(mrb_state *mrb, mrb_value self)
{
  Mix_RewindMusic();
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_mixer_paused_music(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(Mix_PausedMusic());
}

static mrb_value
mrb_sdl2_mixer_set_music_position(mrb_state *mrb, mrb_value self)
{
  mrb_int pos;
  mrb_get_args(mrb, "i", &pos);
  return mrb_fixnum_value(Mix_SetMusicPosition(pos));
}

static mrb_value
mrb_sdl2_mixer_channel_playing(mrb_state *mrb, mrb_value self)
{
  mrb_int which;
  mrb_get_args(mrb, "i", &which);
  return (Mix_Playing(which) == 0) ? mrb_false_value() : mrb_true_value();
}

static mrb_value
mrb_sdl2_mixer_music_playing(mrb_state *mrb, mrb_value self)
{
  return (Mix_PlayingMusic() == 0) ? mrb_false_value() : mrb_true_value();
}

static mrb_value
mrb_sdl2_mixer_set_music_cmd(mrb_state *mrb, mrb_value self)
{
  mrb_value str;
  mrb_get_args(mrb, "S", &str);
  return mrb_fixnum_value(Mix_SetMusicCMD(RSTRING_PTR(str)));
}

static mrb_value
mrb_sdl2_mixer_set_sync_value(mrb_state *mrb, mrb_value self)
{
  mrb_int v;
  mrb_get_args(mrb, "i", &v);
  return mrb_fixnum_value(Mix_SetSynchroValue(v));
}

static mrb_value
mrb_sdl2_mixer_get_sync_value(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(Mix_GetSynchroValue());
}

static mrb_value
mrb_sdl2_mixer_set_sound_fonts(mrb_state *mrb, mrb_value self)
{
  mrb_value v;
  mrb_get_args(mrb, "S", &v);
  return mrb_fixnum_value(Mix_SetSoundFonts(RSTRING_PTR(v)));
}

static mrb_value
mrb_sdl2_mixer_get_sound_fonts(mrb_state *mrb, mrb_value self)
{
  return mrb_str_new_cstr(mrb, Mix_GetSoundFonts());
}

static mrb_value
mrb_sdl2_mixer_get_channel_chunk(mrb_state *mrb, mrb_value self)
{
  mrb_int v;
  mrb_get_args(mrb, "i", &v);
  return mrb_sdl2_chunk(mrb, Mix_GetChunk(v));
}

static mrb_value
mrb_sdl2_mixer_close_audio(mrb_state *mrb, mrb_value self)
{
  Mix_CloseAudio();
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_mixer_init(mrb_state *mrb, mrb_value self)
{
  mrb_int v;
  mrb_get_args(mrb, "i", &v);
  return mrb_fixnum_value(Mix_Init(v));
}

static mrb_value
mrb_sdl2_mixer_quit(mrb_state *mrb, mrb_value self)
{
  Mix_Quit();
  return mrb_nil_value();
}

void
mrb_mruby_sdl2_mixer_gem_init(mrb_state *mrb)
{
  int arena_size;
	struct RClass *mod_Mixer = mrb_define_module_under(mrb, mod_SDL2, "Mixer");
  class_Chunk = mrb_define_class_under(mrb, mod_Mixer, "Chunk", mrb->object_class);
  class_Music = mrb_define_class_under(mrb, mod_Mixer, "Music", mrb->object_class);

  MRB_SET_INSTANCE_TT(class_Chunk, MRB_TT_DATA);
  MRB_SET_INSTANCE_TT(class_Music, MRB_TT_DATA);
	
  mrb_define_module_function(mrb, mod_Mixer, "init",                  mrb_sdl2_mixer_init,                ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "quit",                  mrb_sdl2_mixer_quit,                ARGS_NONE());
	mrb_define_module_function(mrb, mod_Mixer, "open",                  mrb_sdl2_mixer_open,                ARGS_REQ(4));
	mrb_define_module_function(mrb, mod_Mixer, "allocateChannels",      mrb_sdl2_mixer_allocate_channels,   ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "querySpec",             mrb_sdl2_mixer_query_spec,          ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "get_num_chuck_decoder", mrb_sdl2_mixer_GetNumChunkDecoders, ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "get_chunk_decoder",     mrb_sdl2_mixer_GetChunkDecoder,     ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "get_num_music_decoder", mrb_sdl2_mixer_GetNumMusicDecoders, ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "get_music_decoder",     mrb_sdl2_mixer_GetMusicDecoder,     ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "set_panning",           mrb_sdl2_mixer_set_panning,         ARGS_REQ(3));
  mrb_define_module_function(mrb, mod_Mixer, "set_position",          mrb_sdl2_mixer_set_position,        ARGS_REQ(3));
  mrb_define_module_function(mrb, mod_Mixer, "set_distance",          mrb_sdl2_mixer_set_distance,        ARGS_REQ(2));
  mrb_define_module_function(mrb, mod_Mixer, "set_reverse_stereo",    mrb_sdl2_mixer_set_reverse_stereo,  ARGS_REQ(2));
  mrb_define_module_function(mrb, mod_Mixer, "reverse_channel",       mrb_sdl2_mixer_reverse_channel,     ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "group_channel",         mrb_sdl2_mixer_group_channel,       ARGS_REQ(2));
  mrb_define_module_function(mrb, mod_Mixer, "group_channels",        mrb_sdl2_mixer_group_channels,      ARGS_REQ(3));
  mrb_define_module_function(mrb, mod_Mixer, "group_count",           mrb_sdl2_mixer_group_count,         ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "group_oldest",          mrb_sdl2_mixer_group_oldest,        ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "group_newer",           mrb_sdl2_mixer_group_newer,         ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "volume",                mrb_sdl2_mixer_volume,              ARGS_REQ(2));
  mrb_define_module_function(mrb, mod_Mixer, "music_volume",          mrb_sdl2_mixer_music_volume,        ARGS_REQ(2));
  mrb_define_module_function(mrb, mod_Mixer, "halt_channel",          mrb_sdl2_mixer_halt_channel,        ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "halt_group",            mrb_sdl2_mixer_halt_group,          ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "halt_music",            mrb_sdl2_mixer_halt_music,          ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "expire_channel",        mrb_sdl2_mixer_expire_channel,      ARGS_REQ(2));
  mrb_define_module_function(mrb, mod_Mixer, "fade_out_channel",      mrb_sdl2_mixer_fade_out_channel,    ARGS_REQ(2));
  mrb_define_module_function(mrb, mod_Mixer, "fade_out_group",        mrb_sdl2_mixer_fade_out_group,      ARGS_REQ(2));
  mrb_define_module_function(mrb, mod_Mixer, "fade_out_music",        mrb_sdl2_mixer_fade_out_music,      ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "fading_music",          mrb_sdl2_mixer_fading_music,        ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "fading_channel",        mrb_sdl2_mixer_fading_channel,      ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "pause_channel",         mrb_sdl2_mixer_pause_channel,       ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "resume_channel",        mrb_sdl2_mixer_resume_channel,      ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "paused_channel",        mrb_sdl2_mixer_paused_channel,      ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "pause_music",           mrb_sdl2_mixer_pause_music,         ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "resume_music",          mrb_sdl2_mixer_resume_music,        ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "rewind_music",          mrb_sdl2_mixer_rewind_music,        ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "paused_music",          mrb_sdl2_mixer_paused_music,        ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "set_music_position",    mrb_sdl2_mixer_set_music_position,  ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "channel_playing?",      mrb_sdl2_mixer_channel_playing,     ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "music_playing?",        mrb_sdl2_mixer_music_playing,       ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "set_music_cmd",         mrb_sdl2_mixer_set_music_cmd,       ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "set_sync_value",        mrb_sdl2_mixer_set_sync_value,      ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "get_sync_value",        mrb_sdl2_mixer_get_sync_value,      ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "set_sound_fonts",       mrb_sdl2_mixer_set_sound_fonts,     ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "get_sound_fonts",       mrb_sdl2_mixer_get_sound_fonts,     ARGS_NONE());
  mrb_define_module_function(mrb, mod_Mixer, "get_channel_chunk",     mrb_sdl2_mixer_get_channel_chunk,   ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Mixer, "close_audio",           mrb_sdl2_mixer_close_audio,         ARGS_NONE());

	mrb_define_method(mrb, class_Chunk, "initialize",    mrb_sdl2_mixer_chunk_initialize,    ARGS_REQ(1));
	mrb_define_method(mrb, class_Chunk, "free",          mrb_sdl2_mixer_chunk_free,          ARGS_NONE());
  mrb_define_method(mrb, class_Chunk, "destroy",       mrb_sdl2_mixer_chunk_free,          ARGS_NONE());
  mrb_define_method(mrb, class_Chunk, "play",          mrb_sdl2_mixer_chunk_play,          ARGS_REQ(2) | ARGS_OPT(1));
  mrb_define_method(mrb, class_Chunk, "fade_in_timed", mrb_sdl2_mixer_music_fade_in_timed, ARGS_REQ(4));
  mrb_define_method(mrb, class_Chunk, "volume",        mrb_sdl2_mixer_chunk_volume,        ARGS_REQ(1));

  mrb_define_method(mrb, class_Music, "initialize",  mrb_sdl2_mixer_music_initialize,  ARGS_REQ(1) | ARGS_REQ(2));
  mrb_define_method(mrb, class_Music, "free",        mrb_sdl2_mixer_music_free,        ARGS_NONE());
  mrb_define_method(mrb, class_Music, "destroy",     mrb_sdl2_mixer_music_free,        ARGS_NONE());
  mrb_define_method(mrb, class_Music, "type",        mrb_sdl2_mixer_music_get_type,    ARGS_NONE());
  mrb_define_method(mrb, class_Music, "play",        mrb_sdl2_mixer_music_play,        ARGS_REQ(1));
  mrb_define_method(mrb, class_Music, "fade_in",     mrb_sdl2_mixer_music_fade_in,     ARGS_REQ(2));
  mrb_define_method(mrb, class_Music, "fade_in_pos", mrb_sdl2_mixer_music_fade_in_pos, ARGS_REQ(2));


  arena_size = mrb_gc_arena_save(mrb);
  mrb_define_const(mrb, mod_Mixer, "MIX_CHANNELS",          mrb_fixnum_value(MIX_CHANNELS));
  mrb_define_const(mrb, mod_Mixer, "MIX_DEFAULT_FREQUENCY", mrb_fixnum_value(MIX_DEFAULT_FREQUENCY));
  mrb_define_const(mrb, mod_Mixer, "MIX_DEFAULT_FORMAT",    mrb_fixnum_value(MIX_DEFAULT_FORMAT));
  mrb_define_const(mrb, mod_Mixer, "MIX_DEFAULT_CHANNELS",  mrb_fixnum_value(MIX_DEFAULT_CHANNELS));
  mrb_define_const(mrb, mod_Mixer, "MIX_MAX_VOLUME",        mrb_fixnum_value(MIX_MAX_VOLUME));
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mrb_define_const(mrb, mod_Mixer, "MIX_NO_FADING",  mrb_fixnum_value(MIX_NO_FADING));
  mrb_define_const(mrb, mod_Mixer, "MIX_FADING_OUT", mrb_fixnum_value(MIX_FADING_OUT));
  mrb_define_const(mrb, mod_Mixer, "MIX_FADING_IN",  mrb_fixnum_value(MIX_FADING_IN));
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mrb_define_const(mrb, mod_Mixer, "MUS_NONE",     mrb_fixnum_value(MUS_NONE));
  mrb_define_const(mrb, mod_Mixer, "MUS_CMD",      mrb_fixnum_value(MUS_CMD));
  mrb_define_const(mrb, mod_Mixer, "MUS_WAV",      mrb_fixnum_value(MUS_WAV));
  mrb_define_const(mrb, mod_Mixer, "MUS_MOD",      mrb_fixnum_value(MUS_MOD));
  mrb_define_const(mrb, mod_Mixer, "MUS_MID",      mrb_fixnum_value(MUS_MID));
  mrb_define_const(mrb, mod_Mixer, "MUS_OGG",      mrb_fixnum_value(MUS_OGG));
  mrb_define_const(mrb, mod_Mixer, "MUS_MP3",      mrb_fixnum_value(MUS_MP3));
  mrb_define_const(mrb, mod_Mixer, "MUS_MP3_MAD",  mrb_fixnum_value(MUS_MP3_MAD));
  mrb_define_const(mrb, mod_Mixer, "MUS_FLAC",     mrb_fixnum_value(MUS_FLAC));
  mrb_define_const(mrb, mod_Mixer, "MUS_MODPLUG",  mrb_fixnum_value(MUS_MODPLUG));
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mrb_define_const(mrb, mod_Mixer, "MIX_INIT_FLAC",       mrb_fixnum_value(MIX_INIT_FLAC));
  mrb_define_const(mrb, mod_Mixer, "MIX_INIT_MOD",        mrb_fixnum_value(MIX_INIT_MOD));
  mrb_define_const(mrb, mod_Mixer, "MIX_INIT_MODPLUG",    mrb_fixnum_value(MIX_INIT_MODPLUG));
  mrb_define_const(mrb, mod_Mixer, "MIX_INIT_MP3",        mrb_fixnum_value(MIX_INIT_MP3));
  mrb_define_const(mrb, mod_Mixer, "MIX_INIT_OGG",        mrb_fixnum_value(MIX_INIT_OGG));
  mrb_define_const(mrb, mod_Mixer, "MIX_INIT_FLUIDSYNTH", mrb_fixnum_value(MIX_INIT_FLUIDSYNTH));
  mrb_gc_arena_restore(mrb, arena_size);
}

void
mrb_mruby_sdl2_mixer_gem_final(mrb_state *mrb)
{
}