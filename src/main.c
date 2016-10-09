/*
	Copyright (c) 2016 ra, All rights reserved.
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#include <cauchy_256.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <assert.h>

int lua_cauchy_256_encode(lua_State *L) {
  int k = lua_tointeger(L, 1);
  int m = lua_tointeger(L, 2);

  size_t len;
  const char *message = lua_tolstring(L, 3, &len);

  int bytes = lua_tointeger(L, 4);

  char **data_ptrs = alloca(sizeof(char*) * k);
  char *recovery_blocks = alloca(bytes * m);


  int i;
  for(i=0;i<k;i++) {
    data_ptrs[i] = (char*)message + (i*bytes);
  }

  cauchy_256_encode(k, m, (const unsigned char**)data_ptrs, recovery_blocks, bytes);

  lua_newtable(L);

  i = 0;
  for(i=0;i<k;i++) {
    char *block = (char*)message + i*bytes;
    lua_pushlstring(L, block, bytes);
    lua_rawseti(L, -2, i+1);
  }

  for(i=0;i<m;i++) {
    char *block = recovery_blocks + i*bytes;
    lua_pushlstring(L, block, bytes);
    lua_rawseti(L, -2, i+k+1);
  }

  return 1;
}

struct CauchyDecoder {
  int k;
  int m;
  int bytes;
  int original_count;
  int recovery_count;
  char *data_start;
  char *data_ret_start;
  Block block_info[0];
  unsigned char data[0]; // for reference ; data is after the block_info list
  unsigned char data_ret[0]; // for reference ; data is after the block_info list
};

int lua_cauchy_decoder(lua_State *L) {
  int k = lua_tointeger(L, 1);
  int m = lua_tointeger(L, 2);
  int bytes = lua_tointeger(L, 3);

  struct CauchyDecoder *decoder = lua_newuserdata(L, sizeof(*decoder) + sizeof(Block)*k + (bytes*(k+k)));

  decoder->k = k;
  decoder->m = m;
  decoder->bytes = bytes;
  decoder->recovery_count = 0;
  decoder->original_count = 0;

  decoder->data_start = (char*)(decoder->block_info+k);
  decoder->data_ret_start = decoder->data_start+k*bytes;

  return 1;
}

int lua_cauchy_decoder_push(lua_State *L) {
  struct CauchyDecoder *decoder = lua_touserdata(L, 1);
  int row = lua_tointeger(L, 2);

  size_t len;
  const char *new_data = lua_tolstring(L, 3, &len);

  int k = decoder->k;
  int m = decoder->m;
  int bytes = decoder->bytes;

  int insertion_point;

  if (row < k) {
    insertion_point = decoder->original_count++;
    memcpy(decoder->data_ret_start+row*bytes, new_data, bytes);
  } else {
    insertion_point = k - (++decoder->recovery_count);
  }

  unsigned char *dest = (unsigned char*)decoder->data_start + insertion_point*bytes;
  memcpy(dest, new_data, bytes);

  Block *block = &decoder->block_info[insertion_point];
  block->data = dest;
  block->row = row;

  if (decoder->original_count + decoder->recovery_count < k) {
    lua_pushnil(L);
    return 1;
  }

  if (decoder->original_count == k) {
    lua_pushlstring(L, decoder->data_ret_start, bytes*k);
    return 1;
  }

  if (cauchy_256_decode(k, m, decoder->block_info, bytes)) {

    assert(k + m <= 256);
    assert(decoder->block_info != 0);
    assert(bytes % 8 == 0);

    lua_pushnil(L);
    return 1;
  }

  int i, e;

  block = &decoder->block_info[k - decoder->recovery_count];
  for (i = 0, e = decoder->recovery_count; i < e; ++i, ++block) {
    memcpy(decoder->data_ret_start+block->row*bytes, block->data, bytes);
  }

  lua_pushlstring(L, decoder->data_ret_start, bytes*k);
  return 1;
}

int luaopen_longhair(lua_State *L) {
  cauchy_256_init();

  lua_newtable(L);
  lua_pushcfunction(L, lua_cauchy_256_encode);
  lua_setfield(L, -2 ,"cauchy_256_encode");

  lua_pushcfunction(L, lua_cauchy_decoder);
  lua_setfield(L, -2 ,"cauchy_decoder");

  lua_pushcfunction(L, lua_cauchy_decoder_push);
  lua_setfield(L, -2 ,"cauchy_decoder_push");

  return 1;
}
