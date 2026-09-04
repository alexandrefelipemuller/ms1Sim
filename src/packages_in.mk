EXEEXT		=

# MegaSquirtSimulator builds only the m68hc08 core (MC68HC908GP32 target
# used by MS1/Slowduino-HC08 firmware). Other uCsim architectures were
# removed from this fork; see git history if any are ever needed again.

M68HC08		= m68hc08.src

PKGS		= $(M68HC08)

PKGS_ALL	= m68hc08.src

curses_ok	= @curses_ok@

enable_serio        = yes
enable_feapp        = yes

ifeq ($(enable_serio),yes)
ifeq ($(curses_ok),yes)
SERIO		= serio.src
else
SERIO		=
endif
else
SERIO		=
endif

ifeq ($(enable_feapp),yes)
UCSIM		= ucsim.src
else
UCSIM		=
endif

APPS		= $(SERIO) $(UCSIM)

APPS_ALL	= serio.src ucsim.src
