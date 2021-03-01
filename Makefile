TARGET	= cpulatency
SRCEXT	= .c
SVC		= .service
CC		= clang
CP		= cp -f
RM		= rm -f
STRIP	= strip
SUDO	= sudo
CFLAGS	= -s -std=gnu99 -Oz -Wall -Wextra -Werror -pedantic -fomit-frame-pointer

all: $(TARGET)

clean:
	@printf '%s\n' "Cleaning up ..."
	@$(RM) $(TARGET)

$(TARGET):
	@printf '%s\n' "Compile $(TARGET) ..."
	@$(CC) $(CFLAGS) -o $(TARGET) $(TARGET)$(SRCEXT) && \
		printf '%s\n' "Compile successful!"

# Aggressive stripping crashes on exit due to stripping the
# .fini_array and .fini sections, but saves up to 16 bytes.
strip: $(TARGET)
	@printf '%s\n' "Stripping $(TARGET) ..."
	@$(STRIP) --strip-all -R .gnu.build.attributes -R .note.gnu.build-id -R .gnu.hash -R .comment -R .note.ABI-tag -R .gnu.version -R .eh_frame_hdr -R .eh_frame -R .shatrtab -R .fini_array -R .fini $(TARGET) && \
		printf '%s\n' "Stripping successful!"

install: strip
	@printf '%s\n' "Stopping running service ..."
	@$(SUDO) systemctl stop $(TARGET)$(SVC) > /dev/null 2>&1
	@printf '%s\n' "Installing $(TARGET) ..."
	@$(SUDO) $(CP) $(TARGET) /usr/local/sbin/$(TARGET)
	@printf '%s\n' "Installing $(TARGET)$(SVC) ..."
	@$(SUDO) $(CP) $(TARGET)$(SVC) /usr/lib/systemd/system/$(TARGET)$(SVC)
	@printf '%s\n' "Activating $(TARGET)$(SVC) ..."
	@$(SUDO) systemctl daemon-reload && \
		$(SUDO) systemctl enable $(TARGET)$(SVC) && \
		$(SUDO) systemctl start $(TARGET)$(SVC) && \
		systemctl is-active $(TARGET)$(SVC) > /dev/null 2>&1 && \
		printf '%s\n' "Success! $(TARGET)$(SVC) active!"

