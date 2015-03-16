# Purpose #

Create a program that allows the user to update and launch the game while keeping it user friendly


# Flow #

Program Launch

> Main form
> Check if GameData Path was set
> > Yes it is set
> > > Parse file list from server
> > > Compare files and check if it needs update/download
> > > > Show Progress bar(s) and other information
> > > > > Yes file needs to be downloaded or updated
> > > > > > Then Download/Update this file to client and Move to the appropriate folder

> > > Launch the mod
> > > No it is not set
> > > > Prompt User to set the GameData path
> > > > Re-create form with new information