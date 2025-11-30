Place WAV audio files here for the game to use.

Expected file names:
- title.wav   -> Background music for the title menu (looping)
- level1.wav  -> Background music for level 1 (looping)
- level2.wav  -> Background music for level 2 (looping) // optional
- level3.wav  -> Background music for level 3 (looping) // optional
- level4.wav  -> Background music for level 4 (looping) // optional
- level5.wav  -> Background music for level 5 (looping) // optional
- level6.wav  -> Background music for level 6 (looping) // optional
- death.wav   -> Optional sound effect when player dies (one-shot)
- victory.wav -> Optional sound effect when player wins (one-shot)

Notes:
- Only WAV files are supported with the current implementation (PlaySound / winmm).
- To add MP3 or other formats you will need a different audio library like SDL2_mixer, SFML, or Media Foundation.
- Make sure your WAV files are properly encoded and not too large for memory.

Build instructions:
- This project uses MinGW. Update `make.bat` with your MinGW path if needed.
- The build command now links with winmm (-lwinmm) to play audio.

Example:
- Place your audio files under `assets/` with the expected names and run the `make.bat` to build and run the game.
