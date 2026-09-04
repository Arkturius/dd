#
# Makefile
#

NAME		:=	didier

INC_DIR		:=	include
SRC_DIR		:=	src
OBJ_DIR		:=	.build

include sources.mk

SRCS		:=	$(addprefix $(SRC_DIR)/, $(SRCS))
OBJS		:=	$(addprefix $(OBJ_DIR)/, $(SRCS:%.c=%.o))

CC			:=	clang -std=gnu2y
CFLAGS		:=	-Wall -Wextra -gdwarf-2
LDFLAGS		:=
IFLAGS		:=	-I$(INC_DIR)

RM			:=	rm -rf
DIR_DUP		:=	mkdir -p

DEBUG		?=	0
ifeq ($(DEBUG), 1)
	CFLAGS	+=	-fsanitize=address -static-libsan
endif

all:		$(NAME)

debug:
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory DEBUG=1

$(NAME):	$(OBJS)
	@printf $(MSG_LINK) $(notdir $@)
	@$(CC) $(IFLAGS) $^ -o $@ $(LDFLAGS) 

$(OBJ_DIR)/%.o:	%.c
	@$(DIR_DUP) $(@D)
	@printf $(MSG_COMP) $(notdir $@) $<
	@$(CC) $(IFLAGS) $(CFLAGS) -o $@ -c $<

clean:
	@printf $(MSG_RMRF) $(notdir $(OBJ_DIR))
	@$(RM) $(OBJ_DIR)

fclean:				clean
	@printf $(MSG_RMRF) $(notdir $(NAME))
	@$(RM) $(NAME)

re:					fclean all

#
# Strings
#

RED			:=	"\\033[31m"
GREEN		:=	"\\033[32m"
CYAN		:=	"\\033[36m"
GREY		:=	"\\033[90m"

BOLD		:=	"\\033[1m"
ITALIC		:=	"\\033[3m"

RESET		:=	"\\033[0m"

COLOR_COMP	:=	$(BOLD)$(ITALIC)$(GREY)
COLOR_LINK	:=	$(BOLD)$(ITALIC)$(GREEN)
COLOR_RMRF	:=	$(BOLD)$(ITALIC)$(RED)

MSG_COMP	:=	" $(COLOR_COMP)■$(RESET) compiling $(COLOR_COMP)%-16s$(RESET) from $(COLOR_COMP)%s$(RESET)\n"
MSG_LINK	:=	" $(COLOR_LINK)■$(RESET) linking   $(COLOR_LINK)%-16s$(RESET)\n"
MSG_RMRF	:=	" $(COLOR_RMRF)■$(RESET) deleting  $(COLOR_RMRF)%-16s$(RESET)\n"
