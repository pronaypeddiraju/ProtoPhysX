//-----------------------------------------------------------------------------------------------
// EngineBuildPreferences.hpp
//
// Defines build preferences that the Engine should use when building for this particular game.
//
// Note that this file is an exception to the rule "engine code shall not know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//

//#define ENGINE_DISABLE_AUDIO	// (If uncommented) Disables AudioSystem code and fmod linkage.

