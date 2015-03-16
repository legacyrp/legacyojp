Compiling and Debugging OJP

This section covers how to compile and/or debug OJP in Visual Studios 2008.
Compiling:

  1. Open the solution file (.sln).
> 2. Select Build->Configuration Manager from the menu bar.
> 3. On the Active solution configuration pulltab, select Debug(Final).
> 4. Click Close.
> 5. Select Build->Build Solution from the menu bar.
> 6. The resulting dlls will be in the /jke folders.

Debugging:

  1. Open the solution file (.sln).
> 2. Select Build->Configuration Manager from the menu bar.
> 3. On the Active solution configuration pulltab, select Debug.
> 4. Right click on your JK2game project in the Solution Explorer.
> 5. Click on Set as Startup Project.
> 6. Right click on your JK2game project again.
> 7. Select Properties.
> 8. Click on Debugging under Configuration Properties.
> 9. Set Command to point to the jamp.exe in your GameData folder wherever your copy of JKA is installed.
  1. . Add the following to the beginning of your Command Arguments "+set fs\_cdpath "<your JKA /GameData>" +set fs\_game "jke" +set fs\_dirbeforepak 1 +set sv\_pure 0 +set viewlog 1".  Where <your JKA /GameData> is the location of your /GameData folder of your JKA install.  Example:  +set fs\_cdpath "d:\games\Jedi Knight Academy\GameData" +set fs\_game jke +set fs\_dirbeforepak 1 +set sv\_pure 0 +set viewlog 1
  1. . Set Working Directory to "../..".
  1. . (Skip this step if you last compiled in the Debug solution configuration) Select Build->Rebuild Solution from the menu bar.
  1. . From the menu bar, select Debug->Start Debugging.