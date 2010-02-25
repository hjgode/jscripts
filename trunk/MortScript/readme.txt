Welcome to the MortScript project!

The project files are a bit old, and Microsoft makes it quite difficult to rename
them without any troubles, so there are some older names here, "MortStarter" never
made it public.

The workspace file is MortStarter.vcw, and is made for EVC4 (SP4 or 5).
To compile, you also need the PPC2003 and Smartphone2003 SDKs.
The PPC and PNA versions have to be compiled with the PPC2003 SDK, the Smartphone
version with the Smartphone SDK (there should be a dropdown in the toolbars, which
usually shows "PPC 2003" by default, maybe sometimes the outdated "Standard SDK").
Since EVC doesn't allow debugging on WM5 devices, Debug configurations might not
work or at least use some bad directories.

Please note you have to check out the MortTools to a parallel directory (e.g.
\Projects\MortTools and \Projects\MortScript). The current tools libraries are
checked in as binaries, too, but to be sure, recompiling them might not harm...

The workspace contains the following projects:
- Autorun: The source for autorun.exe
  Contains configurations for PPC ("Release"), PNA ("Release PNA"), and Smartphone
  ("Release SP")
- MortRunnerSP is the soruce for both Smartphone and PNA version, that's because
  both don't offer MFC and thus are more similar.
  The "Release SP" configuration is for Smartphones, the "Release PNA" configuration
  for PNAs.
- MortStarter contains the PPC source (mostly the same files as "MortRunnerSP", but
  e.g. with different dialog sources and resources).
  Please use only the "Release" configuration.
- SetupDll is for the setup.dll that can be used in own CABs.
  The PPC exe works fine on PNAs, too, so the configurations only differ by destination
  paths. For Smartphones, there's "Release SP"
- Types is a project from MortTools. Since MortScript makes excessive use of them,
  they're here for debugging and patches.

MortScript project folders:
- Application contains the base application classes, e.g. command line parameters,
  instance checks, and so on...
- Resource Files speaks for itself...
- Dialogs contains the dialog classes (yes, really! ;))
- Core contains the main interpreter (to parse the script) and the expression parser
  (for things like "a+5*SomeFunction()")
- Helpers contains some help functions, like for accessing INI files, the registry,
  searching the structure arrays used to find the Commands and Functions, ...
- Commands contains all commands. If you add a new command, be sure to add it to
  Commands.cpp - the MortScript command must be lowercase and at the alphabetical
  correct position! Also, all commands implementations should start with Cmd and
  must use the same parameters (see other implementations for examples).
- Functions contains the functions. Most is similar to commands, only the parameters
  and return values differ.