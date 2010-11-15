###############################################################################
#
#	Modified by KPL, few more flags added to compilation/linking;
#	section "burn" added, so command "make burn" could compile and link sources,
#	and then burn them to PIC, using pk2cmd and pickit2 programmer.
#
# PIC compile makefile for use with SDCC and GPUTILS
# You can find an dummy projects at http://szokesandor.hu/sdcc
#
# C2006.12.02. [EMAIL PROTECTED]
# C2008.03.22. [EMAIL PROTECTED]
#
# Please send bug reports or comments to the author.
# If you improved this makefile, please send the updates back to the author!
#
# Licensed under the GNU GPL V2 or later
#
#
#----------------------------------------------------------------------------
# Name of the .hex file without extention
#
PRJ          = daylight
#
#-----------------------------------------------------------------------------
# Used modules, the first should contain the main() function
# Assembly modules should have extention ".S", because the current clean up!
#
SRCS_C       := main.c
SRCS_ASM     :=
#
#----------------------------------------------------------------------------
# Type of the used cpu
#
CPU          = 16f88
#
#----------------------------------------------------------------------------
# Type of the architecture eg. -mpic14 or -mpic16
#
ARCH         = pic14
#
#----------------------------------------------------------------------------
# Adjust to sdcc data dir and to gputils dir.
# Normally they are installed into /usr/share
# In my case I have built sdcc under my home.
#
SDCCDIR      = /usr/share/sdcc
GPUTILSDIR   = /usr/share/gputils
#
#----------------------------------------------------------------------------
# Adjust according to architecture 14bit (pic) or 16bit (pic16)
#
#INCLUDEDIR   = $(SDCCDIR)/include/pic
#LIBDIR       = $(SDCCDIR)/lib/pic

INCLUDEDIR   = $(SDCCDIR)/include/pic
LIBDIR       = $(SDCCDIR)/lib/pic
#
#----------------------------------------------------------------------------
# Defining the tools, you may have several version installed
# for different reasons (testing, production, etc.)
#
SDCC         = sdcc
GPASM        = gpasm
GPDASM       = gpdasm
GPLINK       = gplink
PROGRAMMER   = pk2cmd
#
#----------------------------------------------------------------------------
# We should link against these libraries.
# If custom startup code is used crt0i.o should be removed and
# the new crt module should be added to the SRCS_ASM line.
#
LIBS         = libsdcc.lib pic$(CPU).lib
#
#----------------------------------------------------------------------------
# Flags sdcc:
#   -c :compile only
#   -S :stop after assembly
#   -V : show actual command line the compiler executing
#   --verbose: Shows the various actions the compiler is performing
#
# Flags for gplink:
#   -c :
#   -m :
#   -w :
#   -r :
#   -I dir:
#   -s file: the linker file
SDCC_FLAGS   =  -S -m$(ARCH) -p$(CPU) --debug -c
GPASM_FLAGS  = -g
GPLINK_FLAGS = -c -m -w -r -I $(LIBDIR) -s $(GPUTILSDIR)/lkr/$(CPU).lkr
PROGRAMMER_FLAGS = -Ppic$(CPU) -J 
RUN_VOLTAGE = 5.0
#
###############################################################################
#
# NO USER SERVICABLE SETTING BELOW THIS LINE!
#
# Changes hereafter can totally break your build process.
# Modify only, if you know what are you doing!
# If you improved this makefile, please send the updates back to the author!
#
DEPDIR       = .deps
DEPFILES     = $(DEPDIR)/$(*F)
OBJS_C       := $(SRCS_C:.c=.o)
OBJS_ASM     = $(SRCS_ASM:.asm=.o)
OBJS         = $(OBJS_C) $(OBJS_ASM)
#
#----------------------------------------------------------------------------
# Only the generated asm files will be deleted
#
CLEAN_ASMS   = $(SRCS_C:.c=.asm)
#
#----------------------------------------------------------------------------
# Redefine the compiler
#
CC           = $(SDCC)
#
#----------------------------------------------------------------------------
# For dependancy, we use makedepend a bit sligthly modified
#
MAKEDEPEND   = touch $(DEPFILES).d && makedepend -I$(INCLUDEDIR) -Dpic$(CPU) -f$(DEPFILES).d $<
#
##############################################################################
# Main target
#
all: $(PRJ).hex
#all: burn
#
##############################################################################
# Linking the modules alltogether
#
$(PRJ).hex: $(OBJS)
	@echo "------ Linking of $(PRJ).hex started."
	$(GPLINK) $(GPLINK_FLAGS) -o $(PRJ).hex $(OBJS) $(LIBS)
	$(GPDASM) -p$(CPU) $(PRJ).hex > $(PRJ).dasm
	@echo "------ Linking of $(PRJ).hex finished."
#
##############################################################################
# Compiling and assembling the modules, with dependencies to its include files
#
# fixme: use only pipe for creating dependancy files
# compile through asm
#
%.o: %.c
	@mkdir -p $(DEPDIR)
	@$(MAKEDEPEND); \
	cp $(DEPFILES).d $(DEPFILES).dep; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	    -e '/^$$/ d' -e 's/$$/ :/' < $(DEPFILES).d >> $(DEPFILES).dep; \
	rm -f $(DEPFILES).d $(DEPFILES).d.bak
	@echo "------ Compilation of $< started."
	$(CC) $(SDCC_FLAGS) $<
	$(GPASM) $(GPASM_FLAGS) -c $*.asm
	@echo "------ Compilation of $< finished."
##############################################################################
# compiling standalone asm files into object file if needed
#
%.o: %.asm
	$(GPASM) $(GPASM_FLAGS) -c $<
#----------------------------------------------------------------------------
# Include generated dependancy to depend on.
#
-include $(SRCS:%.c=$(DEPDIR)/%.dep)
#
#----------------------------------------------------------------------------
# Cleaning the folder, deleting all the files that can be regenerated
#
clean:
	-rm -f *.o $(CLEAN_ASMS) *.lst *.p *.d *.cod *.hex *.c~ *.asm~ *.map \
	*.cof *.dasm *.dump* $(DEPFILES)* *.adb
	-rmdir $(DEPDIR)
	@echo "------ Project directory cleaned."

#
#----------------------------------------------------------------------------
# Programmer functions
# 
program: $(PRJ).hex
	@echo "------ Burning of $(PRJ).hex started."
	$(PROGRAMMER) $(PROGRAMMER_FLAGS) -M -F$(PRJ).hex
	@echo "------ Burning of $(PRJ).hex finished."

run:
	$(PROGRAMMER) $(PROGRAMMER_FLAGS) -A$(RUN_VOLTAGE) -T -R

stop:
	$(PROGRAMMER) $(PROGRAMMER_FLAGS) -A$(RUN_VOLTAGE) -R

verify:
	$(PROGRAMMER) $(PROGRAMMER_FLAGS) -R -Y -F$(PRJ).hex

.PHONY: all clean dist
#
# end of makefile 
###############################################################################