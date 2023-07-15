#pragma once
namespace ScaleformManager {
	void RegisterMenu();
	void LoadLocalizations();
	void RegisterScaleform(GFxMovieView* a_view, GFxValue* a_f4se_root);
	void OpenMenu();
	void CloseMenu();
	void ClearMenuSelection();
}