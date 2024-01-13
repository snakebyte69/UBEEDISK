********************************************************************************
# uBeeDisk Version 4.01 running on MacOS #
- uBee : Original Code v4.00 Copyright (C) 2007-2017
- fathertedcrilly : The tricky stuff
- Snake : The dumb stuff
********************************************************************************
                                    uBeeDisk 4.01

         A tool for converting disks/images from one to another with
       optional auto detection options for Microbee, DOS, Applix disks.

                          Copyright (C) 2008-2017 uBee

For contact details please see the 'Contact' section at the end of this file.
********************************************************************************

For more information and support join our Social Media
# Social Media # 
- Microbee Programmers Facebook https://www.facebook.com/groups/microbeeprogrammers
- Microbee Programmers Discord https://discord.gg/hSsTfsA6gy

Distribution License
====================
See license.txt in this distribution.

Overview
========
This program copies disks and images from one to another, formats disks,
speed test drives, clean drive heads and is able to scan a disk to help
determine the physical layout.

The program's original primary development goal was to archive Microbee
floppy disks to images and to write these back to floppy, in recent times
many other computer formats have been added so it is now a general purpose
floppy read/write tool for MFM and FM disks.

Other disk formats can be added to a ubeedisk.ini file.  The intended
audience is mainly for users of computer emulators and for archival
purposes.

As the program makes use of the 'LibDsk' library there are many options for
image types.  The program provides some data recovery methods along with
'info' files for each disk image file created.  An 'info' file contains
information about the disk image, a status map of all sectors read from the
disk and an MD5 stamp of the associated disk image.

The program makes use of the 'LibDsk' library.  It can be found here:
http://www.seasip.demon.co.uk/Unix/LibDsk/

This README file is updated for each new release and may contain additional
examples and corrections.

IMPORTANT: It should be pointed out that once a floppy disk image has been
created it is never written to again if it is for archival purposes.  This
means DO NOT use the disk image under an emulator or by any other tools that
may write to the disk image.  Make a working copy(s) of the disk image and
only access that.  It is also important to archive the '.info' files that
UbeeDisk creates as it records any errors in sectors.

To access files on disk images see the section "Accessing files".

Platforms
---------
The source should be able to be built and run under Linux, and Windows 2000
and up platforms.  Windows 95/98/Me systems are not supported.  Other Unix
systems may work but depends on what host floppy driver code is available in
the LibDsk library.

Overall features
----------------
* Supports the remote 'Floppyio' floppy interface board.
* Copying uses track buffering on reads with configurable skew.
* Very easy to use with the built in auto detection.
* Can be run from USB Flash drives under a Linux 'Live CD'
* Copy Microbee and Honeysoft disks.
* Scan disk tracks for sector ID header information.
* Format disks using the option custom GAP and SYNC lengths.
* Access to remote floppy disks on your Microbee.
* Speed testing of drive spindle.
* Cleaning mode using a special disk or by manual methods.
* Based on the 'LibDsk' library providing many disk types and formats.
* Configurable retry counts.
* Unattended or operator interactive copy mode.
* Copy process mode may be paused to allow the drive heads to be cleaned
  and/or switching error handling modes.
* Prompt operator for corrective action on read errors when using
  interactive mode.
* Creation of information files with a retry count for each sector and
  other information used to create the image.
* Option to set level of logging for disk read errors.
* Auto drive type sensing.

Floppy hardware
===============
Finding a PC that has floppy support is becoming more and more difficult as
it's old technology.

Finding 3.5" HD drives is not too difficult.  5.25" 80T HD are less common
and 5.25" 40T DD are very difficult to locate but 5.25" 80T HD or DD drives
can normally be used in their place.  8" drives are around but expect to pay
a high price.

If you still have some Floppy drives hang on to them!

Later PC BIOS's may only support one floppy drive or floppy support may not
even be supported. Eventually the 34W sockets and FDC hardware is likely to
be removed at some stage if not already.  Even if hardware support is
provided the Floppy controller IC and BIOS code may not work on some disks
due to Index hole problems.  There are work arounds for it though.

If you have any old PC motherboards and FDC controllers hang on to them.

Off the shelf USB 3.5" Floppy drives allow DOS disks to be accessed but
these won't work with some disk formats such as Microbee.

In light of all these issues/problems the author of uBeeDisk developed a USB
floppy interface board to interact with 3.5", 5.25" and 8" SD/DD/HD floppy
drives using MFM or FM recording modes.  The name of this project is
"Floppyio".

https://discord.gg/tmYuvEtMH6

Archived Microbee disk images
=============================
The Microbee Software Preservation Project (MSPP) would be interested to
have any Microbee disk images produced by this program or by any other
means.  The MSPP is a community of well organised Microbee users trying to
preserve software for the Microbee and other Australian microcomputers from
around the 80's era.  The MSPP is also able to image Microbee or other disks
provided by the community.

https://discord.gg/tmYuvEtMH6

IMPORTANT INFORMATION
=====================
Differences in systems, drivers, floppy drives, FD Controllers exist.  How
well the imaging process works can depend on these and many other factors.

It is important to realise that an unsuccessful read on one set-up may work
on another and even if the disk can't be read without error does not mean
the disk is bad.  There are other alternative methods to recover data from
disks if this program can not.

Using the '--log=2' option will log all read errors in the 'err' file that
occur during the imaging process and can be used as a diagnostic tool.  Most
errors will be recovered from without even the operator even being aware of
it.  If the error can not be recovered from the operator will be prompted
for the next action to be taken if using the interactive mode or will be
handled automatically when the unattended mode is in use.

Please keep in mind that logged errors at the end of the file DOES NOT mean
a disk is bad, a successful read can occur on a retry and the error
information provided is really just a guide to the condition of the disk
and/or hardware.  Always refer to the 'Sector errors' and the 'SECTOR STATUS
MAP' to determine how much of the disk is in error.

If problems are found then the most reliable way to read disks is from a
utility run on the native machine where the disk would normally be used and
using uBeeDisk over a remote connection.  In the case of the Microbee use
FDS.COM.  Even disks that appear to be quite scuffed (not deeply scratched)
can be read error free.

Old disks
---------
Old disks will make the drive heads become dirty very quickly especially
when retries are carried out as this leaves deposits on the heads and can be
quite difficult to remove, the amount of dirt on the heads may not be easily
visible but even a small amount can stop the imaging process from reading
the disks.

I have experienced issues when reading single sided disks that afterwards
the drives can't read the other disk side, I have experienced this problem
more than once and so far has been fixed by cleaning the heads.  Cleaning
the heads needs to be done very carefully, at least one of the heads is
spring loaded and can be easily broken off from it's support if not
careful.

A disk that makes a squealing sound can often be a sign that the disk
surface is coming away from the heads, it may also be noise coming from the
disk jacket or the disk's center drive hole, it is important to STOP
immediately if these sounds are heard and find the cause and remedy before
continuing.

Any deposits on the heads can damage a good disk that follows.  Always clean
the drive heads before continuing.

5.25" and 8" disks can be checked for the surface coming away by lightly
scraping the disk surface next to the disk's center drive hole, never test
the surface that comes in contact with the heads.  The amount that comes
away can be a good indication of the condition of the disk.

Mouldy/dirty disks
------------------
Old disks may also have surface mould that needs to be removed, this is
mainly intended for 5.25" or 8" disks.  The process I use is as follows:

1. Use a preferably new or good used floppy and cut of the top of the disk
   jacket being careful not to cut into the disk media.  Come down about
   0.5mm at most.  Before doing this make sure the inside disk media is
   sitting at the bottom of the jacket to avoid cutting it.  Remove the disk
   media from the jacket and discard or put aside.  Label the disk as a
   "Temporary replacement jacket"

2. With the disk to be cleaned first make note of the actual disk contained
   in the jacket as it will need to go back into a new jacket afterwards in
   the correct orientation.  The center of the disk usually has some
   reinforcing as an indication.

   Use the steps in (1) to open up the jacket. Except don't discard the
   media inside it! but put the jacket aside as it won't be used again.

3. Use some mild dish washing liquid in lukewarm water (NOT HOT). Carefully
   wash both sides of the disk surface using a clean light wash cloth while
   avoiding flexing the disk.

4. Rinse the disk off with lukewarm water (NOT HOT).  Then use some paper
   towell and pat dry the disk.  Use a layer or two under the disk.

