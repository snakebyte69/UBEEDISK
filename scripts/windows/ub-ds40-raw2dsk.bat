::==============================================================================
:: ub-ds40-raw2dsk - Converts a Microbee DS40 raw file to a CPC-EMU DSK file.
:: SJK 2010/02/26.
::
:: Usage: ub-ds40-raw2dsk input_file output_file
::
:: Note: options are used here that will overide the format type given in any
:: file extensions!  Prompting for a disk description and the creation of an
:: INFO and ERR files are disabled. The verbose level is set to 0 to disable
:: normal reporting.
::==============================================================================

if %1x==x goto usage
if %2x==x goto usage

ubeedisk --info=off --verbose=0 --format=ds40 --itype=raw --if=%1 --otype=dsk --of=%2
goto done

:usage
echo %0 - Converts a Microbee DS40 raw file to a CPC-EMU DSK file.
echo Usage: %0 input_file output_file

:done
