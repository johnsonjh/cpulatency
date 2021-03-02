TARGET	= cpulatency
SRCEXT	= .c
SVC		= .service
CC		= clang
CP		= cp -f
RM		= rm -f
STRIP	= strip
SUDO	= sudo
CFLAGS	= -s \
		  -std=gnu99 \
		  -Oz \
		  -Wall -Wextra -Werror \
		  -pedantic \
		  -fomit-frame-pointer

.PHONY: all clean strip install

all: $(TARGET)

clean:
	@printf '%s\n' "Cleaning up ..."
	@$(RM) $(TARGET)

$(TARGET):
	@printf '%s\n' "Compile $(TARGET) ..."
	@$(CC) $(CFLAGS) -o $(TARGET) $(TARGET)$(SRCEXT) && \
		printf '%s\n' "Compile successful!"

strip: $(TARGET)
	@printf '%s\n' "Stripping $(TARGET) ..."
	@$(STRIP) --strip-all \
		-R .gnu.build.attributes \
		-R .note.gnu.build-id \
		-R .gnu.hash \
		-R .comment \
		-R .note.ABI-tag \
		-R .gnu.version \
		-R .eh_frame_hdr \
		-R .eh_frame \
		-R .shatrtab $(TARGET) && \
		printf '%s\n' "Stripping successful!"

install: strip
	@printf '%s\n' "Stopping $(TARGET)$(SVC) ..."
	@$(SUDO) systemctl stop $(TARGET)$(SVC) > /dev/null 2>&1
	@printf '%s\n' "Installing $(TARGET) ..."
	@$(SUDO) $(CP) $(TARGET) /usr/local/sbin/$(TARGET)
	@printf '%s\n' "Installing $(TARGET)$(SVC) ..."
	@$(SUDO) $(CP) $(TARGET)$(SVC) /usr/lib/systemd/system/$(TARGET)$(SVC)
	@printf '%s\n' "Starting $(TARGET)$(SVC) ..."
	@$(SUDO) systemctl daemon-reload && \
		$(SUDO) systemctl enable $(TARGET)$(SVC) && \
		$(SUDO) systemctl start $(TARGET)$(SVC) && \
		systemctl is-active $(TARGET)$(SVC) > /dev/null 2>&1 && \
		printf '%s\n' "$(TARGET)$(SVC) successfully installed!"