5. When the disk media is completely dry it can be inserted into the new
   jacket in the correct orientation (don't use the old jacket).  It must go
   back the correct way or it won't read.

Buckled disks
-------------
Disks that that have become buckled due to heat exposure can be recovered! 

Simply use the same process for the Mouldy disks (the washing part is
optional).  The inside media usually is not buckled, usually it's only the
jacket part that has buckled.

I have used this process with good success in the past and what may look
like an unrecoverable disk is certainly recoverable.

Using uBeeDisk
==============
On Windows machines you MUST have the Fdrawcmd.sys filter driver installed
if using the PC's floppy drives, without it floppy access will not work
correctly.

Fdrawcmd.sys is a floppy filter driver for Windows 2000/XP/2003/Vista.

This driver will be referred to as the 'ntwdm' floppy driver in this
documentation.

You can get it from here:

http://simonowen.com/fdrawcmd/

Windows examples are shown below.  On Unix systems drives 'A:' - 'Z:' will
be translated to '/dev/fd0' - '/dev/fd25'.

A generic 'fd' driver type name may be used to represent the host's floppy
driver.  On Windows this will be translated to 'ntwdm' and on Unices to
the 'floppy' driver.

Disk Detection
--------------
The software can detect most Microbee format disks from a bunch of Microbee
disks.  It can't be expected to detect Microbee formats from an assortment
of CP/M and/or other disks.  CP/M disks don't have a consistent layout
format from one to another to make this possible in a reliable manner.

Disk detection is also supported for other disk formats such as Appplix and
DOS disks.  See the --detect option.  The Applix and DOS disks are much
easier to identify as they contain known information in the boot sectors.

Disk copying
------------
The default process is to copy disks.  This will use a known disk format or
options to set each parameter.  The copy process does not blindly try to
copy disks as this would mean that errors on disks would not be able to
distinguished between deliberate (copy protection) or damaged disks.  A disk
dump process may be added at a later stage for this method but should be
avoided if a standard copy process can be used.

Driver types
------------
The '--itype' and '--otype' options allow the driver to be set for input and
output.

Specifying these won't normally be necessary.  The input and output driver
types can be determined by uBeeDisk from the input and output file names
specified.

Where a floppy device is specified for the file name the appropriate floppy
driver will be used.  On Windows drives 'X:' will use the 'ntwdm' driver and
on Unices drives 'X:' and '/dev/fd' drives will use the 'floppy' driver.

The driver used for disk images will use one of the following if the 'ext'
part of the file name is recognised and must use lower case as shown:

ext     driver
---     ------
.dsk    dsk
.edsk   edsk
.imd    imd
.qm     qm
.tel    tele
.cfi    cfi  
.nw     nanowasp  
.raw    raw
.ss40   raw
.ss80   raw
.ds40   raw
.ds80   raw
.ds82   raw
.ds84   raw
.ds8x   raw

It is recommended that all images created use the CPCEMU DSK format.

PAUSE MENU
----------
The PAUSE menu may be activated by pressing a key while running any uBeeDisk
process.  The options provided will depend on the process being run.  One of
the following prompts will be appear:

[R]esume [A]bort [C]lean [M]ode set to interactive/unattended ?:
[R]esume [A]bort [C]lean ?:
[R]esume [A]bort ?:

The operator selects one of the above options and the process continues as
follows:

[R]: Resume normal operation.
[A]: Abort the current process.
[C]: Clean the drive heads using a special cleaning disk.
[M]: Toggle the error handling mode between Interactive and Unattended for
     future error handling.

CREATE A DISK IMAGE
-------------------
As the most common and important use of this program will be for making disk
images of Microbee disks the program has been targeted to make the command
line parameters as simple as possible.

In older releases of uBeeDisk the --detect=mbee option was the default
setting but now if auto detection is required it must be specified.  If you
know the format of the disk it may be better to use --format option instead
rather than testing the disk image everytime.

To make a disk image using Microbee auto detection from a floppy in drive A:
on Windows or /dev/fd0 on Unices:

ubeedisk --detect=mbee --of=myimage.dsk

To make a disk image using a format option from a floppy in drive A: on
Windows or /dev/fd0 on Unices:

ubeedisk --format=ds40 --of=myimage.dsk

The above example uses the 'ds40' format but could be any other format. 
Disk formats can also be defined as macros in 'ubeedisk.ini' in which case
the following can be used.  Change 'format' to the required macro name.

ubeedisk format --of=myimage.dsk

The '--of' option is the name of the image to be created and this example
uses the '.dsk' extension which will cause the CPCEMU DSK driver to be used. 

Output file names can be simple like the above example but substitution
characters are also supported (See "File name substitution").

To make an image from another floppy drive will require using another
option.  To make a disk image from floppy drive B: instead:

ubeedisk --if=B: --of=myimage.dsk

Accessing disks on a Microbee computer is also possible by running a
Microbee floppy disk server application that serves disk data over a serial
port.  See 'Access to remote disks on your Microbee' further on.

When creating images you will be prompted for a description of the disk
contents before the imaging process starts unless turned off with a
'--entdesc=off' option or --diskdesc or --diskdescf options are used.  The
last 2 options provide an alternative way to pass the information to the
program and are mainly intended to be used by a GUI program.

When creating disk images the '--count=n' option can be added to cause the
imaging process to loop and automatically create file names for each image. 
See the '--count' option under section 'Command Line Options'.

See the 'COPY PROCESS AND RETRIES' and 'THE 3 RETRY LEVELS' section for an
explanation of how the error recovery works when copying if more detailed
information is required.  If running the program in Interactive mode
(default) then the error recovery options will need to be understood but in
the unattended mode this can be ignored and just let the program handle
this.

CREATE A FLOPPY FROM AN IMAGE
-----------------------------
To make a floppy from a Microbee DS80 DSK image you can use the '--format'
option as follows:

ubeedisk --format=ds80 --if=myimage.dsk --of=A:

or use a '--detect=mbee3' option:

ubeedisk --detect=mbee3 --if=myimage.dsk --of=A:

Note: 'mbee' can only be used when detecting input from floppy drives.  The
--detect option won't work on 'raw' images types, you will need to use the
--format option for 'raw' types.

ACCESS TO REMOTE DISKS ON YOUR MICROBEE
---------------------------------------
The 'remote' disk type should allow reading/writing sectors over a serial
port to any floppy disk based Microbee Z80 computer.

The FDS.COM program, sources and documentation containing some examples can
be found here under 'Utilities':

http://www.microbee-mspp.org.au/repository

SCANNING A DISK/IMAGE
---------------------
Scanning a disk will show the sector headers for each track.  This can be
used to determine the structure of a disk:

Since version 1.3.0 this process will use auto detection of the data rate
(250, 300, 500 and 1000 kHz) and encoding mode (mfm and fm) for each track
that is scanned and report this along with the other data.  The common modes
are tried first to speed up the detection process.

The --scanhead=s[,f] option may be used with the scanning process to set the
head scanning range.  If this option is not used then heads 0-1 are scanned.

To scan head 0 only:
--scanhead=0

To scan head 1 only:
--scanhead=1

To scan heads from 0-1:
--scanhead=0,1

This example will scan the first 10 cylinders.  You should provide a
finishing cylinder or the drive may attempt to step in too far:

ubeedisk --disk=scan --if=a: --finish=9

The next example will scan a sub-section of a disk using the --start,
--finish and --scanhead options:

ubeedisk --disk=scan --if=a: --start=10 --finish=11 --scanhead=0

Scanning disks can be deceptive,  you may see sectors on side 1 (as opposed
to side 0) that may lead you to think the disk has a double sided layout. 
These may just be the result of left overs or unused portions of the disk. 
A good example of this would be a 720K IBM factory formatted disk then
formatted to use Microbee SS80 format.  In this case one will observe
sectors 1-10 on side 0 and sectors 1-9 on side 1.  The disk may also not
have used all the tracks so left over tracks could also be evident.

If the disk is truly single side then you will only see side 0 sector
headers if formatted.  True single sided disks have no magnetic surface on
side 1 but disks can also appear to be a true singled sided if never
formatted on side 1 (i.e. contains random noise).  A disk that has never
been formatted will show no output during the scan.

Here is an example of the output from this command using the following
command line and a 720K 3.5" MSDOS disk in drive A:

ubeedisk --disk=scan --if=a: --finish=0

Cylinder:   0  Head:  0  Sectors:  9  Data rate kHz:  300  Encoding: mfm
------------------------------------------------------------------------
Track:   0  Side:  0  Sector:   1  Bytes/sector:  512
Track:   0  Side:  0  Sector:   2  Bytes/sector:  512
Track:   0  Side:  0  Sector:   3  Bytes/sector:  512
Track:   0  Side:  0  Sector:   4  Bytes/sector:  512
Track:   0  Side:  0  Sector:   5  Bytes/sector:  512
Track:   0  Side:  0  Sector:   6  Bytes/sector:  512
Track:   0  Side:  0  Sector:   7  Bytes/sector:  512
Track:   0  Side:  0  Sector:   8  Bytes/sector:  512
Track:   0  Side:  0  Sector:   9  Bytes/sector:  512

Cylinder:   0  Head:  1  Sectors:  9  Data rate kHz:  300  Encoding: mfm
------------------------------------------------------------------------
Track:   0  Side:  1  Sector:   1  Bytes/sector:  512
Track:   0  Side:  1  Sector:   2  Bytes/sector:  512
Track:   0  Side:  1  Sector:   3  Bytes/sector:  512
Track:   0  Side:  1  Sector:   4  Bytes/sector:  512
Track:   0  Side:  1  Sector:   5  Bytes/sector:  512
Track:   0  Side:  1  Sector:   6  Bytes/sector:  512
Track:   0  Side:  1  Sector:   7  Bytes/sector:  512
Track:   0  Side:  1  Sector:   8  Bytes/sector:  512
Track:   0  Side:  1  Sector:   9  Bytes/sector:  512

FORMATTING A DISK/IMAGE
-----------------------
This will format a disk/image to the format type requested.  As a safety
precaution you will be asked to enter 'format' before formatting will
proceed unless a '--force' option is also specified:

This example will format the disk in drive A: to Microbee DS80 format:

ubeedisk --disk=format --format=ds80 --of=A:

See the 'ubeedisk.ini.sample' file for some examples of Microbee formatting
entries.

STUBBORN DISKS
--------------
I have had some 5.25" 80 track disks that will not take a new format.  This
includes a copy process as it will normally also format the tracks first
unless told otherwise.

After attempting to format a disk scan would show the old data.

I'm not sure what actually causes this to occur but possibly the following
theories may apply:

* Possibly inconsistent track positioning from one drive to another and the
  strongest field is picked up?

* The new format head flux on the drive used is weaker than what was used by
  another drive so can't cut through the old data?

* The FDC refused to actually write the data for some reason?

I have found that a stack of 10 x 7mm diameter rare earth magnets run all
over the disk jacket (end on) on both sides fixes this problem.  Place a
piece of clean paper of the head slot in the jacket to avoid direct contact
with the media.

How powerful the magnets need to be I don't know. I just used what I had on
hand and didn't do experiments to find what other magnets may work or the
best method to apply it.

This process may also be helpful when disks have been formatted on 40 track
drives previously and an 80 track format is now required.

REPORTING THE DISK GEOMETRY
---------------------------
To report the geometry of a known floppy disk format in drive A: use the
following command:

ubeedisk --disk=info --if=a:

This is the output of the command:

Detecting 'mbee' disk format...
hs350: Microbee Honeysoft 3.5"

       Sidedness: 0 (alt)
       Cylinders: 40 (may vary)
           Heads: 1
   Sectors/track: 10x512 and 5x1024
     Sector base: 1
     Sector size: 512 and 1024
 Input data rate: 2 (sd)
Output data rate: 2 (sd)
         R/W gap: 12
      Format gap: 23
         FM mode: 0 (no)
   No multi mode: 1 (yes)
     No skip del: 1 (yes)
       iside1as0: 0
       secbase1s: 1
       secbase2c: -1
       secbase2s: 1
       Read skew: 2
Read skew offset: 1
   special flags: 1

TEST/ADJUST DRIVE SPINDLE SPEED
-------------------------------
This allows the speed of the spindle motor to be checked and adjusted. As
it's not possible to access the Index hole signal directly via the LibDsk
library the speed detection uses sectors found on an existing disk.  The
minimum requirement is that the disk must be in a density that the drive and
controller is able to understand and that cylinder 0 and head 0 contains at
least one valid sector ID.

The --start option may be used to specify another cylinder number.

The track being used (T0, S0) must not have the same sector ID repeated or
the readings will be incorrect.  Just about any disk can be used that does
not contain a strange format.  Don't use disks you are trying to archive as it
will cause unnecessary wear that can be avoided, an old MSDOS disk should
suffice and should be freshly formatted to ensure the layout is standard.

The speed test command automatically checks each density format in turn
until the correct one is found.

Setting the drive speed is normally achieved by adjusting a trim-pot resistor
in the motor control section of the drive.  Care should be taken as there
may be more than one variable resistor in the drive and the correct one
needs to be located.  If unsure mark the current position first (if not
a multi-turn trim-pot).

AC drives motors are not normally adjustable.

If the speed of the drive is out by a long way then it may not be able to
read the disk at all so might require adjusting the speed until some output
is seen.

Use the following command to speed test a drive:

ubeedisk --disk=speed --if=a:

The command will run for a maximum of about 1 minute, this is done to help
prevent excessive wear on the disk if forgotten about.

Here is some output from a 3.5" disk using the above command:

Cylinder:   0  Head:  0  Sectors: 18  Data rate kHz:  500  Encoding: mfm
Disk rotations for results: 5  Sector number sampled: 9

Press any key to exit (exits after 60 iterations)...

Iterations:  1  Time (ms): 1001  RPM: 300
Iterations:  2  Time (ms): 1001  RPM: 300
Iterations:  3  Time (ms): 1001  RPM: 300
Iterations:  4  Time (ms): 1002  RPM: 299
Iterations:  5  Time (ms): 1001  RPM: 300
Iterations:  6  Time (ms): 1001  RPM: 300
Iterations:  7  Time (ms): 1002  RPM: 299
Iterations:  8  Time (ms): 1001  RPM: 300
Iterations:  9  Time (ms): 1002  RPM: 299
Iterations: 10  Time (ms): 1001  RPM: 300

The accuracy of the values reported will be partly based on the resolution
of system clock and any other processes happening in the background.

Using the built in speed detection provided by uBeeDisk is useful if the
required knowledge and/or proper equipment is not available, the best way to
set the speed of the drive is to follow the manufacturers drive maintenance
instructions and acquire the necessary equipment.

CLEANING DRIVE HEADS
--------------------
A built in cleaning process designed to work with a special cleaning disk
may be called up from the command line or from the Pause or Interactive
menus.

To use from the command line place the cleaning disk into the target drive
and use the following command (drive A example):

ubeedisk --disk=clean --if=a:

The head cleaning process may also be called from the Pause and Interactive
error menus.  When using interactively follow the instructions given when
selecting the 'C' option.

The cleaning process uses the following sequence:

Start at track 0 and step in 5 tracks at a time and finish at track 35 where
the sequence is then reversed until the head reaches track 0.  The process
completes after 5 iterations.

Use --verbose=2 to see all the output for this command.

The following will be seen when run from the command line:

Press any key to exit (exits after 5 passes)...

Cleaning pass: 1/5
Cleaning pass: 2/5
Cleaning pass: 3/5
Cleaning pass: 4/5
Cleaning pass: 5/5

Currently this is the only cleaning algorithm employed.  Others may be added
later.

TESTING DSK IMAGES
------------------
Testing of DSK images including Microbee copy protected types is possible
using the Microbee emulator program 'uBee512' (v4.4.0 or later).

This is a fairly straight forward process if the disk has a system to boot
from:

ubee512 -a diskimage.dsk

Disks that do not have a system can also be tested but describing how to do
this is beyond the scope of this document.

KNOWN PROBLEMS
--------------
Previous known problems have been fixed.

COPY PROCESS AND RETRIES
========================

Overwriting files
-----------------
Before writing to the output file a check is first made to see if it already
exists, if so the user will be prompted for the action to be taken:

If a single file copy process then:
Destination file already exists. Overwrite "filename.ext"
[N]o [Y]es ?:

If the copy is part of a batch process then:
Destination file already exists. Overwrite "filename.ext"
[N]o [Y]es [A]ll overwrite [S]kip all ?:

The operator selects one of the above options and the process continues as
follows:

[N]: do not overwrite this one file.
[Y]: overwrite this one file.
[A]: overwrite this and all other existing files.
[S]: do not overwrite this or any other files.   

The user may force overwriting of files by specifying --force on the command
line.

Display updating
----------------
When a disk is being copied the progress will be updated on a single line
which includes the current cylinder, head, logical sector, and physical
sector numbers.  A Microbee DS40 disk will report:

Cylinder: 00/39 Head: 0/0 Sector: (log) 00x/009, (phy) 00x/010

Additionally the use of a --verbose=2 option will provide more information
reporting the physical sectors read and in what order.  This verbose level
will also cause a new line to be output for each cylinder copied.  A
Microbee DS40 disk will report the following sectors on the end of each
line:

2  4  6  8 10  3  5  7  9  1

Track read buffering
--------------------
From version 2.0.0 uBeeDisk employs track buffering when reading disks. 
Sectors are no longer read one sector at a time as this slowed down the
reading process.  The buffering of tracks has resulted in much faster
copying of disks when reading from floppy drives.  The less time spent on a
disk track the better as there will be less wear to the disk surface.

See additional section "Buffering and Cache read/write control".

Sector read skewing
-------------------
From version 2.1.0 all sector reads use disk skewing to avoid disk rotation
latency.  The default value is set to 2 but may be overridden with the
--skew option.  Some of the internal formats have other skew values defined. 
Some disks are best read with a skew of 1.  The default skew value of 2
works well with most disks that have a sequential sector arrangement on the
disk and should only require 2 revolutions of the disk to read all the
sectors on a single track regardless of the number of sectors.

If uBeeDisk has not completed processing a sector before the next one is
ready under the drive head it will be missed and one complete disk
revolution will be required before it can be accessed.  A 30 sector per
track disk may take up to 30 revolutions of the disk to read the complete
track if this is occurring but a skew of 2 will reduce it down to 2
revolutions instead.

A --skew-ofs=n option is provided that determines the first physical sector
number used in a track read process.  The first physical sector will be this
value added to the sector base value at the time of reading, if the sector
base number is 1 (--secbase=1) and n=0 the first sector read will be 1.  The
default value is 1.  For most disks this will mean that sector 2 is the
first sector to be read and was chosen as it may help with problems with the
Index hole and first sector issues on NEC uPD765 compatible ICs.

Error handling
--------------
The copy process provides two methods to handle errors during disk reads. 
The default method uses the Interactive mode of operation unless an
--unattended=on is specified or switched during the copy process.

If during a track read buffer process an error occurs then all the sectors
on the track will be read in using the non buffered sector by sector method
but will switch back to buffered mode after the track or head changes.

Pausing and switching modes
---------------------------
While copying the operator may press a keyboard key to pause the copy
process.  This will cause the Pause menu to be shown where the error mode
can be changed.

If the current error mode is set to Unattended mode then the following
Pause menu is seen:

Interactive mode
----------------
The interactive mode (default) will prompt the operator what to do when
'retries_l1' and 'retries_l2' expire.  The four options presented to the
operator are as follows:

INTERACTIVE ERROR MENU
----------------------
re[T]ry (no home) [R]etry (home) [I]gnore [S]kip [A]bort [C]lean [M]ode ?:

The operator selects one of the above options and the process continues as
follows:

[T]: Causes a retry with no track 0 seek.
[R]: Causes a retry but seeks to track 0 first.  It is common practice for
     bad sector reads to try to recover on a retry by first doing a seek to
     track 0 before returning and trying again.
[I]: Ignore the error and proceed.  The sector will be marked as bad in the
     'info' file.
[S]: Same as for [I], all further errors encountered for the current disk
     will also be ignored.
[A]: Abort the current disk copy process.
[C]: Clean the drive heads using a special cleaning disk.
[M]: Toggle the error handling mode between Interactive and Unattended for
     future error handling.

Unattended mode
---------------
In the unattended mode all disk read errors will be handled automatically by
using one of four options described in the interactive mode above.

This mode is designed to handle errors in such a way to avoid excessive disk
wear and will abort if too many retries are made.

The 'auto_retry_sector' variable counts the number of retries for a given
sector and is reset to 0 for each new sector.  When this reaches the maximum
allowed value the 'auto_retry_abort' variable counter is incremented and if
that reaches the maximum allowed value then the read process is aborted.

Each time an error occurs a check will be made to see if a retry using a
seek track 0 should be initiated first (only 1 seek to T0 per head/track is
allowed) or simply just retry reading the same sector without any track 0
seek.  Once the maximum number of track 0 seeks have been used up only
retries without seeking to T0 are issued. (This is to reduce wear on disks)

The 3 options to control the operation of this mode are as follows:

  --unattended-rab=n
  --unattended-rps=n
  --unattended-seek=n

See the 'Command Line Options' section for detailed information on each
option.  It is unlikely that any of these will need to be changed.

THE 3 RETRY LEVELS
==================
This is a technical explanation of how the 3 levels of retries works in the
code of uBeeDisk for those that may need to know.  These retry values can be
set with options as described below:

LibDsk (level 1)
----------------
A LibDsk retry level is set here to help make sure there is plenty of time
for a sector to be read successfully at the LibDsk level.  This should not
be set to a too low value as it can cause many soft errors and possibly
causes read problems on some disks (especially Microbee disks).  The value
can be changed using the --retry-l1 option if required.

When using 'remote' as the input type a default value of 1 will be used.  A
lower value for 'remote' is normally preferred as too many retries can occur
when disks are being read on native hardware which isn't necessary and can
cause excessive disk wear.

read_sector() (level 2)
-----------------------
The 2nd level of retries are performed in the read_sector() function which
can be changed using the --level-l2 option if required.  If the LibDsk
retries fail then this 2nd retry level comes into play.

read_sector_retry() (level 3)
-----------------------------
If level 2 fails to read a sector then this 3rd level comes into play.

If the Unattended mode is in use then some more retries (with and without T0
seeks), ignore or abort operations will be issued.  The exact way this works
depends on the count of previous errors and seek counts that have already
occurred.

Command Line Options
====================
Usage: ubeedisk [options]

  --append-error          This option is used to append the error log to the
                          'info' file instead of creating an 'err' file.

  --autorate=x            Test if drive is a 5.25" HD or 5.25" DD type if
                          x=on, if x=off no drive type detection will be used.
                          When enabled and the drive is a HD type and has less
                          than 70 tracks specified then double stepping will be
                          enabled and the data rate changed to DD. This only
                          affects 'floppy' and 'ntwdm' driver types and devices
                          that support the AUTORATE option. Default is on.
  --autorateip=x          Same as --autorate but sets input autorate only.
  --autorateop=x          Same as --autorate but sets output autorate only.

  --cacher=x              Enable/Disable Track caching for reads. This option
                          is only supported by some devices (i.e. Floppyio).
                          x=off to disable, x=on to enable. Default setting
                          depends on the device.
  --cachew=x              Enable/Disable Track caching for writes. This option
                          is only supported by some devices (i.e. Floppyio).
                          x=off to disable, x=on to enable. Default setting
                          depends on the device.

  --config=file           Allows an alternative configuration file to be used
                          or if file='none' then no configuration file will be
                          used.  This option if used must be the first option
                          declared on the command line.  The default file used
                          for configuration is 'ubeedisk.ini' and must be
                          located in the ubeedisk directory.
  --confv=v               Intended to be used in ubeedisk.ini scripts as a way
                          to check for old script versions and warn the user.
                          'v' is set to match the release version. Set this to
                          'none' to ignore the version check.

  --count=n               Count value is a number appended to the output file
                          name base. Using this option makes the copy process
                          prompt the user for disk changes. i.e. if the output
                          file name requested is disk.dsk and n=0 then the
                          name becomes disk000.dsk.  The number is incremented
                          by one for each iteration.

  --cylinders=n, -c       Set/override the number of cylinders.

  --datarate=x, -d        Set/override the input and output data rates. The
                          value is based on rates defined by LibDsk. The
                          allowed values are:

                          hd : data rate for 1.4Mb/1.2Mb disk in 3.5/5.25"
                               drive and eight inch drives.
                          dd : data rate for DD 5.25" disk in 1.2Mb drive.
                          sd : data rate for DD disk in 400Kb or 800Kb drive.
                          ed : data rate for 2.8Mb 3.5" disk in 3.5" drive.

  --datarateip=x          Same as --datarate but sets input datarate only.
  --datarateop=x          Same as --datarate but sets output datarate only.

  --description=str       Set/override the format description.

  --detect=x              Use disk format detection. x determines what disks
                          should be tested. Currently the supported detection
                          types are:

                          off    : disable detection.
                          applix : Applix 1616 disks.
                          fm     : FM disks (i.e. 8" disks).
                          dos    : DOS, Atari ST and MSX-DOS disks.
                          atari  : DOS Atari ST disks (forces).
                          msx    : DOS MSX-DOS disks (forces).
                          mbee   : Microbee disks.
                          mbee3  : Microbee 3.5" disks (forces).
                          mbee5  : Microbee 5.25" disks (forces).

  --disk=x                Select the required process. Default process is the
                          'copy' process if not specified. The processes
                          supported are:

                          copy   : copy standard layout disks.
                          dump   : not currently implemented.
                          info   : show disk format information only.
                          scan   : scan disk tracks for sector ID information.
                          format : format disk tracks.
                          speed  : check rotational speed of drive.
                          clean  : clean disk drive heads.

  --diskdesc=x            Pass a disk description. Repeat this option for as
                          many lines of text that are required.  Each line may
                          have up to 100 characters.  This option causes the
                          prompting for a description to be disabled.

  --diskdescf=fn          Pass the name of a file containing a disk
                          description.  Each line may have up to 100
                          characters. This option causes the prompting for a
                          description to be disabled.

  --echo=x                Echo a string to stdout. The string may also contain
                          an environment variable.
  --echoq=x               Same as --echo option but echoes a quoted version of
                          the environment variable if any spaces are found.

  --entdesc=x             Prompt operator to enter a disk description if x=on
                          or bypass if x=off when using the copy process.
                          Default is on.

  --erase=n               Set track erasure value to be used if formatting.
                          This option is only supported by some devices
                          (i.e. Floppyio). This forces a low level format of
                          the entire track to the same value.  The 'n' value
                          should not be a special controller value.

  --fdwa1=x               This option may be used to enable/disable some PC
                          only floppy disk access work-around #1 code. If
                          enabled the input drive is closed then reopened at
                          certain points in the program. Default is enabled.

  --fdwa2=x               This option may be used to enable/disable some floppy
                          disk access work-around #2 code. If enabled Track #0
                          is seeked followed by closing/reopening the input
                          drive at certain points in the program. Default is
                          enabled.

  --fdwa=x                Enables/disables both the fdwa1 and fdwa2 settings.

  --flags=x               This option has been deprecated.

  --finish=n              Set/override the finish cylinder/track for processes.
                          Default value is set to the highest value found by
                          --detect, --format or --cylinder options.  Tracks
                          may also be specified by using --sfmode.

  --fm=n                  Set/override the use of FM mode.  n=1 if the disk
                          uses an FM format or n=0 if MFM.
  --fmip=n                Same as --fm but sets input recording mode only.
  --fmop=n                Same as --fm but sets output recording mode only.

  --fmtgap=n              Set/override the LibDsk geometry format gap value.

  --force                 Use this option to stop confirmation of any requested
                          actions. i.e. when formatting a disk.

  --forceside=x           Forces formatting the output sector header ID side
                          value according to one of the following parameters:

                          off : use the source header values if possible (def).
                          on  : use the physical head value for side.
                          00  : use value 0 for head 0 and value 0 for head 1.
                          01  : use value 0 for head 0 and value 1 for head 1.
                          10  : use value 1 for head 0 and value 0 for head 1.
                          11  : use value 1 for head 0 and value 1 for head 1.

  --format=x, -f          Specify the disk format to use. The value is first
                          checked against built in values and if no matches
                          are found then against values in LibDsk and
                          the 'libdskrc' file. Use the --lformat option to see
                          a list of the available formats.

  --gapset=pe:v,[pe:v...] Set number of format GAP/SYNC bytes for format gaps.
                          See the README file for usage.

  --heads=n, -h           Set/override the number of heads.

  --help                  Send this help information to stdout.

  --idstep=x              Determine if the input drive should use double
                          stepping.  x=on to enable, x=off to disable.
                          Default is off. (auto detection may set to on)

  --if=name, --ifile      Input file name.  This may be a floppy device or
                          name of a disk image.  On Windows and Unices systems
                          floppy devices 'A:', 'B:', etc. and on Unices
                          '/dev/fd0', '/dev/fd1', etc. may be used. Default
                          for input file will be 'A:' or '/dev/fd0'.

  --if-*                  Conditional processing options for scripting use.
                          If any of the conditionals returns a true result
                          then option processing is enabled, a false result
                          turns processing off until a true condition is met.
                          See the README file for all available options.

  --ignore-errors=x       When copying and Interactive mode is in context all
                          copy errors will be ignored if this option is
                          enabled. x=on to enable, x=off to disable. This
                          action may also be enabled on a per disk copy from
                          the Interactive menu. Default is off.

  --info=x                Determine if an 'info' file should be created. The
                          'info' file uses the output name with '.info'
                          appended. x=on to enable, x=off to disable. Default
                          is on.  The 'info' file contains very important
                          information about the disk image and should always
                          be enabled and the file kept.

  --iside=n               Force side n of input. n may be 0 or 1.

  --it=x, --itype, -i     Set/override the input driver type.  If no type is
                          given the input file name will be used to determine
                          the appropriate type.  The following types are
                          supported by the LibDsk library, see the LibDsk
                          documentation for further details: dsk, edsk, raw,
                          logical, floppy, ntwdm, myz80, cfi, qm, teledisk,
                          imd, nanowasp, rcpmfs, remote.

                          If 'fd' is specified for x then this will be
                          translated to the host system's floppy driver. On
                          Windows this is 'ntwdm' and Unices is the 'floppy'
                          driver.  This is useful for making scripts portable.

  --lcon                  List the [section] names found in the configuration
                          file.
  --lconw                 Same as --lcon option except uses a wide format.
  --lcons=n               Sets the list start point for --lcon and --lconw
                          options. Default value is 1.

  --lformat               Lists all the LibDsk built-in and external disk
                          formats that are available.

  --log=n                 Logging level, set the log level when 'info' files
                          are created to one of the following. Default is 1.

                          0 : No read errors are logged.
                          1 : Read errors are logged only after the retry
                              count expires.
                          2 : All read errors are logged. Very useful for
                              checking for floppy drive and HW problems.

  --ltype                 Lists all the LibDsk driver types that are available.

  --nofill                Normally all tracks from 'start' to 'finish' are
                          written with actual data and the remainder filled
                          in with 0xe5 bytes for the format concerned,  this
                          option forces completion at the 'finish' point
                          selected with no end fill.

  --noformat=n, -n        Set n=1 to prevent or n=0 to allow formatting of
                          the output drive/image.  Default is 0.

  --nomulti=n             Set/Override no multi-track mode. Set n=1 to prevent
                          or n=0 to allow multi-track mode.

  --noskip=n              Set/Override no skipping of deleted data. Set n=1
                          to prevent or n=0 to allow skipping.

  --odstep=x              Determine if the output drive should use double
                          stepping.  x=on to enable, x=off to disable.
                          Default is off. (auto detection may set to on)

  --of=name, --ofile      Output file name.  This may be a floppy device or
                          name of a disk image.  On Windows and Unices systems
                          floppy devices 'A:', 'B:', etc. and on Unices
                          '/dev/fd0', '/dev/fd1', etc. may be used.
                          Output names support substitution '@' characters
                          (see README for full details).

  --oside=n               Force side n of output. n may be 0 or 1.

  --ot=x, --otype, -o     Set/override the output driver type.  If no type is
                          given the output file name will be used to determine
                          the appropriate type.  The following types are
                          supported by the LibDsk library, see the LibDsk
                          documentation for further details: dsk, edsk, raw,
                          logical, floppy, ntwdm, myz80, cfi, qm, teledisk,
                          imd, nanowasp, rcpmfs, remote.

                          If 'fd' is specified for x then this will be
                          translated to the host system's floppy driver. On
                          Windows this is 'ntwdm' and Unices is the 'floppy'
                          driver.  This is useful for making scripts portable.

  --pskew=n,n,n...        Set physical sector skewing for track formatting. A
                          maximum of 256 values are allowed. This will be used
                          by side 0 and side 1 of the disk.
  --pskew0=n,n,n...       As for --pskew but values applies to side 0 only.
  --pskew1=n,n,n...       As for --pskew but values applies to side 1 only.

  --retry-l1=n            Set the number of read tries at the lowest level
                          before level 2 comes into play. Default value is 30
                          for a 'copy' and 5 for a 'scan/speed' command.  1 is
                          the default value if the input type is 'remote'.
  --retry-l2=n, --retry=n Set the 2nd level of read tries before the 3rd level
                          comes into play.  The 3rd and final level is an
                          unattended or interactive mode. Default value is 3.

  --rwgap=n               Set/override the read/write sector gap value.

  --scanhead=s[,f]        Set the head range to be scanned when using the
                          --disk=scan option, 's' is the head to start with,
                          'f' is an optional finish head otherwise finish will
                          be the same as 's'. Default values are 0,1.

  --secbase=n             Set/override the sector base number.

  --secbase2s=n           Set/override the sector base2 number.

  --secbase2c=n           Set/override the sector base2 cylinder start number.

  --secsize=n             Set/override the sector size. Must be >= 128 and
                          a power of 2.

  --sectors=n, -s         Set/override the number of sectors per track

  --sfmode=x              Set the mode for --start and --finish values. x='t'
                          for tracks or x='c' for cylinder mode.  By default
                          the mode is cylinders.  When set to track mode the
                          start and finish values can have finer control.

  --side1as0=n            Set/override the physical properties of side 1
                          sector header ID side information. n=1 indicates the
                          header uses 0 instead of 1. n=0 indicates the side
                          information matches the head 1 number.

  --sidedness=x           This option has been deprecated.
  --sideoffs=n            Set side offset value used in sector headers. Most
                          disks use 0 but some may use an offset for other
                          purposes such as a double sided disk flag. The
                          default value is as defined in the format.

  --signature=name        The archiver's signature can appear in the 'info'
                          file by setting a name.

  --skew=n                Set/override the skew value used by the track read
                          process. This value when correctly set can greatly
                          improve the disk reading speed during the copy
                          process. The default skew is set to 2.
  --skew-ofs=n            Set/override the first physical sector number read in
                          a track read process. The first physical sector will
                          be this value added to the sector base value at the
                          time of reading, if the sector base number is 1
                          (--secbase=1) and n=0 the first sector read will be
                          1. The default value is 1.

  --start=n               Set/override the start cylinder/track for processes.
                          Default this value is set to 0 cylinders but tracks
                          may also be specified by using --sfmode.

  --unattended=x          Use this option to enable/disable automated error
                          handling. x=on to enable, x=off to disable.
                          Default is off.
  --unattended-rab=n      Set number of sector errors allowed before aborting
                          the copy process when the unattended mode is in use.
                          Default value is 300.
  --unattended-rps=n      Set number of retries per sector allowed when the
                          unattended mode is in use.  Default value is 2.
  --unattended-seek=n     Set the maximum number of error seeks per disk read
                          allowed when the unattended mode is in use. After
                          this no more seeks to track 0 will occur between
                          read errors. Default value is 10.

  --verbose=n             Set the reporting level. A higher level includes
                          reporting of any lower levels. The verbose reporting
                          levels are:

                          0  : Only error messages, interaction prompts and
                               processes intended to convey general information
                               will be output.
                          1  : Normal program output reporting (default).
                          >1 : Additional reporting levels.

  --version, -v           Output the program version number to stdout.

If you have any new feature suggestions, bug reports, etc. then post a new
topic at www.microbee-mspp.org.au

Error file structure
====================
By default an error file '.err' will be created if 'info' files are enabled
and any disk errors occur.

Below is an example of an error log file and using a '--log=2' option.  The
example shows where 2 bad sectors caused problems.  An understanding of the
'Unattended mode' and 'THE 3 RETRY LEVELS' is required to follow what is
happening. The example below used the unattended mode and default values for
all retry levels and unattended mode.

The 'Try #' numbers shown are occurring at Level 2.  Level 1 errors are not
reported as these are at the LibDsk level.  The same 3 retries at a
particular sector repeat when the unattended mode attempts further retries.

LOG OF READ ERRORS
------------------
This log contains errors that were encountered during the imaging process.
Read errors may have been corrected on further retries.

The sector count at 'Sector errors' and the 'SECTOR STATUS MAP' in the
'info' file should be used to determine what bad sectors the disk (image)
may have.

The frequency of log entries recorded here is determined by the logging
level option '--log=n'.

Try #: 01/03 Cylinder: 78 Head: 1 Sector: 006 Error: (-15) Missing address mark.
Try #: 02/03 Cylinder: 78 Head: 1 Sector: 006 Error: (-15) Missing address mark.
Try #: 03/03 Cylinder: 78 Head: 1 Sector: 006 Error: (-15) Missing address mark.
Try #: 01/03 Cylinder: 78 Head: 1 Sector: 006 Error: (-15) Missing address mark.
Try #: 02/03 Cylinder: 78 Head: 1 Sector: 006 Error: (-15) Missing address mark.
Try #: 03/03 Cylinder: 78 Head: 1 Sector: 006 Error: (-15) Missing address mark.
Try #: 01/03 Cylinder: 78 Head: 1 Sector: 006 Error: (-15) Missing address mark.
Try #: 02/03 Cylinder: 78 Head: 1 Sector: 006 Error: (-15) Missing address mark.
Try #: 03/03 Cylinder: 78 Head: 1 Sector: 006 Error: (-15) Missing address mark.
Try #: 01/03 Cylinder: 79 Head: 1 Sector: 005 Error: (-13) Data error.
Try #: 02/03 Cylinder: 79 Head: 1 Sector: 005 Error: (-13) Data error.
Try #: 03/03 Cylinder: 79 Head: 1 Sector: 005 Error: (-13) Data error.
Try #: 01/03 Cylinder: 79 Head: 1 Sector: 005 Error: (-13) Data error.
Try #: 02/03 Cylinder: 79 Head: 1 Sector: 005 Error: (-13) Data error.
Try #: 03/03 Cylinder: 79 Head: 1 Sector: 005 Error: (-13) Data error.
Try #: 01/03 Cylinder: 79 Head: 1 Sector: 005 Error: (-13) Data error.
Try #: 02/03 Cylinder: 79 Head: 1 Sector: 005 Error: (-13) Data error.
Try #: 03/03 Cylinder: 79 Head: 1 Sector: 005 Error: (-13) Data error.

Info file structure
===================
'Info' files are created by default when making images from floppy disks.
These 'info' files are important as they contain vital information about
each disk image.  Any bad sectors will be recorded along with other physical
attributes about the disk and the parameters used to make the image.

Read errors will also be appended to this file if '--append-error' has been
specified, the level including turning off error logging can be set using
the '--log' option.

The 'info' file will be the output image file name with '.info' appended.

The following is an info file created from a Microbee DS80 disk.

The command line used under Linux and a Floppyio unit was:

ubeedisk --detect=mbee --if=serial:/dev/ttyACM0,115200-crtscts,a: --of=test-3.0.0.dsk

Under a PC and assuming a drive A: is 3.5" type use:

ubeedisk --detect=mbee --if=a: --of=test-3.0.0.dsk

DISK/IMAGE INFORMATION
----------------------
File output        test-3.0.0.dsk
File output MD5    38836952bb506c095eb73890042433c7
Disk name          ds80
Disk name desc     Microbee DS DD 80T 3.5" (Microbee)
Archived by        uBee
Creation date      24 January 2017  18:28:43

DISK DESCRIPTION
----------------
test disk

PROGRAM PARAMETERS
------------------
Image program      ubeedisk-4.0.0
Detection req      mbee
Disk format req    
File input         ttyACM0,115200-crtscts,a:
Input type         remote
Output type        dsk
Input dstep        no
Output dstep       no
Input side         both
Output side        both
Start cylinder     0
Finish cylinder    79
Start track (log)  0
Finish track (log) 159
Force side         off
Input comp         
Output comp        
Retries lev1 (max) 1
Retries lev2 (max) 3
Logging level      1
Input data rate    sd
Output data rate   sd

UNATTENDED MODE SETTINGS
------------------------
Used unattended    no
Sector ret/abort   300
Retries/sector max 2
Error seeks max    10

PHYSICAL ATTRIBUTES
-------------------
Input drive type   1.44 Mb 3.5"
Output drive type  disk image or unknown
Sidedness          alt
Data rate          sd
Cylinders          80
Heads              2
Sectors/track      10
Sector size        512
Sector base #1     1
Sector base #2     21
Read skew          2
Read skew offset   1

DISK COPY RESULTS
-----------------
Sector errors      0
Sector retries     0

SECTOR STATUS MAP
-----------------
A sector position containing a '.' indicates the sector read correctly first
time. An 'X' indicates the sector was not read (terminated early). An 'ERR'
indicates the sector had a read error and a value from '1-999' indicates the
number of level 2 retries that were needed to successfully read the sector.

Cylinder  Head   (10x512)  001 002 003 004 005 006 007 008 009 010 
   0       0                .   .   .   .   .   .   .   .   .   .  
   0       1                .   .   .   .   .   .   .   .   .   .  
   1       0                .   .   .   .   .   .   .   .   .   .  
   1       1                .   .   .   .   .   .   .   .   .   .  
Cylinder  Head   (10x512)  021 022 023 024 025 026 027 028 029 030 
   2       0                .   .   .   .   .   .   .   .   .   .  
   2       1                .   .   .   .   .   .   .   .   .   .  
   3       0                .   .   .   .   .   .   .   .   .   .  
   3       1                .   .   .   .   .   .   .   .   .   .  
   4       0                .   .   .   .   .   .   .   .   .   .  
   4       1                .   .   .   .   .   .   .   .   .   .  
   5       0                .   .   .   .   .   .   .   .   .   .  
   5       1                .   .   .   .   .   .   .   .   .   .  
   6       0                .   .   .   .   .   .   .   .   .   .  
   6       1                .   .   .   .   .   .   .   .   .   .  
   7       0                .   .   .   .   .   .   .   .   .   .  
   7       1                .   .   .   .   .   .   .   .   .   .  
   8       0                .   .   .   .   .   .   .   .   .   .  
   8       1                .   .   .   .   .   .   .   .   .   .  
   9       0                .   .   .   .   .   .   .   .   .   .  
   9       1                .   .   .   .   .   .   .   .   .   .  
  10       0                .   .   .   .   .   .   .   .   .   .  
  10       1                .   .   .   .   .   .   .   .   .   .  
  11       0                .   .   .   .   .   .   .   .   .   .  
  11       1                .   .   .   .   .   .   .   .   .   .  
  12       0                .   .   .   .   .   .   .   .   .   .  
  12       1                .   .   .   .   .   .   .   .   .   .  
  13       0                .   .   .   .   .   .   .   .   .   .  
  13       1                .   .   .   .   .   .   .   .   .   .  
  14       0                .   .   .   .   .   .   .   .   .   .  
  14       1                .   .   .   .   .   .   .   .   .   .  
  15       0                .   .   .   .   .   .   .   .   .   .  
  15       1                .   .   .   .   .   .   .   .   .   .  
  16       0                .   .   .   .   .   .   .   .   .   .  
  16       1                .   .   .   .   .   .   .   .   .   .  
  17       0                .   .   .   .   .   .   .   .   .   .  
  17       1                .   .   .   .   .   .   .   .   .   .  
  18       0                .   .   .   .   .   .   .   .   .   .  
  18       1                .   .   .   .   .   .   .   .   .   .  
  19       0                .   .   .   .   .   .   .   .   .   .  
  19       1                .   .   .   .   .   .   .   .   .   .  
  20       0                .   .   .   .   .   .   .   .   .   .  
  20       1                .   .   .   .   .   .   .   .   .   .  
  21       0                .   .   .   .   .   .   .   .   .   .  
  21       1                .   .   .   .   .   .   .   .   .   .  
  22       0                .   .   .   .   .   .   .   .   .   .  
  22       1                .   .   .   .   .   .   .   .   .   .  
  23       0                .   .   .   .   .   .   .   .   .   .  
  23       1                .   .   .   .   .   .   .   .   .   .  
  24       0                .   .   .   .   .   .   .   .   .   .  
  24       1                .   .   .   .   .   .   .   .   .   .  
  25       0                .   .   .   .   .   .   .   .   .   .  
  25       1                .   .   .   .   .   .   .   .   .   .  
  26       0                .   .   .   .   .   .   .   .   .   .  
  26       1                .   .   .   .   .   .   .   .   .   .  
  27       0                .   .   .   .   .   .   .   .   .   .  
  27       1                .   .   .   .   .   .   .   .   .   .  
  28       0                .   .   .   .   .   .   .   .   .   .  
  28       1                .   .   .   .   .   .   .   .   .   .  
  29       0                .   .   .   .   .   .   .   .   .   .  
  29       1                .   .   .   .   .   .   .   .   .   .  
  30       0                .   .   .   .   .   .   .   .   .   .  
  30       1                .   .   .   .   .   .   .   .   .   .  
  31       0                .   .   .   .   .   .   .   .   .   .  
  31       1                .   .   .   .   .   .   .   .   .   .  
  32       0                .   .   .   .   .   .   .   .   .   .  
  32       1                .   .   .   .   .   .   .   .   .   .  
  33       0                .   .   .   .   .   .   .   .   .   .  
  33       1                .   .   .   .   .   .   .   .   .   .  
  34       0                .   .   .   .   .   .   .   .   .   .  
  34       1                .   .   .   .   .   .   .   .   .   .  
  35       0                .   .   .   .   .   .   .   .   .   .  
  35       1                .   .   .   .   .   .   .   .   .   .  
  36       0                .   .   .   .   .   .   .   .   .   .  
  36       1                .   .   .   .   .   .   .   .   .   .  
  37       0                .   .   .   .   .   .   .   .   .   .  
  37       1                .   .   .   .   .   .   .   .   .   .  
  38       0                .   .   .   .   .   .   .   .   .   .  
  38       1                .   .   .   .   .   .   .   .   .   .  
  39       0                .   .   .   .   .   .   .   .   .   .  
  39       1                .   .   .   .   .   .   .   .   .   .  
  40       0                .   .   .   .   .   .   .   .   .   .  
  40       1                .   .   .   .   .   .   .   .   .   .  
  41       0                .   .   .   .   .   .   .   .   .   .  
  41       1                .   .   .   .   .   .   .   .   .   .  
  42       0                .   .   .   .   .   .   .   .   .   .  
  42       1                .   .   .   .   .   .   .   .   .   .  
  43       0                .   .   .   .   .   .   .   .   .   .  
  43       1                .   .   .   .   .   .   .   .   .   .  
  44       0                .   .   .   .   .   .   .   .   .   .  
  44       1                .   .   .   .   .   .   .   .   .   .  
  45       0                .   .   .   .   .   .   .   .   .   .  
  45       1                .   .   .   .   .   .   .   .   .   .  
  46       0                .   .   .   .   .   .   .   .   .   .  
  46       1                .   .   .   .   .   .   .   .   .   .  
  47       0                .   .   .   .   .   .   .   .   .   .  
  47       1                .   .   .   .   .   .   .   .   .   .  
  48       0                .   .   .   .   .   .   .   .   .   .  
  48       1                .   .   .   .   .   .   .   .   .   .  
  49       0                .   .   .   .   .   .   .   .   .   .  
  49       1                .   .   .   .   .   .   .   .   .   .  
  50       0                .   .   .   .   .   .   .   .   .   .  
  50       1                .   .   .   .   .   .   .   .   .   .  
  51       0                .   .   .   .   .   .   .   .   .   .  
  51       1                .   .   .   .   .   .   .   .   .   .  
  52       0                .   .   .   .   .   .   .   .   .   .  
  52       1                .   .   .   .   .   .   .   .   .   .  
  53       0                .   .   .   .   .   .   .   .   .   .  
  53       1                .   .   .   .   .   .   .   .   .   .  
  54       0                .   .   .   .   .   .   .   .   .   .  
  54       1                .   .   .   .   .   .   .   .   .   .  
  55       0                .   .   .   .   .   .   .   .   .   .  
  55       1                .   .   .   .   .   .   .   .   .   .  
  56       0                .   .   .   .   .   .   .   .   .   .  
  56       1                .   .   .   .   .   .   .   .   .   .  
  57       0                .   .   .   .   .   .   .   .   .   .  
  57       1                .   .   .   .   .   .   .   .   .   .  
  58       0                .   .   .   .   .   .   .   .   .   .  
  58       1                .   .   .   .   .   .   .   .   .   .  
  59       0                .   .   .   .   .   .   .   .   .   .  
  59       1                .   .   .   .   .   .   .   .   .   .  
  60       0                .   .   .   .   .   .   .   .   .   .  
  60       1                .   .   .   .   .   .   .   .   .   .  
  61       0                .   .   .   .   .   .   .   .   .   .  
  61       1                .   .   .   .   .   .   .   .   .   .  
  62       0                .   .   .   .   .   .   .   .   .   .  
  62       1                .   .   .   .   .   .   .   .   .   .  
  63       0                .   .   .   .   .   .   .   .   .   .  
  63       1                .   .   .   .   .   .   .   .   .   .  
  64       0                .   .   .   .   .   .   .   .   .   .  
  64       1                .   .   .   .   .   .   .   .   .   .  
  65       0                .   .   .   .   .   .   .   .   .   .  
  65       1                .   .   .   .   .   .   .   .   .   .  
  66       0                .   .   .   .   .   .   .   .   .   .  
  66       1                .   .   .   .   .   .   .   .   .   .  
  67       0                .   .   .   .   .   .   .   .   .   .  
  67       1                .   .   .   .   .   .   .   .   .   .  
  68       0                .   .   .   .   .   .   .   .   .   .  
  68       1                .   .   .   .   .   .   .   .   .   .  
  69       0                .   .   .   .   .   .   .   .   .   .  
  69       1                .   .   .   .   .   .   .   .   .   .  
  70       0                .   .   .   .   .   .   .   .   .   .  
  70       1                .   .   .   .   .   .   .   .   .   .  
  71       0                .   .   .   .   .   .   .   .   .   .  
  71       1                .   .   .   .   .   .   .   .   .   .  
  72       0                .   .   .   .   .   .   .   .   .   .  
  72       1                .   .   .   .   .   .   .   .   .   .  
  73       0                .   .   .   .   .   .   .   .   .   .  
  73       1                .   .   .   .   .   .   .   .   .   .  
  74       0                .   .   .   .   .   .   .   .   .   .  
  74       1                .   .   .   .   .   .   .   .   .   .  
  75       0                .   .   .   .   .   .   .   .   .   .  
  75       1                .   .   .   .   .   .   .   .   .   .  
  76       0                .   .   .   .   .   .   .   .   .   .  
  76       1                .   .   .   .   .   .   .   .   .   .  
  77       0                .   .   .   .   .   .   .   .   .   .  
  77       1                .   .   .   .   .   .   .   .   .   .  
  78       0                .   .   .   .   .   .   .   .   .   .  
  78       1                .   .   .   .   .   .   .   .   .   .  
  79       0                .   .   .   .   .   .   .   .   .   .  
  79       1                .   .   .   .   .   .   .   .   .   .  

Physical skewing
================
From v4.0.0 uBeeDisk has --pskew, --pskew0 and --pskew1 options.  These may
be used to change the physical skewing on the floppy disk.  Normally without
these options the skewing will be 1,2,3,4,etc. for both sides of a disk.

The --pskew option sets the skew values for both sides of a disk while the
--pskew0 option sets side 0 values and --pskew1 sets side 1 values.

Here is an example from 'ubeedisk.ini':

[pskewdemo]
--echo="This is a demo/test of physical sector skewing"
--format=default
--description="Test physical skew values"
--cylinders=80
--heads=2
--sectors=5
--pskew0=4,2,5,3,1
--pskew1=1,4,2,5,3

It should be noted that a disk scan may not actually reveal the true sector
ordering.

These --pskew options are not the same as the --skew and --skew-ofs options
which are only used for reading disk sectors to reduce latency.

The internal disk formats built into uBeeDisk may also specify the physical
skew values.

GAP and SYNC values
===================
From v4.0.0 uBeeDisk has a --gapset option.  This may be used when
formatting tracks on a floppy disk to non-standard GAP and/or SYNC lengths
or to disable GAP4 calculations.

It allows a customised track layout to the suit the original disk
hardware/driver.  This option only works for devices that support it. 
Currently only GAP4 can be set for the LibDsk floppy driver but GAPs 1,2,3
and SYNCs 1,2,3 lengths can also be set for devices that support the
Floppyio (v1.2.0 firmware) 'dsk_set_option' using the GAPSET and SYNCSET
internal options.

If those internal options are not supported by a device it may return a not
known error but the error is ignored by uBeeDisk in which case the requested
GAP or SYNC settings won't be changed.

The --gapset option is as follows:

--option=pe:v,[pe:v...]

'p' is the parameter type and may be 'g' for a GAP or 's' for a SYNC
parameter.  'v' is the length value.  Multiple parameters and values may be
specified with the one --gapset option separated by commas.

Here is an actual example taken from 'ubeedisk.ini' for the [atari-11s]
macro.  The spaces are optional to make it easier to read and if used must
also be double quoted:

--gapset="g1:10,s1:3, g2:22,s2:12, g3:0,s3:0, g4:1"

GAP4 (g4) value by default is 0xefff.  This setting is used to enable GAP4
calculation by the supported device.  uBeeDisk also uses it to do
calculations internally for the LibDsk floppy driver.

If GAP4 is set to 0xeffe then this will force the device driver to use the
'dg_fmtgap' length value in the LibDsk disk geometry.

Any other value for GAP4 is used as the length value.

GAP 1-3 and SYNC 1-3 do not have any special reserved length values so
any values 0x0000 - 0x3fff (0- 16383) are allowed to be specified.

The internal disk formats built into uBeeDisk may also specify GAP and SYNC
values.

Buffering and Cache read/write control
======================================
uBeeDisk uses a read buffer when reading sectors of the disk.  Normally this
will read an entire track into memory holding the required sector.  Each
time a sector from the same cylinder and head is read it will come from the
buffer rather than from a new read from the disk.  This greatly improves
read speed especially with appropriate skew values (--skew, --skew-ofs). 
The only exception is when a read error occurs then any other sectors on the
same cylinder/head read will come straight from the disk.

The buffering was introduced to be used with the LibDsk PC floppy driver.

Better buffering (caching) can be achieved by devices that handle this at
the lowest level as entire tracks can be buffered without any need for
skewing.  Such is the case with the Floppyio.  There are cases though when
the low level caching can actually slow down performance if the tracks were
formatted with small GAP4 values as the floppy controller circuit can't
detect the following sector in time it may have to wait for an entire disk
revolution to before it can be read.  In such cases read or write caching
might need to be disabled.

The --cacher and --cachew options may be used to enable/disable read or
write caching by devices that support the Floppyio 'dsk_set_option' using
the RBUFFER and WBUFFER internal options.

The internal disk formats built into uBeeDisk may also disable read/write
caching on a per format basis.

File name substitution
======================
Modify the output file name using substitution.

This allows some LibDsk geometry and other values to be inserted into the
file name in a readable form.  A substitution is carried out when a '@'
character is found in the file name specified.  The single character that
follows then determines the substitution value.  Normal text characters may
be used in and around substitutions.

There are three special "command" characters that are used to determine if
the substitution is to use upper or lower case characters and one for
terminating a concatenated substitution sequence (internal use).

This is very useful for use with --detect=dos and any non-raw disk types so
that the actual format can be included in the file name.

Substitution values
-------------------
@h sided SS/DS
@H heads
@d density SD/DD/HD
@f recording format FM/MFM
@c number of cylinders
@t sectors per track
@s sector size
@m media descriptor byte (DOS disks only)
@n name of media format (i.e. ds40, ds80, applix80, dos)
@@ inserts a '@' character

Substitution command values
---------------------------
@u following substitutions will be in upper case
@l following substitutions will be in lower case (default)
@z terminates a concatenated substitution sequence (for internal use)

Concatenated substitutions sequences
------------------------------------
@0 ->               @h@d_@f_@cx@tx@s_@m@z     ('_@m' only if --detect=dos)
@1 ->               @n_@h@d_@f_@cx@tx@s_@m@z  ('_@m' only if --detect=dos)
@2 ->               @n_@h@d_@f_@cx@tx@s@z
@3 ->               @n_@h@d_@cx@tx@s@z
@4 ->               @n_@cx@tx@s@z
@5 ->               @cx@tx@s@z
@r ->               .fdd-@c-@H-@t-@s@z        (uBee512 RAW images)

Examples for a 360k DOS disk and DSK image
------------------------------------------
@1.dsk ->           dos_dsdd_mfm_40x9x512_(FD).dsk
@2.dsk ->           dos_dsdd_mfm_40x9x512.dsk
@3.dsk ->           dos_dsdd_40x9x512.dsk
@4.dsk ->           dos_40x9x512.dsk
@5.dsk ->           40x9x512.dsk
blah_@1_blah.dsk -> blah_dos_dsdd_mfm_40x9x512_(FD)_blah.dsk

Examples for a Microbee CIAB DSK image converted to RAW format
--------------------------------------------------------------
ubeedisk --format=ss80 --if=ciab.dsk --otype=raw --of=disk@r
ubeedisk --format=ss80 --if=ciab.dsk --otype=raw --of=@n@r

Configuration Files
===================
The configuration file uses the same options that are possible on the
command line.  Customised sections can be added making invoking ubeedisk
with options much easier.  Command line parameters may be used in addition
and more than one customised section can be invoked from the same command
line.

A 'ubeedisk.ini' text file (the default) is required to hold the
configuration script. The supplied 'ubeedisk.ini.sample' file can be used as
a template.

Alternative configuration files may also be used if the first option on the
command line is --config=my_config.

The default path for configuration files is where the ubeedisk.exe program
is located on Windows and in the '.ubeedisk' directory on Unix home
accounts.

From v4.0.0 uBeeDisk checks the script version and warns the user if it is
older than the uBeeDisk version.  To disable the warning use --confv=none in
the script.  At minimum a configuration file 'ubeedisk.ini' is required to
avoid the script not found warning message:

[global-start]
--confv=none

File structure
--------------
- Comments lines are preceded by '#' or ';' and must be the first non-space
  character on the line. Blank lines are ignored.

- A definition name must be surrounded with square bracket characters '['
  and ']'. All lines below this are part of the definition until a new
  definition name or the end of file is reached.  A maximum of 5000
  definitions are allowed.

- A definition's contents can NOT reference any other definitions.  The
  command line options may include any number of references to definitions
  contained in the file and may be mixed with options.

- Values contained in the [global-start] and [global-end] sections are
  always referenced but their use is optional.  It is not the intention that
  these be used directly by the user.  The [global-start] section is applied
  before any command line options or customised sections are processed.  The
  [global_end] section allows options to override what resulted from a
  command line or a customised section.

- Names used for a definition section must be unique.  If any command line
  option parameters that contains a space before it matches the definition
  name then it will be treated as file definition instead.

- Options may be divided up and placed onto new lines to make for easier
  reading.

Listing the definitions
-----------------------
Use the --lcon or lconw option or use the 'listall' entry found in the
sample file to list all the definition names contained in the configuration
file.  A 'list' entry is also included that will list all entries except for
some early administration entries.

Accessing files
===============
The files on disk images can be accessed using various tools.

IMPORTANT: It should be pointed out that once a floppy disk image has been
created that it is never written to again if it for archival purposes.  This
means DO NOT use the disk image under an emulator or by any other tools that
may write to the disk image.  Make a working copy(s) of the disk image and
only access that.  It is also important to archive the '.info' files that
ubeedisk creates as it records any errors in sectors.

CP/M disk images
----------------
For CP/M disk images there is 'cpmtools' or uBee's patched version of it
for Microbee disks that targets some difficult formats.

DOS/Atari ST/MSX disk images
----------------------------
For DOS/Atari ST/MSX type disk images use 'mtools'.  This is a Unix program
but there may be builds available for Windows.  It only needs to be able to
access RAW disk images so does not need to have floppy support built in.

Under Unix it is recommended to have the file '.mtoolsrc' file in the user's
home directory which contain the following line:

mtools_skip_check=1

This will make 'mtools' ignore unknown media formats which should allow all
Atari ST and MSX-DOS disk images to also be accessed.  Then use one of the
'mtools' programs with "-i imagename.raw" to access it.

Applix disk images
------------------
For Applix disks use 'apptools' written by the uBeeDisk author.  This is
similar to 'mtools' and 'cpmtools'.  Builds are available for Linux and
Windows systems.

Emulators
---------
Emulators may also be able to access files on disk images but please read the
IMPORTANT note above first.

Projects by this ubee
=======================

uBee512
-------
An emulator for the Microbee Z80 ROM, FDD and HDD based models.
https://discord.gg/SYzQG2XnAz

uBeeDisk
--------
A tool for converting disks/images from one to another with
auto detection options for Microbee disks.

https://discord.gg/RRbfkT3duy

z80em86
-------
A Z80 CPU emulator coded in Intel 86 assembly language.

http://www.cpm.z80.de/source.html
http://www.gaby.de/z80/z80emu.htm

Contact
=======
If you have any new feature suggestions, bug reports, etc. then please post
in Discord at https://discord.gg/RRbfkT3duy

For bug reports please provide the following information:
1) Version number of the uBeeDisk program.
2) The platform it was running on, for example if running on Windows then
   state if W95, W98, Me, W2000, XP, Vista, W7, etc.  Same for Linux, state
   the actual Linux distribution used and version.
3) Command line start up options used (cut and paste if possible)
4) Description of the problem.

# Build for Windows #
- C: make

# Build for macOS #
- % brew install libb2
- % make


## Test with this ##
- % ubeedisk --detect=mbee3 --if=original.dsk --of=copy.dsk



# Build for Linux #
- $ make
