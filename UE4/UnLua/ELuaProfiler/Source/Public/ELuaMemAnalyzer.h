// The MIT License (MIT)

// Copyright 2020 HankShu inkiu0@gmail.com
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "ELuaBase.h"
#include "ELuaMemInfoNode.h"
#include "ELuaMemSnapshot.h"

class ELUAPROFILER_API FELuaMemAnalyzer
{
public:
	FELuaMemAnalyzer();
	~FELuaMemAnalyzer();

public:
	static FELuaMemAnalyzer* GetInstance()
	{
		static FELuaMemAnalyzer Instance;
		return &Instance;
	}

	void Snapshot();

	void PopSnapshot();

	void ForceLuaGC();

	TSharedPtr<FELuaMemInfoNode> GetRoot(int32 Idx = -1);

private:

	const char* key_tostring(lua_State* L, int index, char* buffer, const size_t bufsize);
	void update_node_desc(const void* p, const char* desc);

	/* create snapshot */
	TSharedPtr<FELuaMemSnapshot> CreateSnapshot();

	void traverse_lightuserdata(lua_State* L, const char* desc, int level, const void* parent);
	void traverse_string(lua_State* L, const char* desc, int level, const void* parent);
	void traverse_table(lua_State* L, const char* desc, int level, const void* parent);
	void traverse_userdata(lua_State* L, const char* desc, int level, const void* parent);
	void traverse_function(lua_State* L, const char* desc, int level, const void* parent);
	void traverse_thread(lua_State* L, const char* desc, int level, const void* parent);
	void traverse_object(lua_State* L, const char* desc, int level, const void* parent);

private:
	TSharedPtr<FELuaMemSnapshot> CurSnapshot;
	TArray<TSharedPtr<FELuaMemSnapshot>> Snapshots;
};
