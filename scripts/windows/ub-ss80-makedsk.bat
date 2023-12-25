::==============================================================================
:: ub-ss80-makedsk - Creates a formatted Microbee SS80 CPC-EMU DSK file.
:: SJK 2010/07/17.
::
:: Usage: ub-ss80-makedsk filename
::
:: Note: options are used here that will overide the format type given in any
:: file extensions!  The verbose level is set to 0 to disable normal
:: reporting.  No format confirmation prompting!
::==============================================================================

if %1x==x goto usage
if NOT %2x==x goto usage

ubeedisk --disk=format --force --verbose=0 --format=ss80 --otype=dsk --of=%1
goto done

:usage
echo %0 - Creates a formatted Microbee SS80 CPC-EMU DSK file.
echo Usage: %0 output_file

:done
