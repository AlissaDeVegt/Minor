# Minor

How to download and use:
1. Download the card folder, which will contain all the files needed except vulkan.
2. Download Vulkan and premake if not done so.
3. Change and update the premake5 file.
4. Generate the game project using the bat file.
5. Make a cpp file with a include "Card.h" and a class thats inherits from application
6. add outside of the new class the function:
   Card::Application* Card::CreateApplication() {
	return new Game();
}
where Game(); will be the name of the class.
7. override function start() and update() and start coding.
8. once done building program add the lib file of the engine from the bin file to the right bin locations. 
