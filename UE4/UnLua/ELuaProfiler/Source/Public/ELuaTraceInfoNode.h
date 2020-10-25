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

struct ELUAPROFILER_API FELuaTraceInfoNode
{
	/* show name */
	FString Name = "anonymous";

	/* call time */
	int64 CallTime = 0;

	/* self time */
	int64 SelfTime = 0;

	/* total time */
	int64 TotalTime = 0;

	/* the total size of lua_State when this node invoke */
	int32 CallSize = 0;

	/* the total size of lua_State when this node return */
	//int32 ReturnSize;

	/* the size of this node alloc */
	int32 AllocSize = 0;

	/* the size of this node release */
	int32 GCSize = 0;

	/* the num of calls */
	int32 Count = 0;

	/* debug info event 
	 * LUA_HOOKCALL
	 * LUA_HOOKRET 
	 * LUA_HOOKTAILCALL
	 * LUA_HOOKLINE
	 * LUA_HOOKCOUNT
	 */
	int32 Event = -1;

	/* node id */
	FString ID;

	/* parent node */
	TSharedPtr<FELuaTraceInfoNode> Parent = nullptr;

	/* all child nodes */
	TArray<TSharedPtr<FELuaTraceInfoNode>> Children;

	/* id map to FELuaTraceInfoNode */
	TMap<FString, TSharedPtr<FELuaTraceInfoNode>> ChildIDMap;

	FELuaTraceInfoNode(TSharedPtr<FELuaTraceInfoNode> P, FString& ID, const char* Name, int32 Event)
	{
		ID = ID;
		Name = Name;
		Event = Event;
		Parent = P;
	}

	void AddChild(TSharedPtr<FELuaTraceInfoNode> Child)
	{
		Children.Add(Child);
		ChildIDMap.Add(Child->ID, Child);
	}

	void BeginInvoke()
	{
		CallTime = GetTimeNs();
		CallSize = GetStateMemB();
		Count += 1;
	}

	int32 EndInvoke()
	{
		TotalTime += GetTimeNs() - CallTime;

		int32 Increment = GetStateMemB() - CallSize;
		if (Increment > 0)
		{
			AllocSize += Increment;
		} 
		else
		{
			GCSize += Increment;
		}
		return Event;
	}

	TSharedPtr<FELuaTraceInfoNode> GetChild(const FString& ID)
	{
		if (ChildIDMap.Contains(ID))
		{
			return ChildIDMap[ID];
		}
		return nullptr;
	}

	void Empty()
	{
		Name.Empty();
		CallTime = 0.f;
		SelfTime = 0.f;
		TotalTime = 0.f;
		Count = 0;
		Event = 0;
		ID.Empty();
		Parent = nullptr;
		Children.Empty();
		ChildIDMap.Empty();
	}
};
