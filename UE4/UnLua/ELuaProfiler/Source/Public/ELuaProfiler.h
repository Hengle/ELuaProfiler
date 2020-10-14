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

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "InputCoreTypes.h"
#include "ModuleManager.h"
#include "Commands.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Commands/Commands.h"

/** Declares a log category for this module. */
DECLARE_LOG_CATEGORY_EXTERN(LogELuaProfiler, Log, All);

#ifdef ENABLE_ELUAPROFILER
#endif

class FELuaProfilerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

    
	void PluginButtonClicked();
private:
	// fields
	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;
	bool tabOpened = false;
	TSharedPtr<class FUICommandList> PluginCommands;

	// functions
	void OnTabClosed(TSharedRef<SDockTab> tab);
    
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	bool Tick(float DeltaTime);
	void ClearCurProfiler();
	void AddMenuExtension(FMenuBuilder& Builder);
};
